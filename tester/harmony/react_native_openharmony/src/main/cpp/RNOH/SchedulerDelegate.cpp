/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "SchedulerDelegate.h"
#include <react/renderer/debug/SystraceSection.h>
#include "ParallelComponent.h"
#include "RNOH/FFRTConfig.h"
#include "RNOH/ParallelCheck.h"
#include "RNOH/Performance/HarmonyReactMarker.h"
#include "ffrt/cpp/pattern/job_partner.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <glog/logging.h>
#include <native_vsync/native_vsync.h>
#include <sstream>
#include <string>
#include <string_view>

namespace {

class NextFrameDispatcher {
 public:
  static NextFrameDispatcher& Get() {
    static NextFrameDispatcher instance;
    return instance;
  }

  void post(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_pending.emplace_back(std::move(task));
    }
    requestIfNeeded();
  }

 private:
  NextFrameDispatcher() {
    const char* name = "RNOH::SchedulerDelegate";
    m_vsync = OH_NativeVSync_Create(
        name, static_cast<unsigned int>(std::strlen(name)));
  }

  ~NextFrameDispatcher() {
    if (m_vsync != nullptr) {
      OH_NativeVSync_Destroy(m_vsync);
      m_vsync = nullptr;
    }
  }

  static void onVsync(long long /*timestamp*/, void* data) {
    auto* self = static_cast<NextFrameDispatcher*>(data);
    self->m_requestInFlight.store(false, std::memory_order_release);
    self->flush();
  }

  void requestIfNeeded() {
    bool expected = false;
    if (!m_requestInFlight.compare_exchange_strong(
            expected,
            true,
            std::memory_order_acq_rel,
            std::memory_order_relaxed)) {
      return;
    }

    if (m_vsync != nullptr) {
      (void)OH_NativeVSync_RequestFrame(
          m_vsync, &NextFrameDispatcher::onVsync, this);
      return;
    }

    m_requestInFlight.store(false, std::memory_order_release);
    flush();
  }

  void flush() {
    std::vector<std::function<void()>> tasks;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      tasks.swap(m_pending);
    }
    for (auto& t : tasks) {
      t();
    }
    bool hasMore = false;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      hasMore = !m_pending.empty();
    }
    if (hasMore) {
      requestIfNeeded();
    }
  }

 private:
  std::mutex m_mutex;
  std::vector<std::function<void()>> m_pending;
  std::atomic<bool> m_requestInFlight{false};
  OH_NativeVSync* m_vsync{nullptr};
};

struct TwoBatchSplit {
  bool enabled{false};
  facebook::react::ShadowViewMutationList batchA;
  facebook::react::ShadowViewMutationList batchB;
};

using SteadyClock = std::chrono::steady_clock;

// ---- SchedulerDelegate tuning constants (avoid magic numbers) ----
constexpr size_t SURFACE_LOAD_MAX_ENTRIES = 128;
constexpr int SURFACE_LOAD_TTL_MINUTES = 2;

constexpr size_t NONCREATE_SPLIT_MIN_COUNT = 60;
constexpr int NONCREATE_SPLIT_MIN_COST = 120;

constexpr size_t GROUPS_RESERVE_HINT = 16;
constexpr size_t FENCE_RESERVE_HINT = 64;

constexpr size_t TWO_BATCHES = 2;

// Non-create mutation cost weights (rough heuristic).
constexpr int COST_DEFAULT = 2; // Update / miscellaneous
constexpr int COST_INSERT = 3;
constexpr int COST_REMOVE = 3;
constexpr int COST_DELETE = 4;
// ---------------------------------------------------------------

struct SurfaceLoadState {
  SteadyClock::time_point firstSeen{};
  SteadyClock::time_point lastSeen{};
  uint32_t txCount{0};
};

static std::mutex g_surfaceLoadMutex;
static std::unordered_map<facebook::react::SurfaceId, SurfaceLoadState>
    g_surfaceLoad;

static void doPruneSurfaceLoad(
    SteadyClock::time_point now,
    std::chrono::minutes ttl) {
  for (auto it = g_surfaceLoad.begin(); it != g_surfaceLoad.end();) {
    if (now - it->second.lastSeen > ttl) {
      it = g_surfaceLoad.erase(it);
    } else {
      ++it;
    }
  }
}

static inline void pruneSurfaceLoadLocked(SteadyClock::time_point now) {
  if (g_surfaceLoad.size() <= SURFACE_LOAD_MAX_ENTRIES) {
    return;
  }

  const auto ttl = std::chrono::minutes(SURFACE_LOAD_TTL_MINUTES);
  doPruneSurfaceLoad(now, ttl);
}

static bool isInInitialLoadWindowImpl(facebook::react::SurfaceId surfaceId) {
  if (surfaceId == 0) {
    return false;
  }

  constexpr int64_t LOAD_WINDOW_MS = 3000; // 3s
  constexpr uint32_t LOAD_WINDOW_MAX_TX = 3; // first 3 transactions

  const auto now = SteadyClock::now();
  std::lock_guard<std::mutex> lock(g_surfaceLoadMutex);
  pruneSurfaceLoadLocked(now);

  auto& st = g_surfaceLoad[surfaceId];
  if (st.firstSeen == SteadyClock::time_point{}) {
    st.firstSeen = now;
    st.txCount = 0;
  }
  st.lastSeen = now;
  ++st.txCount;

  const auto elapsedMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - st.firstSeen)
          .count();
  return elapsedMs < LOAD_WINDOW_MS || st.txCount <= LOAD_WINDOW_MAX_TX;
}

static inline bool isInInitialLoadWindow(facebook::react::SurfaceId surfaceId) {
  return isInInitialLoadWindowImpl(surfaceId);
}

static int estimateNonCreateCostImpl(
    const facebook::react::ShadowViewMutation& m) {
  using M = facebook::react::ShadowViewMutation;
  switch (m.type) {
    case M::Create:
      return 0;
    case M::Insert:
      return COST_INSERT;
    case M::Remove:
      return COST_REMOVE;
    case M::Delete:
      return COST_DELETE;
    case M::Update:
    default:
      return COST_DEFAULT;
  }
}

static inline int estimateNonCreateCost(
    const facebook::react::ShadowViewMutation& m) {
  return estimateNonCreateCostImpl(m);
}

static facebook::react::Tag getTargetTagImpl(
    const facebook::react::ShadowViewMutation& m) {
  using M = facebook::react::ShadowViewMutation;
  switch (m.type) {
    case M::Create:
    case M::Insert:
    case M::Update:
      return m.newChildShadowView.tag;
    case M::Remove:
    case M::Delete:
      return m.oldChildShadowView.tag;
    default:
      return (m.oldChildShadowView.tag != 0) ? m.oldChildShadowView.tag
                                             : m.newChildShadowView.tag;
  }
}

static inline facebook::react::Tag getTargetTag(
    const facebook::react::ShadowViewMutation& m) {
  return getTargetTagImpl(m);
}

static inline bool isStructuralWithParent(
    const facebook::react::ShadowViewMutation& m) {
  using M = facebook::react::ShadowViewMutation;
  return m.type == M::Insert || m.type == M::Remove;
}

static TwoBatchSplit splitNonCreateMutationsIntoTwoBatches(
    const facebook::react::ShadowViewMutationList& nonCreate) {
  facebook::react::SystraceSection s(
      "#RNOH::SchedulerDelegate::splitNonCreateMutationsIntoTwoBatches");

  TwoBatchSplit out;
  const size_t n = nonCreate.size();

  // Pre-scan: used to (1) distinguish real "move" (Remove+Insert) from "delete"
  // (Remove+Delete), and (2) protect Insert+Update chains for the same tag from
  // being split across frames.
  std::unordered_set<facebook::react::Tag> willInsertTags;
  willInsertTags.reserve(n);
  std::unordered_map<facebook::react::Tag, size_t> lastUpdateIndex;
  lastUpdateIndex.reserve(n);

  int quickTotalCost = 0;
  for (size_t i = 0; i < n; ++i) {
    const auto& m = nonCreate[i];
    quickTotalCost += estimateNonCreateCost(m);

    using M = facebook::react::ShadowViewMutation;
    const auto tag = getTargetTag(m);
    if (m.type == M::Insert) {
      willInsertTags.insert(tag);
    } else if (m.type == M::Update) {
      lastUpdateIndex[tag] = i; // keep last
    }
  }

  // Small & cheap batches don't benefit from extra frame; keep single batch.
  // (Kept compatible with the original guard, but allow splitting if cost is
  // high even when n is small.)
  if (n < NONCREATE_SPLIT_MIN_COUNT &&
      quickTotalCost < NONCREATE_SPLIT_MIN_COST) {
    out.enabled = false;
    out.batchA = nonCreate;
    return out;
  }

  struct Group {
    size_t start{0};
    size_t end{0};
    int cost{0};
  };

  std::vector<Group> groups;
  groups.reserve(GROUPS_RESERVE_HINT);

  size_t groupStart = 0;
  int groupCost = 0;

  // openMovedTags: tags that have been Removed and are expected to be Inserted
  // later (i.e. Move).
  std::unordered_set<facebook::react::Tag> openMovedTags;
  openMovedTags.reserve(FENCE_RESERVE_HINT);

  // pendingInitTags: tags that have been Inserted but still have Updates later
  // in this same transaction. We store their last update index and prevent
  // boundary split until those updates are consumed.
  std::unordered_map<facebook::react::Tag, size_t> pendingInitTags;
  pendingInitTags.reserve(FENCE_RESERVE_HINT);

  constexpr int SOFT_SAMEPARENT_BREAK_COST =
      90; // allow breaking long Insert/Remove runs under same parent

  auto pushGroup = [&groups, &groupStart, &groupCost](size_t endExclusive) {
    if (endExclusive <= groupStart) {
      return;
    }
    groups.push_back(Group{groupStart, endExclusive, groupCost});
    groupStart = endExclusive;
    groupCost = 0;
  };

  for (size_t i = 0; i < n; ++i) {
    const auto& m = nonCreate[i];
    groupCost += estimateNonCreateCost(m);

    {
      using M = facebook::react::ShadowViewMutation;
      const auto tag = getTargetTag(m);

      // Distinguish "move" vs "delete":
      // - Only treat Remove as "move-open" if we know there will be a matching
      // Insert later.
      // - If it's a delete chain (Remove+Delete), do not block all boundaries.
      if (m.type == M::Remove) {
        if (willInsertTags.find(tag) != willInsertTags.end()) {
          openMovedTags.insert(tag);
        }
        // For safety: if an inserted tag gets removed, it no longer needs init
        // protection.
        pendingInitTags.erase(tag);
      } else if (m.type == M::Insert) {
        // Close move-open when we see the matching Insert.
        auto it = openMovedTags.find(tag);
        if (it != openMovedTags.end())
          openMovedTags.erase(it);

        // Protect Insert+Update chain: if this tag has Updates later, keep it
        // within the same group until last Update.
        auto uIt = lastUpdateIndex.find(tag);
        if (uIt != lastUpdateIndex.end() && uIt->second > i) {
          pendingInitTags[tag] = uIt->second;
        }
      } else if (m.type == M::Update) {
        // If this Update is the last Update for a tag we inserted, the init
        // fence is closed here.
        auto pIt = pendingInitTags.find(tag);
        if (pIt != pendingInitTags.end() && pIt->second == i) {
          pendingInitTags.erase(pIt);
        }
      } else if (m.type == M::Delete) {
        // Remove+Delete is NOT a move; make sure it doesn't keep openMovedTags
        // stuck.
        openMovedTags.erase(tag);
        pendingInitTags.erase(tag);
      }
    }

    if (i + 1 < n) {
      const auto& next = nonCreate[i + 1];

      bool sameParentStructuralBlock = false;
      if (isStructuralWithParent(m) && isStructuralWithParent(next)) {
        if (m.parentShadowView.tag == next.parentShadowView.tag) {
          sameParentStructuralBlock = true;
        }
      }

      // boundaryAllowed must satisfy:
      // 1) No open move chain (Remove ... Insert of same tag)
      // 2) No pending Insert+Update init chains
      // 3) Avoid cutting tiny same-parent structural runs, but allow a soft
      // break once current group is big enough
      const bool boundaryAllowed = openMovedTags.empty() &&
          pendingInitTags.empty() &&
          (!sameParentStructuralBlock ||
           groupCost >= SOFT_SAMEPARENT_BREAK_COST);
      if (boundaryAllowed) {
        pushGroup(i + 1);
      }
    }
  }
  pushGroup(n);

  if (groups.size() <= 1) {
    out.enabled = false;
    out.batchA = nonCreate;
    return out;
  }

  int totalCost = 0;
  for (const auto& g : groups)
    totalCost += g.cost;

  if (totalCost < NONCREATE_SPLIT_MIN_COST) {
    out.enabled = false;
    out.batchA = nonCreate;
    return out;
  }

  const int target = totalCost / static_cast<int>(TWO_BATCHES);

  size_t bestGroupBoundaryIndex = 0;
  int bestDiff = std::numeric_limits<int>::max();

  int prefixCost = 0;
  for (size_t k = 1; k < groups.size(); ++k) {
    prefixCost += groups[k - 1].cost;
    const int diff = std::abs(prefixCost - target);
    if (diff < bestDiff) {
      bestDiff = diff;
      bestGroupBoundaryIndex = k;
    }
  }

  size_t splitIndex = groups[bestGroupBoundaryIndex - 1].end;
  if (splitIndex == 0 || splitIndex >= n) {
    if (n >= TWO_BATCHES) {
      splitIndex = n / TWO_BATCHES;
      if (splitIndex == 0) {
        splitIndex = 1;
      }
      if (splitIndex >= n) {
        splitIndex = n - 1;
      }
    } else {
      out.enabled = false;
      out.batchA = nonCreate;
      return out;
    }
  }

  out.enabled = true;
  out.batchA = facebook::react::ShadowViewMutationList(
      nonCreate.begin(), nonCreate.begin() + splitIndex);
  out.batchB = facebook::react::ShadowViewMutationList(
      nonCreate.begin() + splitIndex, nonCreate.end());
  return out;
}

} // namespace

namespace rnoh {

void SchedulerDelegate::schedulerDidFinishTransaction(
    MountingCoordinator::Shared mountingCoordinator) {
  facebook::react::SystraceSection s(
      "#RNOH::SchedulerDelegate::schedulerDidFinishTransaction");
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::
          FABRIC_FINISH_TRANSACTION_START);
  mountingCoordinator->getTelemetryController().pullTransaction(
      [](auto transaction, auto const& surfaceTelemetry) {},
      [this](auto transaction, auto const& surfaceTelemetry) {
        performOnMainThread(
            [transaction](MountingManager::Shared const& mountingManager) {
              mountingManager->doMount(transaction->getMutations());
            });
      },
      [this](auto transaction, auto const& surfaceTelemetry) {
        int taskId = random();
        std::string taskTrace =
            "#RNOH::TaskExecutor::runningTask t" + std::to_string(taskId);

        auto performOnMainThreadNextFrame =
            [this](std::function<void(MountingManager::Shared const&)> task) {
              NextFrameDispatcher::Get().post(
                  [this, t = std::move(task)]() mutable {
                    facebook::react::SystraceSection s(
                        "#RNOH::SchedulerDelegate::onNextFrame");
                    this->performOnMainThread(std::move(t));
                  });
            };

        if (IsParallelizationWorkable()) {
          facebook::react::SystraceSection s(
              "#RNOH::SchedulerDelegate::didMount size:",
              transaction->getMutations().size());
          auto job_partner =
              ffrt::job_partner<ScenarioID::MUTATION_PARALLELIZATION>::
                  get_main_partner(
                      ffrt::job_partner_attr()
                          .max_parallelism(MAX_THREAD_NUM_MUTATION_CREATE)
                          .qos(THREAD_PRIORITY_LEVEL_5));
          ShadowViewMutationList otherCreateMutationList;
          ShadowViewMutationList otherMutationList;

          for (uint64_t i = 0; i < transaction->getMutations().size(); ++i) {
            auto const& mutation = transaction->getMutations()[i];
            if (mutation.type == facebook::react::ShadowViewMutation::Create) {
              if (parallelComponentHandles.find(
                      mutation.newChildShadowView.componentHandle) !=
                      parallelComponentHandles.end() ||
                  ComponentNameManager::getInstance().hasComponentName(
                      mutation.newChildShadowView.componentName)) {
                job_partner->submit([this, &mutation] {
                  try {
                    this->m_mountingManager.lock()->handleMutation(mutation);
                  } catch (std::exception const& e) {
                    LOG(ERROR) << "Mutation " << mutation.type
                               << " failed: " << e.what();
                  }
                });
              } else {
                otherCreateMutationList.push_back(mutation);
              }
            } else {
              otherMutationList.push_back(mutation);
            }
          }
          job_partner->wait();

          performOnMainThread(
              [transaction, otherCreateMutationList, taskTrace, this](
                  MountingManager::Shared const& mountingManager) {
                facebook::react::SystraceSection s(
                    "#RNOH::SchedulerDelegate::otherCreate");
                mountingManager->didMount(otherCreateMutationList);
              });

          const auto surfaceId = transaction->getSurfaceId();
          const bool inLoadWindow = isInInitialLoadWindow(surfaceId);
          if (inLoadWindow) {
            auto allOther = std::move(otherMutationList);
            performOnMainThread(
                [transaction,
                 otherMutationList = std::move(allOther),
                 taskTrace,
                 this](MountingManager::Shared const& mountingManager) {
                  facebook::react::SystraceSection s(taskTrace.c_str());
                  mountingManager->didMount(otherMutationList);
                  mountingManager->finalizeMutationUpdates(
                      transaction->getMutations());
                });
            performOnMainThread(
                [this](MountingManager::Shared const& mountingManager) {
                  mountingManager->clearPreallocatedViews();
                });
            return;
          }

          auto split = splitNonCreateMutationsIntoTwoBatches(otherMutationList);
          if (split.enabled) {
            auto batchA = std::move(split.batchA);
            auto batchB = std::move(split.batchB);

            performOnMainThread(
                [batchA, this](MountingManager::Shared const& mountingManager) {
                  facebook::react::SystraceSection s(
                      "#RNOH::SchedulerDelegate::other-nonCreate-A size:",
                      batchA.size());
                  mountingManager->didMount(batchA);
                });
            performOnMainThreadNextFrame(
                [transaction, batchB, this](
                    MountingManager::Shared const& mountingManager) {
                  facebook::react::SystraceSection s(
                      "#RNOH::SchedulerDelegate::other-nonCreate-B size:",
                      batchB.size());
                  mountingManager->didMount(batchB);
                  mountingManager->finalizeMutationUpdates(
                      transaction->getMutations());
                  mountingManager->clearPreallocatedViews();
                });
          } else {
            performOnMainThread(
                [transaction, otherMutationList, taskTrace, this](
                    MountingManager::Shared const& mountingManager) {
                  facebook::react::SystraceSection s(
                      "#RNOH::SchedulerDelegate::other-and-finalize");
                  mountingManager->didMount(otherMutationList);
                  mountingManager->finalizeMutationUpdates(
                      transaction->getMutations());
                });
            performOnMainThread(
                [this](MountingManager::Shared const& mountingManager) {
                  facebook::react::SystraceSection s(
                      "#RNOH::SchedulerDelegate::clearPreallocatedViews");
                  mountingManager->clearPreallocatedViews();
                });
          }
          logTransactionTelemetryMarkers(*transaction);
          return;
        }

        auto mutationVecs = transaction->getMutations();
        facebook::react::ShadowViewMutationList mutationVec;
        facebook::react::ShadowViewMutationList otherMutation;
        // The reason for using 70 here is that the average time for each
        // mutation creation is about 60 microseconds,and the time for 70 is 4.2
        // milliseconds, which corresponds to the length of half a frame
        size_t fragmentSize = 70;
        std::vector<facebook::react::ShadowViewMutationList> destVec;
        auto it = mutationVecs.begin();
        while (it != mutationVecs.end()) {
          if (it->type == facebook::react::ShadowViewMutation::Create) {
            mutationVec.push_back(*it);
          } else {
            otherMutation.push_back(*it);
          }
          ++it;
        }
        size_t mutationSize = mutationVec.size();
        if (mutationSize > 0) {
          destVec.reserve((mutationSize + fragmentSize - 1) / fragmentSize);
          for (size_t i = 0; i < mutationSize; i += fragmentSize) {
            destVec.emplace_back(
                mutationVec.begin() + i,
                mutationVec.begin() + std::min(mutationSize, i + fragmentSize));
          }
          for (const auto& mutations : destVec) {
            performOnMainThread(
                [mutations,
                 taskTrace](MountingManager::Shared const& mountingManager) {
                  facebook::react::SystraceSection s(taskTrace.c_str());
                  mountingManager->didMount(mutations);
                });
          }
        }
        performOnMainThread(
            [otherMutation, mutationVecs, taskTrace, this](
                MountingManager::Shared const& mountingManager) {
              facebook::react::SystraceSection s(taskTrace.c_str());
              mountingManager->didMount(otherMutation);
              mountingManager->finalizeMutationUpdates(mutationVecs);
            });
        performOnMainThread(
            [otherMutation, mutationVecs, this](
                MountingManager::Shared const& mountingManager) {
              mountingManager->clearPreallocatedViews();
            });
        logTransactionTelemetryMarkers(*transaction);
        facebook::react::SystraceSection s(
            "#RNOH::TaskExecutor::runTask t", taskId);
      });
  if (auto mountingManager = m_mountingManager.lock()) {
    mountingManager->clearPreallocationRequestQueue();
  }
}

void SchedulerDelegate::logTransactionTelemetryMarkers(
    MountingTransaction const& transaction) {
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_FINISH_TRANSACTION_END);
  auto telemetry = transaction.getTelemetry();
  auto commitStartTime =
      telemetryTimePointToMilliseconds(telemetry.getCommitStartTime());
  auto commitEndTime =
      telemetryTimePointToMilliseconds(telemetry.getCommitEndTime());
  auto diffStartTime =
      telemetryTimePointToMilliseconds(telemetry.getDiffStartTime());
  auto diffEndTime =
      telemetryTimePointToMilliseconds(telemetry.getDiffEndTime());
  auto layoutStartTime =
      telemetryTimePointToMilliseconds(telemetry.getLayoutStartTime());
  auto layoutEndTime =
      telemetryTimePointToMilliseconds(telemetry.getLayoutEndTime());

  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_COMMIT_START,
      "",
      commitStartTime);
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_COMMIT_END,
      "",
      commitEndTime);
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_DIFF_START,
      "",
      diffStartTime);
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_DIFF_END,
      "",
      diffEndTime);
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_LAYOUT_START,
      "",
      layoutStartTime);
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::FABRIC_LAYOUT_END,
      "",
      layoutEndTime);
}

void SchedulerDelegate::schedulerDidRequestPreliminaryViewAllocation(
    SurfaceId /*surfaceId*/,
    const ShadowNode& shadowNode) {
  auto preallocationRequestQueue = m_weakPreallocationRequestQueue.lock();
  if (preallocationRequestQueue == nullptr) {
    return;
  }
  preallocationRequestQueue->push(
      PreallocationRequest{
          shadowNode.getTag(),
          shadowNode.getComponentHandle(),
          shadowNode.getComponentName(),
          shadowNode.getProps()});
}

void SchedulerDelegate::schedulerDidDispatchCommand(
    const ShadowView& shadowView,
    std::string const& commandName,
    folly::dynamic const& args) {
  performOnMainThread([shadowView, commandName, args](
                          MountingManager::Shared const& mountingManager) {
    mountingManager->dispatchCommand(shadowView, commandName, args);
  });
}

void SchedulerDelegate::schedulerDidSendAccessibilityEvent(
    const ShadowView& shadowView,
    std::string const& eventType) {
  performOnMainThread([shadowView, eventType](
                          MountingManager::Shared const& mountingManager) {
    mountingManager->schedulerDidSendAccessibilityEvent(shadowView, eventType);
  });
}

void SchedulerDelegate::schedulerDidSetIsJSResponder(
    ShadowView const& shadowView,
    bool isJSResponder,
    bool blockNativeResponder) {
  performOnMainThread([shadowView, isJSResponder, blockNativeResponder](
                          MountingManager::Shared const& mountingManager) {
    mountingManager->setIsJsResponder(
        shadowView, isJSResponder, blockNativeResponder);
  });
}

} // namespace rnoh
