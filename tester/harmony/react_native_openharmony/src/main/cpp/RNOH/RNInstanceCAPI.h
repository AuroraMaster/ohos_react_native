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
#include <rawfile/raw_file_manager.h>
#include <react/renderer/animations/LayoutAnimationDriver.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <react/renderer/scheduler/Scheduler.h>
#include <react/renderer/uimanager/LayoutAnimationStatusDelegate.h>

#include "ArkTSMessageHub.h"
#include "RNOH/ArkTSChannel.h"
#include "RNOH/EventDispatcher.h"
#include "RNOH/EventEmitRequestHandler.h"
#include "RNOH/GlobalJSIBinder.h"
#include "RNOH/MessageQueueThread.h"
#include "RNOH/MountingManager.h"
#include "RNOH/RNInstance.h"
#include "RNOH/ShadowViewRegistry.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "RNOH/TurboModuleFactory.h"
#include "RNOH/TurboModuleProvider.h"
#include "RNOH/UITicker.h"

#include "RNOH/ArkTSMessageHandler.h"
#include "RNOH/ComponentInstanceFactory.h"
#include "RNOH/ComponentInstancePreallocationRequestQueue.h"
#include "RNOH/ComponentInstanceRegistry.h"
#include "RNOH/arkui/ArkUISurface.h"
#include "RNOH/arkui/NodeContentHandle.h"

namespace rnoh {
using MutationsListener = std::function<void(
    MutationsToNapiConverter const&,
    facebook::react::ShadowViewMutationList const& mutations)>;

class RNInstanceCAPI : public RNInstanceInternal,
                       public facebook::react::LayoutAnimationStatusDelegate {
 public:
  RNInstanceCAPI(
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
      MountingManager::Shared mountingManager,
      std::vector<ArkTSMessageHandler::Shared> arkTSMessageHandlers,
      ArkTSChannel::Shared arkTSChannel,
      ArkTSMessageHub::Shared arkTSMessageHub,
      ComponentInstanceRegistry::Shared componentInstanceRegistry,
      ComponentInstanceFactory::Shared componentInstanceFactory,
      ComponentInstancePreallocationRequestQueue::Shared
          componentInstancePreallocationRequestQueue,
      SharedNativeResourceManager nativeResourceManager,
      bool shouldEnableDebugger,
      bool shouldEnableBackgroundExecutor,
      std::string hspModuleName,
      std::string cacheDir)
      : RNInstanceInternal(
            id,
            contextContainer,
            std::move(taskExecutor),
            std::move(nativeResourceManager),
            hspModuleName),
        m_shadowViewRegistry(shadowViewRegistry),
        m_turboModuleFactory(std::move(turboModuleFactory)),
        m_componentDescriptorProviderRegistry(
            componentDescriptorProviderRegistry),
        m_mutationsToNapiConverter(mutationsToNapiConverter),
        m_eventEmitRequestHandlers(eventEmitRequestHandlers),
        m_globalJSIBinders(globalJSIBinders),
        m_uiTicker(uiTicker),
        m_mountingManager(std::move(mountingManager)),
        m_shouldEnableDebugger(shouldEnableDebugger),
        m_shouldEnableBackgroundExecutor(shouldEnableBackgroundExecutor),
        m_arkTSMessageHub(std::move(arkTSMessageHub)),
        m_componentInstanceRegistry(componentInstanceRegistry),
        m_componentInstanceFactory(componentInstanceFactory),
        m_arkTSChannel(std::move(arkTSChannel)),
        m_arkTSMessageHandlers(std::move(arkTSMessageHandlers)),
        m_componentInstancePreallocationRequestQueue(
            std::move(componentInstancePreallocationRequestQueue)),
        m_cacheDir(cacheDir) {}

  ~RNInstanceCAPI() noexcept override;

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
      bool isRTL);

  void updateSurfaceConstraints(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      float fontSizeMultiplier,
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
      bool isRTL);

  facebook::react::Size measureSurface(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      float fontSizeMultiplier,
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
      folly::dynamic&& initialProps);

  void startSurface(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      float fontSizeMultiplier,
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

  void synchronouslyUpdateViewOnUIThread(
      facebook::react::Tag tag,
      folly::dynamic props) override;

  facebook::react::ContextContainer const& getContextContainer() const override;

  void attachRootView(
      NodeContentHandle nodeContentHandle,
      facebook::react::Tag surfaceId);

  void detachRootView(facebook::react::Tag surfaceId);

  TurboModule::Shared getTurboModule(const std::string& name) override;

  ComponentInstance::Shared findComponentInstanceByTag(
      facebook::react::Tag tag);

  std::optional<facebook::react::Tag> findComponentInstanceTagById(
      const std::string& id);

  std::optional<std::string> getNativeNodeIdByTag(
      facebook::react::Tag tag) const;

  void handleArkTSMessage(
      const std::string& name,
      folly::dynamic const& payload) override;

  void postMessageToArkTS(
      const std::string& name,
      folly::dynamic const& payload) override;
  void addArkTSMessageHandler(ArkTSMessageHandler::Shared handler);
  void removeArkTSMessageHandler(ArkTSMessageHandler::Shared handler);
  int getId() override {
    return m_id;
  }
  std::string getCacheDir() override {return m_cacheDir;}
  std::optional<Surface::Weak> getSurfaceByRootTag(
      facebook::react::Tag rootTag) override;
  void registerFont(
      std::string const& fontFamily,
      std::string const& fontFilePath) override;
  void setJavaScriptExecutorFactory(
      std::shared_ptr<facebook::react::JSExecutorFactory> jsExecutorFactory)
      override;
  void setArkUINodeContext(const ArkUINode::Context::Shared& arkUINodeContext) {
    if (m_componentInstanceFactory) {
      m_componentInstanceFactory->setArkUINodeContext(arkUINodeContext);
    } else {
      DLOG(ERROR)
          << "RNInstanceCAPI::setArkUINodeContext: m_componentInstanceFactory is null";
    }
  }
 protected:
  std::map<
      facebook::react::Tag,
      std::shared_ptr<facebook::react::SurfaceHandler>>
      surfaceHandlers;
  std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry>
      m_componentDescriptorProviderRegistry;
  ShadowViewRegistry::Shared m_shadowViewRegistry;
  TurboModuleFactory m_turboModuleFactory;
  TurboModuleProvider::Shared m_turboModuleProvider;
  std::shared_ptr<EventDispatcher> m_eventDispatcher;
  MutationsToNapiConverter::Shared m_mutationsToNapiConverter;
  EventEmitRequestHandlers m_eventEmitRequestHandlers;
  GlobalJSIBinders m_globalJSIBinders;
  std::shared_ptr<facebook::react::LayoutAnimationDriver> m_animationDriver;
  UITicker::Shared m_uiTicker;
  std::mutex m_unsubscribeUITickListenerMtx;
  std::function<void()> unsubscribeUITickListener = nullptr;
  ComponentInstancePreallocationRequestQueue::Shared
      m_componentInstancePreallocationRequestQueue;
  std::shared_ptr<MessageQueueThread> m_jsQueue;
  bool m_shouldEnableDebugger;
  bool m_shouldEnableBackgroundExecutor;
  std::unordered_map<facebook::react::SurfaceId, ArkUISurface::Shared>
      m_surfaceById;
  ComponentInstanceRegistry::Shared m_componentInstanceRegistry;
  ComponentInstanceFactory::Shared m_componentInstanceFactory;
  MountingManager::Shared m_mountingManager;
  float m_densityDpi;
  std::mutex m_arkTSMessageHandlersMtx;
  std::vector<ArkTSMessageHandler::Shared> m_arkTSMessageHandlers;
  ArkTSChannel::Shared m_arkTSChannel;
  ArkTSMessageHub::Shared m_arkTSMessageHub;
  std::shared_ptr<facebook::react::JSExecutorFactory> m_jsExecutorFactory =
      nullptr;
  std::string m_cacheDir;

  void initialize();
  void initializeScheduler(
      std::shared_ptr<TurboModuleProvider> turboModuleProvider);
  std::shared_ptr<TurboModuleProvider> createTurboModuleProvider();
  void onUITick(UITicker::Timestamp recentVSyncTimestamp);
  void onAnimationStarted() override; // react::LayoutAnimationStatusDelegate
  void onAllAnimationsComplete()
      override; // react::LayoutAnimationStatusDelegate
  void onConfigurationChange(folly::dynamic const& payload);
};

} // namespace rnoh
