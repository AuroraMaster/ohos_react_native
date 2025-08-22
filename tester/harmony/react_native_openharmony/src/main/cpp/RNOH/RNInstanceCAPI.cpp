/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNInstanceCAPI.h"
#include <cxxreact/JSBundleType.h>
#include <jsireact/JSIExecutor.h>
#include <react/renderer/animations/LayoutAnimationDriver.h>
#include <react/renderer/componentregistry/ComponentDescriptorProvider.h>
#include <react/renderer/componentregistry/ComponentDescriptorRegistry.h>
#include <react/renderer/debug/SystraceSection.h>
#include <react/renderer/scheduler/Scheduler.h>
#include "ArkTSBridge.h"
#include "NativeLogger.h"
#include "RNInstanceArkTS.h"
#include "RNOH/Assert.h"
#include "RNOH/EventBeat.h"
#include "RNOH/MessageQueueThread.h"
#include "RNOH/MountingManagerCAPI.h"
#include "RNOH/Performance/HarmonyReactMarker.h"
#include "RNOH/Performance/NativeTracing.h"
#include "RNOH/RNInstance.h"
#include "RNOH/SchedulerDelegate.h"
#include "RNOH/ShadowViewRegistry.h"
#include "RNOH/TextMeasurer.h"
#include "RNOH/TurboModuleFactory.h"
#include "RNOH/TurboModuleProvider.h"
#include "RNOHCorePackage/TurboModules/DeviceInfoTurboModule.h"
#include "TaskExecutor/TaskExecutor.h"

using namespace facebook;
namespace rnoh {

RNInstance::~RNInstance() {};

rnoh::RNInstanceCAPI::~RNInstanceCAPI() {
  DLOG(INFO) << "~RNInstanceCAPI::start";
  if (unsubscribeUITickListener != nullptr) {
    unsubscribeUITickListener();
  }
  // clear non-thread-safe objects on the main thread
  // by moving them into a task
  taskExecutor->runSyncTask(
      TaskThread::MAIN,
      [mountingManager = std::move(m_mountingManager),
       componentInstanceRegistry = std::move(m_componentInstanceRegistry),
       componentInstanceFactory = std::move(m_componentInstanceFactory),
       // NOTE: `ArkUISurface` is not copyable, but `std::function` is, so
       // we need to move the map into a shared_ptr first in order to capture it
       surfaces = std::make_shared<decltype(m_surfaceById)>(
           std::move(m_surfaceById))] {});
  DLOG(INFO) << "~RNInstanceCAPI::stop";
}

TaskExecutor::Shared RNInstanceCAPI::getTaskExecutor() {
  return taskExecutor;
};

void RNInstanceCAPI::start() {
  DLOG(INFO) << "RNInstanceCAPI::start";
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::CREATE_REACT_CONTEXT_START);
  this->initialize();
  m_turboModuleProvider = this->createTurboModuleProvider();
  this->initializeScheduler(m_turboModuleProvider);
  this->instance->getRuntimeExecutor()(
      [binders = this->m_globalJSIBinders,
       turboModuleProvider =
           m_turboModuleProvider](facebook::jsi::Runtime& rt) {
        for (auto& binder : binders) {
          binder->createBindings(rt, turboModuleProvider);
        }
      });
  auto textMeasurer =
      m_contextContainer->at<std::shared_ptr<rnoh::TextMeasurer>>(
          "textLayoutManagerDelegate");
  RNOH_ASSERT(textMeasurer != nullptr);
  auto displayMetrics =
      ArkTSBridge::getInstance()->getDisplayMetrics().windowPhysicalPixels;

  float fontScale = displayMetrics.fontScale;
  float scale = displayMetrics.scale;

  m_densityDpi = displayMetrics.densityDpi;

  auto halfLeading =
      ArkTSBridge::getInstance()->getMetadata("half_leading") == "true";
  textMeasurer->setTextMeasureParams(
      fontScale, scale, m_densityDpi, halfLeading);
}

void RNInstanceCAPI::setJavaScriptExecutorFactory(
    std::shared_ptr<facebook::react::JSExecutorFactory> jsExecutorFactory) {
  DLOG(INFO) << "RNInstanceCAPI::setJavaScriptExecutorFactory";
  m_jsExecutorFactory = jsExecutorFactory;
}

void RNInstanceCAPI::initialize() {
  DLOG(INFO) << "RNInstanceCAPI::initialize";
  // create a new event dispatcher every time RN is initialized
  m_eventDispatcher = std::make_shared<EventDispatcher>();
  std::vector<std::unique_ptr<react::NativeModule>> modules;
  auto instanceCallback = std::make_unique<react::InstanceCallback>();
  m_jsQueue = std::make_shared<MessageQueueThread>(this->taskExecutor);
  auto moduleRegistry =
      std::make_shared<react::ModuleRegistry>(std::move(modules));
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::REACT_BRIDGE_LOADING_START);
  this->instance->initializeBridge(
      std::move(instanceCallback),
      std::move(m_jsExecutorFactory),
      m_jsQueue,
      std::move(moduleRegistry));
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::REACT_BRIDGE_LOADING_END);
}

void RNInstanceCAPI::initializeScheduler(
    std::shared_ptr<TurboModuleProvider> turboModuleProvider) {
  DLOG(INFO) << "RNInstanceCAPI::initializeScheduler";
  auto reactConfig = std::make_shared<react::EmptyReactNativeConfig>();
  m_contextContainer->insert("ReactNativeConfig", std::move(reactConfig));

  react::EventBeat::Factory eventBeatFactory =
      [taskExecutor = std::weak_ptr(taskExecutor),
       runtimeExecutor = this->instance->getRuntimeExecutor()](auto ownerBox) {
        return std::make_unique<EventBeat>(
            taskExecutor, runtimeExecutor, ownerBox);
      };

  react::ComponentRegistryFactory componentRegistryFactory =
      [registry = m_componentDescriptorProviderRegistry](
          auto eventDispatcher, auto contextContainer) {
        return registry->createComponentDescriptorRegistry(
            {eventDispatcher, contextContainer});
      };

  react::SchedulerToolbox schedulerToolbox{
      .contextContainer = m_contextContainer,
      .componentRegistryFactory = componentRegistryFactory,
      .runtimeExecutor = this->instance->getRuntimeExecutor(),
      .asynchronousEventBeatFactory = eventBeatFactory,
      .synchronousEventBeatFactory = eventBeatFactory,
  };

  if (m_shouldEnableBackgroundExecutor) {
    schedulerToolbox.backgroundExecutor =
        [executor = this->taskExecutor](std::function<void()>&& callback) {
          if (executor->isOnTaskThread(TaskThread::MAIN)) {
            callback();
            return;
          }

          executor->runTask(TaskThread::BACKGROUND, std::move(callback));
        };
  }

  m_animationDriver = std::make_shared<react::LayoutAnimationDriver>(
      this->instance->getRuntimeExecutor(), m_contextContainer, this);
  m_schedulerDelegate = std::make_unique<rnoh::SchedulerDelegate>(
      m_mountingManager,
      this->taskExecutor,
      m_componentInstancePreallocationRequestQueue,
      shared_from_this());
  this->scheduler = std::make_shared<react::Scheduler>(
      schedulerToolbox, m_animationDriver.get(), m_schedulerDelegate.get());
  turboModuleProvider->setScheduler(this->scheduler);
  DLOG(INFO) << "RNInstanceCAPI::initializeScheduler::end";
}

std::shared_ptr<TurboModuleProvider>
RNInstanceCAPI::createTurboModuleProvider() {
  DLOG(INFO) << "RNInstanceCAPI::createTurboModuleProvider";
  auto turboModuleProvider = std::make_shared<TurboModuleProvider>(
      this->instance->getJSCallInvoker(),
      std::move(m_turboModuleFactory),
      m_eventDispatcher,
      std::move(m_jsQueue),
      m_weakSelf);
  turboModuleProvider->installJSBindings(this->instance->getRuntimeExecutor());
  return turboModuleProvider;
}

std::optional<Surface::Weak> RNInstanceCAPI::getSurfaceByRootTag(
    facebook::react::Tag rootTag) {
  auto it = m_surfaceById.find(rootTag);
  if (it == m_surfaceById.end()) {
    return std::nullopt;
  }
  return it->second;
};

void rnoh::RNInstanceCAPI::emitComponentEvent(
    napi_env env,
    react::Tag tag,
    std::string eventName,
    napi_value payload) {
  DLOG(INFO) << "RNInstanceCAPI::emitComponentEvent";
  EventEmitRequestHandler::Context ctx{
      .env = env,
      .tag = tag,
      .eventName = std::move(eventName),
      .payload = payload,
      .shadowViewRegistry = this->m_shadowViewRegistry,
  };

  if (m_eventDispatcher != nullptr) {
    m_eventDispatcher->sendEvent(ctx);
  }

  for (auto& eventEmitRequestHandler : m_eventEmitRequestHandlers) {
    eventEmitRequestHandler->handleEvent(ctx);
  }
}

void rnoh::RNInstanceCAPI::onMemoryLevel(size_t memoryLevel) {
  DLOG(INFO) << "RNInstanceCAPI::onMemoryLevel";
  // Android memory levels are 5, 10, 15, while Ark's are 0, 1, 2
  static const int memoryLevels[] = {5, 10, 15};
  if (this->instance) {
    this->instance->handleMemoryPressure(memoryLevels[memoryLevel]);
  }
}

void rnoh::RNInstanceCAPI::updateState(
    napi_env env,
    std::string const& componentName,
    facebook::react::Tag tag,
    napi_value newState) {
  DLOG(INFO) << "RNInstanceCAPI::updateState";
  if (auto state =
          m_shadowViewRegistry->getFabricState<facebook::react::State>(tag)) {
    m_mutationsToNapiConverter->updateState(
        env, componentName, state, newState);
  }
}

void rnoh::RNInstanceCAPI::synchronouslyUpdateViewOnUIThread(
    facebook::react::Tag tag,
    folly::dynamic props) {
  DLOG(INFO) << "RNInstanceCAPI::synchronouslyUpdateViewOnUIThread";

  RNOH_ASSERT(taskExecutor->getCurrentTaskThread() == TaskThread::MAIN);

  auto componentInstance = m_componentInstanceRegistry->findByTag(tag);
  if (componentInstance == nullptr) {
    LOG(ERROR)
        << "RNInstanceCAPI::synchronouslyUpdateViewOnUIThread: could not find componentInstance for tag: "
        << tag;
    return;
  }

  auto componentHandle = componentInstance->getComponentHandle();
  auto componentDescriptor =
      scheduler->findComponentDescriptorByHandle_DO_NOT_USE_THIS_IS_BROKEN(
          componentHandle);
  if (componentDescriptor == nullptr) {
    LOG(ERROR)
        << "RNInstanceCAPI::synchronouslyUpdateViewOnUIThread: could not find componentDescriptor for tag: "
        << tag;
    return;
  }

  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::
          FABRIC_UPDATE_UI_MAIN_THREAD_START,
      tag);
  m_mountingManager->updateView(tag, std::move(props), *componentDescriptor);
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::
          FABRIC_UPDATE_UI_MAIN_THREAD_END,
      tag);
}

facebook::react::ContextContainer const&
rnoh::RNInstanceCAPI::getContextContainer() const {
  DLOG(INFO) << "RNInstanceCAPI::getContextContainer";
  return *m_contextContainer;
}

void RNInstanceCAPI::callJSFunction(
    std::string&& module,
    std::string&& method,
    folly::dynamic&& params) {
  this->taskExecutor->runTask(
      TaskThread::JS,
      [weakInstance = std::weak_ptr(this->instance),
       module = std::move(module),
       method = std::move(method),
       params = std::move(params)]() mutable {
        if (auto instance = weakInstance.lock()) {
          instance->callJSFunction(
              std::move(module), std::move(method), std::move(params));
        }
      });
}

void RNInstanceCAPI::onAnimationStarted() {
  facebook::react::SystraceSection s("RNInstanceCAPI::onAnimationStarted");
  if (this->unsubscribeUITickListener != nullptr) {
    return;
  }
  this->unsubscribeUITickListener =
      m_uiTicker->subscribe([this](auto recentVSyncTimestamp) {
        this->taskExecutor->runTask(
            TaskThread::MAIN,
            [weakSelf = weak_from_this(), recentVSyncTimestamp]() {
              if (auto instance = std::dynamic_pointer_cast<RNInstanceCAPI>(
                      weakSelf.lock())) {
                instance->onUITick(recentVSyncTimestamp);
              }
            });
      });
}

void RNInstanceCAPI::onAllAnimationsComplete() {
  facebook::react::SystraceSection s(
      "#RNOH::RNInstanceCAPI::onAllAnimationsComplete");
  if (this->unsubscribeUITickListener == nullptr) {
    return;
  }
  this->unsubscribeUITickListener();
  this->unsubscribeUITickListener = nullptr;
}

void RNInstanceCAPI::onUITick(UITicker::Timestamp /*recentVSyncTimestamp*/) {
  facebook::react::SystraceSection s("#RNOH::RNInstanceCAPI::onUITick");
  if (this->scheduler != nullptr) {
    this->scheduler->animationTick();
  }
}

void RNInstanceCAPI::attachRootView(
    NodeContentHandle nodeContentHandle,
    facebook::react::Tag surfaceId) {
  DLOG(INFO) << "RNInstanceCAPI::registerNodeContentHandle";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    LOG(ERROR) << "Surface with id: " << surfaceId << " not found";
    return;
  }
  it->second->attachToNodeContent(std::move(nodeContentHandle));
}

void RNInstanceCAPI::detachRootView(facebook::react::Tag surfaceId) {
  DLOG(INFO) << "RNInstanceCAPI::registerNodeContentHandle";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    LOG(ERROR) << "Surface with id: " << surfaceId << " not found";
    return;
  }
  it->second->detachFromNodeContent();
}

TurboModule::Shared RNInstanceCAPI::getTurboModule(const std::string& name) {
  auto turboModule = m_turboModuleProvider->getTurboModule(name);
  if (turboModule) {
    auto rnohTurboModule = std::dynamic_pointer_cast<TurboModule>(turboModule);
    if (rnohTurboModule) {
      return rnohTurboModule;
    } else {
      LOG(ERROR) << "TurboModule '" << name
                 << "' should extend rnoh::TurboModule";
      return nullptr;
    }
  }
  return nullptr;
}

void RNInstanceCAPI::createSurface(
    facebook::react::Tag surfaceId,
    std::string const& moduleName) {
  DLOG(INFO) << "RNInstanceCAPI::createSurface";
  m_surfaceById.emplace(
      surfaceId,
      std::make_shared<ArkUISurface>(
          taskExecutor,
          scheduler,
          m_componentInstanceRegistry,
          m_componentInstanceFactory,
          m_arkTSMessageHub,
          surfaceId,
          m_id,
          moduleName));
}

void RNInstanceCAPI::updateSurfaceConstraints(
    facebook::react::Tag surfaceId,
    float minWidth,
    float minHeight,
    float maxWidth,
    float maxHeight,
    float viewportOffsetX,
    float viewportOffsetY,
    float pixelRatio,
    bool isRTL) {
  DLOG(INFO) << "RNInstanceCAPI::updateSurfaceConstraints";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    return;
  }
  it->second->updateConstraints(
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
      viewportOffsetX,
      viewportOffsetY,
      pixelRatio,
      isRTL);
}

facebook::react::Size RNInstanceCAPI::measureSurface(
    facebook::react::Tag surfaceId,
    float minWidth,
    float minHeight,
    float maxWidth,
    float maxHeight,
    float viewportOffsetX,
    float viewportOffsetY,
    float pixelRatio,
    bool isRTL) {
  DLOG(INFO) << "RNInstanceCAPI::measureSurface";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    throw FatalRNOHError("Invalid surfaceId");
  }
  return it->second->measure(
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
      viewportOffsetX,
      viewportOffsetY,
      pixelRatio,
      isRTL);
}

void RNInstanceCAPI::startSurface(
    facebook::react::Tag surfaceId,
    float minWidth,
    float minHeight,
    float maxWidth,
    float maxHeight,
    float viewportOffsetX,
    float viewportOffsetY,
    float pixelRatio,
    bool isRTL,
    folly::dynamic&& initialProps) {
  DLOG(INFO) << "RNInstanceCAPI::startSurface";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    return;
  }
  it->second->start(
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
      viewportOffsetX,
      viewportOffsetY,
      pixelRatio,
      isRTL,
      std::move(initialProps),
      m_animationDriver);
}

void RNInstanceCAPI::setSurfaceProps(
    facebook::react::Tag surfaceId,
    folly::dynamic&& props) {
  DLOG(INFO) << "RNInstanceCAPI::setSurfaceProps";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    return;
  }
  it->second->setProps(std::move(props));
}

void RNInstanceCAPI::stopSurface(
    facebook::react::Tag surfaceId,
    std::function<void()> onStop) {
  DLOG(INFO) << "RNInstanceCAPI::stopSurface";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    return;
  }
  it->second->stop(std::move(onStop));
}

void RNInstanceCAPI::destroySurface(facebook::react::Tag surfaceId) {
  DLOG(INFO) << "RNInstanceCAPI::destroySurface";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    return;
  }
  m_surfaceById.erase(it);
}

void RNInstanceCAPI::setSurfaceDisplayMode(
    facebook::react::Tag surfaceId,
    facebook::react::DisplayMode displayMode) {
  DLOG(INFO) << "RNInstanceCAPI::setSurfaceDisplayMode";
  auto it = m_surfaceById.find(surfaceId);
  if (it == m_surfaceById.end()) {
    return;
  }
  it->second->setDisplayMode(displayMode);
}

ComponentInstance::Shared RNInstanceCAPI::findComponentInstanceByTag(
    facebook::react::Tag tag) {
  return m_componentInstanceRegistry->findByTag(tag);
}

std::optional<facebook::react::Tag>
RNInstanceCAPI::findComponentInstanceTagById(const std::string& id) {
  return m_componentInstanceRegistry->findTagById(id);
}

std::optional<std::string> RNInstanceCAPI::getNativeNodeIdByTag(
    facebook::react::Tag tag) const {
  auto componentInstance = m_componentInstanceRegistry->findByTag(tag);
  if (componentInstance == nullptr) {
    return std::nullopt;
  }
  return componentInstance->getLocalRootArkUINode().getId();
}

void RNInstanceCAPI::onConfigurationChange(folly::dynamic const& payload) {
  if (payload.isNull()) {
    return;
  }
  folly::dynamic windowPhysicalPixels = payload["windowPhysicalPixels"];
  if (windowPhysicalPixels.isNull() ||
      !windowPhysicalPixels["densityDpi"].isDouble()) {
    return;
  }
  float densityDpi = windowPhysicalPixels["densityDpi"].asDouble();
  if (densityDpi == m_densityDpi) {
    return;
  }
  m_densityDpi = densityDpi;
  if (windowPhysicalPixels["scale"].isDouble() &&
      windowPhysicalPixels["fontScale"].isDouble()) {
    float scale = windowPhysicalPixels["scale"].asDouble();
    float fontScale = windowPhysicalPixels["fontScale"].asDouble();
    auto halfLeading =
        ArkTSBridge::getInstance()->getMetadata("half_leading") == "true";
    auto textMeasurer =
        m_contextContainer->at<std::shared_ptr<rnoh::TextMeasurer>>(
            "textLayoutManagerDelegate");
    if (textMeasurer) {
      textMeasurer->setTextMeasureParams(
          fontScale, scale, densityDpi, halfLeading);
    }
    float xDpi = windowPhysicalPixels["xDpi"].asDouble();
    float yDpi = windowPhysicalPixels["yDpi"].asDouble();
    if (xDpi == 0.0f || yDpi == 0.0f) {
      return;
    }
    float scaleRatioDpiX = scale / xDpi;
    float scaleRatioDpiY = scale / yDpi;
    ArkTSBridge::getInstance()->setScaleRatioDpi(
        scaleRatioDpiX, scaleRatioDpiY);
  }
}

void RNInstanceCAPI::handleArkTSMessage(
    const std::string& name,
    folly::dynamic const& payload) {
  facebook::react::SystraceSection s("RNInstanceCAPI::handleArkTSMessage");
  if (name == "CONFIGURATION_UPDATE") {
    onConfigurationChange(payload);
  }
  for (auto& arkTSMessageHandler : m_arkTSMessageHandlers) {
    arkTSMessageHandler->handleArkTSMessage(
        {.messageName = name,
         .messagePayload = payload,
         .rnInstance = this->shared_from_this()});
  }
}

void RNInstanceCAPI::addArkTSMessageHandler(
    ArkTSMessageHandler::Shared handler) {
  m_arkTSMessageHandlers.push_back(handler);
}

void RNInstanceCAPI::removeArkTSMessageHandler(
    ArkTSMessageHandler::Shared handler) {
  for (auto it = m_arkTSMessageHandlers.begin();
       it != m_arkTSMessageHandlers.end();
       it++) {
    if (*it == handler) {
      m_arkTSMessageHandlers.erase(it);
    }
  }
}

void RNInstanceCAPI::postMessageToArkTS(
    const std::string& name,
    folly::dynamic const& payload) {
  m_arkTSChannel->postMessage(name, payload);
}

void RNInstanceCAPI::registerFont(
    std::string const& fontFamily,
    std::string const& fontFilePath) {
  m_contextContainer
      ->at<std::shared_ptr<rnoh::TextMeasurer>>("textLayoutManagerDelegate")
      ->registerFont(m_nativeResourceManager, fontFamily, fontFilePath);
}

} // namespace rnoh
