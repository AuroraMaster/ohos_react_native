/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "SchedulerDelegate.h"
#include <react/renderer/debug/SystraceSection.h>
#include "ParallelComponent.h"
#include "RNOH/Performance/HarmonyReactMarker.h"
#include "ffrt/cpp/pattern/job_partner.h"
#include "RNOH/ApiVersionCheck.h"
#include "RNOH/FFRTConfig.h"

namespace rnoh {

void SchedulerDelegate::schedulerDidFinishTransaction(
    MountingCoordinator::Shared mountingCoordinator) {
  facebook::react::SystraceSection s(
      "#RNOH::SchedulerDelegate::schedulerDidFinishTransaction");
  HarmonyReactMarker::logMarker(HarmonyReactMarker::HarmonyReactMarkerId::
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
            if (IsAtLeastApi21()) {
                facebook::react::SystraceSection s("#RNOH::SchedulerDelegate::didCreateMount");
                auto job_partner = ffrt::job_partner<ScenarioID::MUTATION_PARALLELIZATION>::get_main_partner(
                    ffrt::job_partner_attr()
                        .max_parallelism(MAX_THREAD_NUM_MUTATION_CREATE)
                        .qos(THREAD_PRIORITY_LEVEL_5));
                ShadowViewMutationList otherMutationList;
                for (uint64_t i = 0; i < transaction->getMutations().size(); ++i) {
                    auto const &mutation = transaction->getMutations()[i];
                    if (mutation.type == facebook::react::ShadowViewMutation::Create &&
                        (parallelComponentHandles.find(mutation.newChildShadowView.componentHandle) !=
                             parallelComponentHandles.end() ||
                         ComponentNameManager::getInstance().hasComponentName(
                             mutation.newChildShadowView.componentName))) {
                        job_partner->submit([this, &mutation] {
                            try {
                                this->m_mountingManager.lock()->handleMutation(mutation);
                            } catch (std::exception const &e) {
                                LOG(ERROR) << "Mutation " << mutation.type << " failed: " << e.what();
                            }
                        });
                    } else {
                        otherMutationList.push_back(mutation);
                    }
                }
                job_partner->wait();
                performOnMainThread(
                    [transaction, otherMutationList, this](MountingManager::Shared const &mountingManager) {
                        mountingManager->didMount(otherMutationList);
                        mountingManager->finalizeMutationUpdates(transaction->getMutations());
                    });
                logTransactionTelemetryMarkers(*transaction);
                return;
            }
        auto mutationVecs = transaction->getMutations();
        facebook::react::ShadowViewMutationList mutationVec;
        facebook::react::ShadowViewMutationList otherMutation;
        // The reason for using 70 here is that the average time for each mutation 
        // creation is about 60 microseconds,and the time for 70 is 4.2 milliseconds, 
        // which corresponds to the length of half a frame
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
                        mutationVec.begin() +
                            std::min(mutationSize, i + fragmentSize));
            }
            for (const auto& mutations : destVec) {
                performOnMainThread(
                    [mutations, taskTrace](
                        MountingManager::Shared const &mountingManager) {
                    facebook::react::SystraceSection s(taskTrace.c_str());
                    mountingManager->didMount(mutations);
                    mountingManager->clearPreallocatedViews(mutations);
                });
            }
        }
        performOnMainThread(
            [otherMutation, mutationVecs, taskTrace, this](
                MountingManager::Shared const &mountingManager) {
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