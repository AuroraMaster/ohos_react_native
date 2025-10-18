/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include <cxxreact/Instance.h>
#include <cxxreact/ModuleRegistry.h>
#include <cxxreact/NativeModule.h>
#include <folly/dynamic.h>
#include <react/renderer/animations/LayoutAnimationDriver.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <react/renderer/scheduler/Scheduler.h>
#include <react/renderer/uimanager/LayoutAnimationStatusDelegate.h>

#include "RNOH/ArkTSChannel.h"
#include "RNOH/ArkTSMessageHandler.h"
#include "RNOH/EventDispatcher.h"
#include "RNOH/EventEmitRequestHandler.h"
#include "RNOH/GlobalJSIBinder.h"
#include "RNOH/MessageQueueThread.h"
#include "RNOH/MountingManager.h"
#include "RNOH/ShadowViewRegistry.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "RNOH/TurboModuleFactory.h"
#include "RNOH/TurboModuleProvider.h"
#include "RNOH/UITicker.h"

namespace rnoh {

class RNInstanceArkTS : public RNInstanceInternal,
                        public facebook::react::LayoutAnimationStatusDelegate {
 public:
  RNInstanceArkTS(
      int id,
      std::shared_ptr<facebook::react::ContextContainer> contextContainer,
      TurboModuleFactory&& turboModuleFactory,
      std::shared_ptr<TaskExecutor> taskExecutor,
      std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry>
          componentDescriptorProviderRegistry,
      MutationsToNapiConverter::Shared mutationsToNapiConverter,
      EventEmitRequestHandlers eventEmitRequestHandlers,
      GlobalJSIBinders globalJSIBinders,
      UITicker::Shared uiTicker,
      ShadowViewRegistry::Shared shadowViewRegistry,
      ArkTSChannel::Shared arkTSChannel,
      MountingManager::Shared mountingManager,
      std::vector<ArkTSMessageHandler::Shared> arkTSMessageHandlers,
      bool shouldEnableDebugger,
      bool shouldEnableBackgroundExecutor)
      : RNInstanceInternal(
            id,
            contextContainer,
            std::move(taskExecutor),
            nullptr,
            nullptr),
        instance(std::make_shared<facebook::react::Instance>()),
        scheduler(nullptr),
        m_shadowViewRegistry(shadowViewRegistry),
        m_turboModuleFactory(std::move(turboModuleFactory)),
        m_componentDescriptorProviderRegistry(
            componentDescriptorProviderRegistry),
        m_mutationsToNapiConverter(mutationsToNapiConverter),
        m_eventEmitRequestHandlers(eventEmitRequestHandlers),
        m_globalJSIBinders(globalJSIBinders),
        m_uiTicker(uiTicker),
        m_mountingManager(std::move(mountingManager)),
        m_arkTSMessageHandlers(std::move(arkTSMessageHandlers)),
        m_shouldEnableDebugger(shouldEnableDebugger),
        m_arkTSChannel(std::move(arkTSChannel)),
        m_shouldEnableBackgroundExecutor(shouldEnableBackgroundExecutor) {
  }

  ~RNInstanceArkTS() {
    if (this->unsubscribeUITickListener != nullptr) {
      unsubscribeUITickListener();
    }
    // synchronization primitives used to ensure all tasks currently in
    // queue run before this destructor returns. This ensures the tasks
    // scheduled by this object are not running after the object is
    // destroyed.
    std::mutex surfacesUnregisteredMutex;
    std::condition_variable cv;
    std::unique_lock lock(surfacesUnregisteredMutex);
    taskExecutor->runTask(
        TaskThread::JS, [this, &cv, &surfacesUnregisteredMutex] {
          std::unique_lock lock(surfacesUnregisteredMutex);
          for (auto& [_tag, surfaceHandler] : surfaceHandlers) {
            if (surfaceHandler->getStatus() ==
                facebook::react::SurfaceHandler::Status::Running) {
              surfaceHandler->stop();
            }
            scheduler->unregisterSurface(*surfaceHandler);
          }
          cv.notify_one();
        });
    // block until the task has finished running,
    // to ensure that the instance is not destroyed before all surface are
    // unregistered
    cv.wait(lock);
  }

  TaskExecutor::Shared getTaskExecutor() override;

  void start() override;
  void createSurface(
      facebook::react::Tag surfaceId,
      std::string const& moduleName) override;
  void updateSurfaceConstraints(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      bool isRTL) override;
  facebook::react::Size measureSurface(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      bool isRTL) override;
  void startSurface(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      bool isRTL,
      folly::dynamic&& initialProps) override;
  void setSurfaceProps(facebook::react::Tag surfaceId, folly::dynamic&& props)
      override;
  void stopSurface(facebook::react::Tag surfaceId, std::function<void()> onStop)
      override;
  void destroySurface(facebook::react::Tag surfaceId) override;
  void setSurfaceDisplayMode(
      facebook::react::Tag surfaceId,
      facebook::react::DisplayMode displayMode) override;
  void callJSFunction(
      std::string&& module,
      std::string&& method,
      folly::dynamic&& params) override;
  void emitComponentEvent(
      napi_env env,
      facebook::react::Tag tag,
      std::string eventName,
      napi_value payload) override;
  void onMemoryLevel(size_t memoryLevel) override;
  void updateState(
      napi_env env,
      std::string const& componentName,
      facebook::react::Tag tag,
      napi_value newState) override;
  void handleArkTSMessage(
      const std::string& name,
      folly::dynamic const& payload) override;
  void synchronouslyUpdateViewOnUIThread(
      facebook::react::Tag tag,
      folly::dynamic props) override;
  facebook::react::ContextContainer const& getContextContainer() const override;
  TurboModule::Shared getTurboModule(const std::string& name) override;
  void postMessageToArkTS(
      const std::string& name,
      folly::dynamic const& payload) override;
  NativeResourceManager const* getNativeResourceManager() const override;
  std::string getCacheDir() override {return {};}
  void registerFont(
      std::string const& fontFamily,
      std::string const& fontFilePath) override;
  void setJavaScriptExecutorFactory(
      std::shared_ptr<facebook::react::JSExecutorFactory> jsExecutorFactory) override;

 protected:
  std::map<
      facebook::react::Tag,
      std::shared_ptr<facebook::react::SurfaceHandler>>
      surfaceHandlers;
  std::shared_ptr<facebook::react::Scheduler> scheduler;
    MountingManager::Shared m_mountingManager;
  std::shared_ptr<facebook::react::Instance> instance;
  TurboModuleProvider::Shared m_turboModuleProvider;
  std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry>
      m_componentDescriptorProviderRegistry;
  ShadowViewRegistry::Shared m_shadowViewRegistry;
  TurboModuleFactory m_turboModuleFactory;
  std::shared_ptr<EventDispatcher> m_eventDispatcher;
  MutationsToNapiConverter::Shared m_mutationsToNapiConverter;
  EventEmitRequestHandlers m_eventEmitRequestHandlers;
  GlobalJSIBinders m_globalJSIBinders;
  std::shared_ptr<facebook::react::LayoutAnimationDriver> m_animationDriver;
  UITicker::Shared m_uiTicker;
  std::mutex m_unsubscribeUITickListenerMtx;
  std::function<void()> unsubscribeUITickListener = nullptr;
  std::shared_ptr<MessageQueueThread> m_jsQueue;
  bool m_shouldEnableDebugger;
  bool m_shouldEnableBackgroundExecutor;
  std::vector<ArkTSMessageHandler::Shared> m_arkTSMessageHandlers;
  ArkTSChannel::Shared m_arkTSChannel;
  std::string m_bundlePath;
  std::shared_ptr<facebook::react::JSExecutorFactory> m_jsExecutorFactory = nullptr;

  void initialize();
  void initializeScheduler(
      std::shared_ptr<TurboModuleProvider> turboModuleProvider);
  std::shared_ptr<TurboModuleProvider> createTurboModuleProvider();
  void onUITick(UITicker::Timestamp recentVSyncTimestamp);

  void onAnimationStarted() override; // react::LayoutAnimationStatusDelegate
  void onAllAnimationsComplete()
      override; // react::LayoutAnimationStatusDelegate
};

} // namespace rnoh
