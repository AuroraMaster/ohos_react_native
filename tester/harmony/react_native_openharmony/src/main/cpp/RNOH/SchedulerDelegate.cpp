/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "SchedulerDelegate.h"
#include <react/renderer/debug/SystraceSection.h>
#include "RNOH/Performance/HarmonyReactMarker.h"


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
        auto mutationVecs = transaction->getMutations();
        facebook::react::ShadowViewMutationList mutationVec;
        facebook::react::ShadowViewMutationList otherMutation;
        // The reason for using 70 here is that the average time for each mutation 
        // creation is about 60 microseconds,and the time for 70 is 4.2 milliseconds, 
        // which corresponds to the length of half a frame
        size_t fragmentSize = 70;
        std::vector<facebook::react::ShadowViewMutationList> destVec;
        auto it = mutationVecs.begin();
        while (it != mutationVecs.end() ) {
            if( it->type == facebook::react::ShadowViewMutation::Create ){
                    mutationVec.push_back(*it);
               }else {
                    otherMutation.push_back(*it);
               }
            ++it;
        }
        size_t mutationSize = mutationVec.size();
        if( mutationSize > 0 ){
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
  facebook::react::SystraceSection s(
      "#RNOH::SchedulerDelegate::schedulerDidRequestPreliminaryViewAllocation");
  auto preallocationRequestQueue = m_weakPreallocationRequestQueue.lock();
  if (preallocationRequestQueue == nullptr) {
    return;
  }
  preallocationRequestQueue->push(
      PreallocationRequest{
          shadowNode.getTag(),
          shadowNode.getComponentHandle(),
          shadowNode.getComponentName()});
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