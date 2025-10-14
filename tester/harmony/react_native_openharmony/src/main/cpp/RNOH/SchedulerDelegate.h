/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/scheduler/SchedulerDelegate.h>
#include <react/utils/Telemetry.h>
#include "RNOH/ComponentInstancePreallocationRequestQueue.h"
#include "RNOH/MountingManager.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"

namespace rnoh {

/**
 * @ThreadSafe
 *
 * Implementation of the react::SchedulerDelegate interface.
 * Schedules operations on the main thread
 * and delegates them to the MountingManager.
 */
class SchedulerDelegate final : public facebook::react::SchedulerDelegate {
  using MountingCoordinator = facebook::react::MountingCoordinator;
  using ShadowNode = facebook::react::ShadowNode;
  using ShadowView = facebook::react::ShadowView;
  using SurfaceId = facebook::react::SurfaceId;
  using PreallocationRequest = ComponentInstancePreallocationRequestQueue::Request;

 public:
  SchedulerDelegate(
      MountingManager::Shared mountingManager,
      TaskExecutor::Shared taskExecutor,
      ComponentInstancePreallocationRequestQueue::Weak
          weakPreallocationRequestQueue)
      : m_mountingManager(mountingManager), m_taskExecutor(taskExecutor),
        m_weakPreallocationRequestQueue(
            std::move(weakPreallocationRequestQueue)){};
    
  void schedulerDidRequestPreliminaryViewAllocation(
      SurfaceId /*surfaceId*/,
      const ShadowNode& shadowView) override;

  void schedulerDidFinishTransaction(
      MountingCoordinator::Shared mountingCoordinator) override;
    

  void schedulerDidDispatchCommand(
      const ShadowView& shadowView,
      std::string const& commandName,
      folly::dynamic const& args) override;

  void schedulerDidSendAccessibilityEvent(
      const ShadowView& shadowView,
      std::string const& eventType) override;

  void schedulerDidSetIsJSResponder(
      ShadowView const& shadowView,
      bool isJSResponder,
      bool blockNativeResponder) override;

 private:
  template <typename Operation>
  void performOnMainThread(Operation operation) {
    m_taskExecutor->runTask(
        TaskThread::MAIN,
        [weakMountingManager = m_mountingManager,
         operation = std::move(operation)] {
          if (auto mountingManager = weakMountingManager.lock()) {
            operation(mountingManager);
          }
        });
  }
  static void logTransactionTelemetryMarkers(
      facebook::react::MountingTransaction const& transaction);

  MountingManager::Weak m_mountingManager;
  TaskExecutor::Shared m_taskExecutor;
  ComponentInstancePreallocationRequestQueue::Weak
      m_weakPreallocationRequestQueue;
};

}; // namespace rnoh