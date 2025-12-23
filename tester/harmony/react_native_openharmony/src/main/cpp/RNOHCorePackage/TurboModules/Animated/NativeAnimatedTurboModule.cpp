/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "NativeAnimatedTurboModule.h"
#include "RNOH/ArkTSBridge.h"

#include <jsi/jsi/JSIDynamic.h>
#include "AnimatedNodesManager.h"
#include "RNOH/RNInstance.h"
#include "RNOH/RNInstanceCAPI.h"

using namespace facebook;

namespace rnoh {

jsi::Value startOperationBatch(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->startOperationBatch();
  return facebook::jsi::Value::undefined();
}

jsi::Value finishOperationBatch(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->finishOperationBatch();
  return facebook::jsi::Value::undefined();
}

jsi::Value createAnimatedNode(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  auto config = jsi::dynamicFromValue(rt, args[1]);
  self->createAnimatedNode(args[0].getNumber(), config);
  return facebook::jsi::Value::undefined();
}

jsi::Value updateAnimatedNodeConfig(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->updateAnimatedNodeConfig(args[0].getNumber(), args[1]);
  return facebook::jsi::Value::undefined();
}

jsi::Value getValue(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  auto value = self->getValue(args[0].getNumber());
  args[1].getObject(rt).getFunction(rt).call(rt, value);
  return facebook::jsi::Value::undefined();
}

jsi::Value startListeningToAnimatedNodeValue(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->startListeningToAnimatedNodeValue(rt, args[0].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value stopListeningToAnimatedNodeValue(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->stopListeningToAnimatedNodeValue(args[0].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value connectAnimatedNodes(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->connectAnimatedNodes(args[0].getNumber(), args[1].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value disconnectAnimatedNodes(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->disconnectAnimatedNodes(args[0].getNumber(), args[1].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value startAnimatingNode(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  auto animationId = args[0].getNumber();
  auto config = jsi::dynamicFromValue(rt, args[2]);
  if (args[3].isUndefined()) {
    self->startAnimatingNode(
        animationId,
        args[1].getNumber(),
        config,
        [self, &rt, animationId](bool finished, std::optional<double> value) {
          self->emitAnimationEndedEvent(rt, animationId, finished, value);
        });
    return facebook::jsi::Value::undefined();
  }
  auto callback = std::make_shared<jsi::Function>(
      std::move(args[3].getObject(rt).getFunction(rt)));
  auto endCallback = [&rt, callback = std::move(callback)](
                         bool finished, std::optional<double> value) {
    auto result = jsi::Object(rt);
    result.setProperty(rt, "finished", jsi::Value(finished));
    if (value.has_value()) {
      result.setProperty(rt, "value", value.value());
    }
    callback->call(rt, {std::move(result)});
  };
  self->startAnimatingNode(
      args[0].getNumber(), args[1].getNumber(), config, std::move(endCallback));
  return facebook::jsi::Value::undefined();
}

jsi::Value stopAnimation(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->stopAnimation(args[0].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value setAnimatedNodeValue(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->setAnimatedNodeValue(args[0].getNumber(), args[1].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value setAnimatedNodeOffset(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->setAnimatedNodeOffset(args[0].getNumber(), args[1].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value flattenAnimatedNodeOffset(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->flattenAnimatedNodeOffset(args[0].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value extractAnimatedNodeOffset(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->extractAnimatedNodeOffset(args[0].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value connectAnimatedNodeToView(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->connectAnimatedNodeToView(args[0].getNumber(), args[1].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value disconnectAnimatedNodeFromView(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->disconnectAnimatedNodeFromView(
      args[0].getNumber(), args[1].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value restoreDefaultValues(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->restoreDefaultValues(args[0].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value dropAnimatedNode(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->dropAnimatedNode(args[0].getNumber());
  return facebook::jsi::Value::undefined();
}

jsi::Value addAnimatedEventToView(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  auto dynamicEventMapping = jsi::dynamicFromValue(rt, args[2]);
  self->addAnimatedEventToView(
      args[0].getNumber(), args[1].getString(rt).utf8(rt), dynamicEventMapping);
  return facebook::jsi::Value::undefined();
}

jsi::Value removeAnimatedEventFromView(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  self->removeAnimatedEventFromView(
      args[0].getNumber(), args[1].getString(rt).utf8(rt), args[2].getNumber());
  return facebook::jsi::Value::undefined();
}

bool NativeAnimatedTurboModule::isDisplaySoloistRegistered() const {
  return m_isDisplaySoloistRegistered.load();
}

void NativeAnimatedTurboModule::requestAnimationFrame() {
  m_vsyncListener->requestFrame(
      [weakSelf = weak_from_this()](long long _timestamp) {
        if (auto self = weakSelf.lock()) {
          self->runUpdates(_timestamp);
        }
      });
}

void NativeAnimatedTurboModule::setDisplaySoloistFrameRate(int32_t frameRate) {
  if (frameRate != m_currentFrameRate) {
    DisplaySoloist_ExpectedRateRange range = {0, 120, frameRate};
    int setStatus = OH_DisplaySoloist_SetExpectedFrameRateRange(
        m_nativeDisplaySoloist.get(), &range);
    if (setStatus != 0) {
      LOG(ERROR)
          << "[DisplaySoloist] Failed to set expected frame rate range. Error code: "
          << setStatus;
      return;
    }
    m_currentFrameRate = frameRate;
  }
}

void NativeAnimatedTurboModule::stopDisplaySoloist() {
  if (isDisplaySoloistRegistered()) {
    int stopStatus = OH_DisplaySoloist_Stop(m_nativeDisplaySoloist.get());
    if (stopStatus != 0) {
      LOG(ERROR)
          << "[DisplaySoloist] Failed to stop DisplaySoloist. Error code: "
          << stopStatus;
      return;
    }
    m_isDisplaySoloistRegistered.store(false);
  }
}

void NativeAnimatedTurboModule::startDisplaySoloist() {
  if (isDisplaySoloistRegistered()) {
    return;
  }
  int startStatus = OH_DisplaySoloist_Start(
      m_nativeDisplaySoloist.get(),
      [](long long timestamp, long long targetTimestamp, void* data) {
        auto* nativeAnimatedTM = static_cast<NativeAnimatedTurboModule*>(data);
        if (!nativeAnimatedTM) {
          LOG(ERROR)
              << "[DisplaySoloist] Start: Failed to get NativeAnimatedTurboModule.";
          return;
        }
        nativeAnimatedTM->runUpdates(timestamp);
      },
      this);
  if (startStatus != 0) {
    LOG(ERROR)
        << "[DisplaySoloist] Failed to start DisplaySoloist. Error code: "
        << startStatus;
    return;
  }
  m_isDisplaySoloistRegistered.store(true);
}

NativeAnimatedTurboModule::NativeAnimatedTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : rnoh::ArkTSTurboModule(ctx, name),
      m_nativeDisplaySoloist(
          IsAtLeastApi20() ? OH_DisplaySoloist_Create(false) : nullptr,
          IsAtLeastApi20()
              ? [](OH_DisplaySoloist* d) { OH_DisplaySoloist_Destroy(d); }
              : [](OH_DisplaySoloist* d) {}),
      m_isDisplaySoloistRegistered(false),
      m_animatedNodesManager(
          [this](int frameRate) {
            this->setDisplaySoloistFrameRate(frameRate);
          },
          [this]() {
            if (IsAtLeastApi20()) {
              this->startDisplaySoloist();
            } else {
              this->requestAnimationFrame();
            }
          },
          [this]() { this->stopDisplaySoloist(); }) {
  methodMap_ = {
      {"startOperationBatch", {0, rnoh::startOperationBatch}},
      {"finishOperationBatch", {0, rnoh::finishOperationBatch}},
      {"createAnimatedNode", {2, rnoh::createAnimatedNode}},
      {"updateAnimatedNodeConfig", {2, rnoh::updateAnimatedNodeConfig}},
      {"getValue", {2, rnoh::getValue}},
      {"connectAnimatedNodes", {2, rnoh::connectAnimatedNodes}},
      {"disconnectAnimatedNodes", {2, rnoh::disconnectAnimatedNodes}},
      {"startAnimatingNode", {4, rnoh::startAnimatingNode}},
      {"stopAnimation", {1, rnoh::stopAnimation}},
      {"setAnimatedNodeValue", {2, rnoh::setAnimatedNodeValue}},
      {"setAnimatedNodeOffset", {2, rnoh::setAnimatedNodeOffset}},
      {"flattenAnimatedNodeOffset", {1, rnoh::flattenAnimatedNodeOffset}},
      {"extractAnimatedNodeOffset", {1, rnoh::extractAnimatedNodeOffset}},
      {"connectAnimatedNodeToView", {2, rnoh::connectAnimatedNodeToView}},
      {"disconnectAnimatedNodeFromView",
       {2, rnoh::disconnectAnimatedNodeFromView}},
      {"restoreDefaultValues", {1, rnoh::restoreDefaultValues}},
      {"dropAnimatedNode", {1, rnoh::dropAnimatedNode}},
      {"addAnimatedEventToView", {3, rnoh::addAnimatedEventToView}},
      {"removeAnimatedEventFromView", {3, rnoh::removeAnimatedEventFromView}},
      {"startListeningToAnimatedNodeValue",
       {1, rnoh::startListeningToAnimatedNodeValue}},
      {"stopListeningToAnimatedNodeValue",
       {1, rnoh::stopListeningToAnimatedNodeValue}}};
}

NativeAnimatedTurboModule::~NativeAnimatedTurboModule() {
  stopDisplaySoloist();
  if (m_initializedEventListener) {
    m_ctx.eventDispatcher->unregisterExpiredListeners();
  }
}

void NativeAnimatedTurboModule::startOperationBatch() {}

void NativeAnimatedTurboModule::finishOperationBatch() {}

void NativeAnimatedTurboModule::createAnimatedNode(
    react::Tag tag,
    folly::dynamic const& config) {
  auto lock = acquireLock();
  m_animatedNodesManager.createNode(tag, config);
}

void NativeAnimatedTurboModule::updateAnimatedNodeConfig(
    react::Tag tag,
    const jsi::Value& config) {}

double NativeAnimatedTurboModule::getValue(react::Tag tag) {
  auto lock = acquireLock();
  auto output = m_animatedNodesManager.getNodeOutput(tag);
  RNOH_ASSERT(output.isDouble());
  return output.asDouble();
}

void NativeAnimatedTurboModule::startListeningToAnimatedNodeValue(
    jsi::Runtime& rt,
    react::Tag tag) {
  auto lock = acquireLock();
  m_animatedNodesManager.startListeningToAnimatedNodeValue(
      tag, [this, tag, &rt](double value) {
        this->emitDeviceEvent(
            rt,
            "onAnimatedValueUpdate",
            [tag, value](jsi::Runtime& rt, std::vector<jsi::Value>& args) {
              auto payload = jsi::Object(rt);
              payload.setProperty(rt, "tag", tag);
              payload.setProperty(rt, "value", value);
              args.push_back(std::move(payload));
            });
      });
}

void NativeAnimatedTurboModule::stopListeningToAnimatedNodeValue(
    react::Tag tag) {
  auto lock = acquireLock();
  m_animatedNodesManager.stopListeningToAnimatedNodeValue(tag);
}

void NativeAnimatedTurboModule::connectAnimatedNodes(
    react::Tag parentNodeTag,
    react::Tag childNodeTag) {
  auto lock = acquireLock();
  m_animatedNodesManager.connectNodes(parentNodeTag, childNodeTag);
}

void NativeAnimatedTurboModule::disconnectAnimatedNodes(
    react::Tag parentNodeTag,
    react::Tag childNodeTag) {
  auto lock = acquireLock();
  m_animatedNodesManager.disconnectNodes(parentNodeTag, childNodeTag);
}

void NativeAnimatedTurboModule::startAnimatingNode(
    react::Tag animationId,
    react::Tag nodeTag,
    folly::dynamic const& config,
    EndCallback&& endCallback) {
  auto jsThreadCallback =
      [jsInvoker = this->jsInvoker_, endCallback = std::move(endCallback)](
          bool finished, std::optional<double> value) mutable {
        // callbacks passed from JS need to be called through the jsInvoker
        // to ensure proper handling by React
        jsInvoker->invokeAsync(
            [finished, value, endCallback = std::move(endCallback)] {
              endCallback(finished, value);
            });
      };
  auto lock = acquireLock();
  m_animatedNodesManager.startAnimatingNode(
      animationId, nodeTag, config, std::move(jsThreadCallback));
}

void NativeAnimatedTurboModule::stopAnimation(react::Tag animationId) {
  auto lock = acquireLock();
  m_animatedNodesManager.stopAnimation(animationId);
}

void NativeAnimatedTurboModule::setAnimatedNodeValue(
    react::Tag nodeTag,
    double value) {
  auto lock = acquireLock();
  m_animatedNodesManager.setValue(nodeTag, value);
}

void NativeAnimatedTurboModule::setAnimatedNodeOffset(
    react::Tag nodeTag,
    double offset) {
  auto lock = acquireLock();
  m_animatedNodesManager.setOffset(nodeTag, offset);
}

void NativeAnimatedTurboModule::flattenAnimatedNodeOffset(react::Tag nodeTag) {
  auto lock = acquireLock();
  m_animatedNodesManager.flattenOffset(nodeTag);
}

void NativeAnimatedTurboModule::extractAnimatedNodeOffset(react::Tag nodeTag) {
  auto lock = acquireLock();
  m_animatedNodesManager.extractOffset(nodeTag);
}

void NativeAnimatedTurboModule::connectAnimatedNodeToView(
    react::Tag nodeTag,
    react::Tag viewTag) {
  auto lock = acquireLock();
  m_animatedNodesManager.connectNodeToView(nodeTag, viewTag);
}

void NativeAnimatedTurboModule::disconnectAnimatedNodeFromView(
    react::Tag nodeTag,
    react::Tag viewTag) {
  auto lock = acquireLock();
  m_animatedNodesManager.disconnectNodeFromView(nodeTag, viewTag);
}

void NativeAnimatedTurboModule::restoreDefaultValues(react::Tag nodeTag) {}

void NativeAnimatedTurboModule::dropAnimatedNode(react::Tag tag) {
  auto lock = acquireLock();
  m_animatedNodesManager.dropNode(tag);
}

void NativeAnimatedTurboModule::addAnimatedEventToView(
    react::Tag viewTag,
    std::string const& eventName,
    folly::dynamic const& eventMapping) {
  auto lock = acquireLock();
  initializeEventListener();
  m_animatedNodesManager.addAnimatedEventToView(
      viewTag, eventName, eventMapping);
}

void NativeAnimatedTurboModule::removeAnimatedEventFromView(
    facebook::react::Tag viewTag,
    std::string const& eventName,
    facebook::react::Tag animatedValueTag) {
  auto lock = acquireLock();
  m_animatedNodesManager.removeAnimatedEventFromView(
      viewTag, eventName, animatedValueTag);
}

void NativeAnimatedTurboModule::addListener(const std::string& eventName) {}

void NativeAnimatedTurboModule::removeListeners(double count) {}

void NativeAnimatedTurboModule::runUpdates(long long frameTimeNanos) {
  ArkJS arkJs(m_ctx.env);
  auto lock = this->acquireLock();
  try {
    auto tagsToUpdate = this->m_animatedNodesManager.runUpdates(frameTimeNanos);

    if (m_ctx.taskExecutor->isOnTaskThread(TaskThread::MAIN)) {
      this->setNativeProps(tagsToUpdate);
      return;
    }
    m_ctx.taskExecutor->runTask(
        TaskThread::MAIN,
        [weakSelf = weak_from_this(), tagsToUpdate = std::move(tagsToUpdate)] {
          auto self = weakSelf.lock();
          if (!self) {
            return;
          }
          self->setNativeProps(tagsToUpdate);
        });
  } catch (std::exception& e) {
    LOG(ERROR) << "Error in animation update: " << e.what();
    if (!IsAtLeastApi20()) {
        this->requestAnimationFrame();
    }
  }
}

void NativeAnimatedTurboModule::setNativeProps(
    PropUpdatesList const& tagsToUpdate) {
  if (auto instance = m_ctx.safeInstance.lock(); instance != nullptr) {
    for (auto const& [tag, props] : tagsToUpdate) {
      instance->synchronouslyUpdateViewOnUIThread(tag, props);
    }
    return;
  }
}

void NativeAnimatedTurboModule::setNativeProps(
    facebook::react::Tag tag,
    folly::dynamic const& props) {
#ifdef C_API_ARCH
  if (auto instance = m_ctx.safeInstance.lock(); instance != nullptr) {
    instance->synchronouslyUpdateViewOnUIThread(tag, props);
    return;
  }
#endif
  // NOTE: in ArkTS architecture, we used to construct the `transform`
  // property in a way incompatible with RN's RawProps. With C-API we fix it
  // in the TransformAnimatedNode, but we pass it the old way to ArkTS to
  // preserve compatibility.
  auto clonedProps = props;
  if (auto it = props.find("transform"); it != props.items().end()) {
    auto& transform = it->second;
    clonedProps["transform"] = transform[0]["matrix"];
  }

  ArkJS arkJs(m_ctx.env);
  auto napiTag = arkJs.createInt(tag);
  auto napiProps = arkJs.createFromDynamic(clonedProps);

  auto napiTurboModuleObject =
      arkJs.getObject(m_ctx.arkTSTurboModuleInstanceRef);
  napiTurboModuleObject.call("setViewProps", {napiTag, napiProps});
}

void NativeAnimatedTurboModule::emitAnimationEndedEvent(
    facebook::jsi::Runtime& rt,
    facebook::react::Tag animationId,
    bool finished,
    std::optional<double> value) {
  emitDeviceEvent(
      rt,
      "onNativeAnimatedModuleAnimationFinished",
      [animationId, finished, value](
          facebook::jsi::Runtime& rt, std::vector<jsi::Value>& args) {
        jsi::Object param(rt);
        param.setProperty(rt, "animationId", animationId);
        param.setProperty(rt, "finished", finished);
        if (value.has_value()) {
          param.setProperty(rt, "value", value.value());
        }
        args.emplace_back(std::move(param));
      });
}

void NativeAnimatedTurboModule::handleEvent(
    EventEmitRequestHandler::Context const& ctx) {
  ArkJS arkJs(ctx.env);
  folly::dynamic payload = arkJs.getDynamic(ctx.payload);
  react::Tag tag = ctx.tag;
  auto eventName = ctx.eventName;

  handleComponentEvent(tag, eventName, payload);
}

void NativeAnimatedTurboModule::initializeEventListener() {
  if (m_initializedEventListener) {
    return;
  }

  m_ctx.eventDispatcher->registerEventListener(shared_from_this());
  m_initializedEventListener = true;
}

void NativeAnimatedTurboModule::handleComponentEvent(
    facebook::react::Tag tag,
    std::string const& eventName,
    folly::dynamic payload) {
  auto lock = acquireLock();
  try {
    auto propUpdates =
        m_animatedNodesManager.handleEvent(tag, eventName, payload);
    setNativeProps(propUpdates);
  } catch (std::exception& e) {
    LOG(ERROR) << "handleEvent Error in animation update: " << e.what();
  }
}
} // namespace rnoh
