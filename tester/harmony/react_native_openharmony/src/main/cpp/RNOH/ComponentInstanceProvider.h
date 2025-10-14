/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "ComponentInstanceFactory.h"
#include "ComponentInstancePreallocationRequestQueue.h"
#include "ComponentInstanceRegistry.h"
#include "UITicker.h"

namespace rnoh {
/**
 * @internal
 * @thread: MAIN
 */
class ComponentInstanceProvider
    : public std::enable_shared_from_this<ComponentInstanceProvider>,
      public ComponentInstancePreallocationRequestQueue::Delegate {
  ComponentInstanceFactory::Shared m_componentInstanceFactory;
  std::unordered_map<facebook::react::Tag, ComponentInstance::Shared>
      m_preallocatedComponentInstanceByTag;
  std::mutex m_unsubscribeUITickerListenerMtx;
  std::function<void()> m_unsubscribeUITickerListener = nullptr;
  ComponentInstancePreallocationRequestQueue::Shared
      m_preallocationRequestQueue;
  ComponentInstanceRegistry::Shared m_componentInstanceRegistry;
  ThreadGuard m_threadGuard;
  UITicker::Shared m_uiTicker;
  TaskExecutor::Weak m_weakTaskExecutor;

 public:
  using Shared = std::shared_ptr<ComponentInstanceProvider>;
  using PreallocationRequest = ComponentInstancePreallocationRequestQueue::Request;

  ComponentInstanceProvider(
      ComponentInstancePreallocationRequestQueue::Shared
          preallocationRequestQueue,
      ComponentInstanceFactory::Shared componentInstanceFactory,
      ComponentInstanceRegistry::Shared componentInstanceRegistry,
      UITicker::Shared uiTicker,
      TaskExecutor::Weak weakTaskExecutor);

  ~ComponentInstanceProvider();

  void initialize();

  ComponentInstance::Shared getComponentInstance(
      facebook::react::Tag tag,
      facebook::react::ComponentHandle componentHandle,
      std::string componentName);

  ComponentInstance::Shared createArkTSComponent(
      facebook::react::Tag tag,
      facebook::react::ComponentHandle componentHandle,
      std::string componentName);

  void clearPreallocationRequestQueue();

  void clearPreallocatedViews();

 private:
  /**
   * @thread: JS
   */
  void onPushPreallocationRequest() override;

  void onUITick(UITicker::Timestamp recentVSyncTimestamp);

  void processPreallocationRequest(
      PreallocationRequest const& shadowView);

  bool shouldPausePreallocationToAvoidBlockingMainThread(
      UITicker::Timestamp recentVSyncTimestamp);
};
} // namespace rnoh
