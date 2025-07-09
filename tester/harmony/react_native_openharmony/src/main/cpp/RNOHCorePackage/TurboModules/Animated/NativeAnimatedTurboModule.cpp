/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "NativeAnimatedTurboModule.h"
#include "RNOH/ArkTSBridge.h"

#include <jsi/jsi/JSIDynamic.h>
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
  auto tag = args[1].getNumber();
  auto config = jsi::dynamicFromValue(rt, args[2]);
  // If the endCallback argument is not provided, Animated runs in
  // single-operation mode, relying on RCTDeviceEventEmitter to handle events.
  if (count < 4) {
    self->startAnimatingNode(
        animationId, tag, config, [self, &rt, animationId](bool finished) {
          self->emitAnimationEndedEvent(rt, animationId, finished);
        });
    return facebook::jsi::Value::undefined();
  }
  auto endCallbackWrapped = self->createCallbackWrapper(
      std::move(args[3].getObject(rt).getFunction(rt)), rt);
  self->startAnimatingNode(
      animationId,
      tag,
      config,
      self->wrapEndCallbackWithJSInvoker(
          [&rt, endCallbackWrapped = std::move(endCallbackWrapped)](
              bool finished) {
            auto endCallback = endCallbackWrapped.lock();
            if (endCallback == nullptr) {
              return;
            }
            auto result = jsi::Object(rt);
            result.setProperty(rt, "finished", jsi::Value(finished));
            endCallback->callback().call(rt, {std::move(result)});
            endCallback->allowRelease();
          }));
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

jsi::Value queueAndExecuteBatchedOperations(
    facebook::jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const facebook::jsi::Value* args,
    size_t count) {
  if (count < 1) {
    return facebook::jsi::Value::undefined();
  }
  auto self = static_cast<NativeAnimatedTurboModule*>(&turboModule);
  auto opsAndArgs = args[0].getObject(rt).getArray(rt);

  /**
   * Batching is necessary to maintain the consistency of AnimatedNodesManager's
   * state whenever the UI thread interacts with it (e.g. through
   * handleComponentEvent). Therefore, the lock is acquired here rather than in
   * individual methods. Those methods are not expected to be invoked from
   * JavaScript anyway despite them being listed in the spec. (They are listed
   * in the spec because the spec file is shared between Android and iOS, and
   * iOS can call those methods directly due to different batching mechanism.)
   * This Animated implementation uses "SingleOpBatching" Animated mode used on
   * Android.
   */
  auto lock = self->acquireLock();
  for (size_t i = 0; i < opsAndArgs.size(rt);) {
    auto command =
        static_cast<NativeAnimatedTurboModule::BatchExecutionOpCodes>(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
    switch (command) {
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_CREATE_ANIMATED_NODE:
        self->createAnimatedNode(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            jsi::dynamicFromValue(rt, opsAndArgs.getValueAtIndex(rt, i++)));
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_UPDATE_ANIMATED_NODE_CONFIG:
        self->updateAnimatedNodeConfig(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++));
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::OP_CODE_GET_VALUE:
        self->getValue(opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_START_LISTENING_TO_ANIMATED_NODE_VALUE: {
        self->startListeningToAnimatedNodeValue(
            rt, opsAndArgs.getValueAtIndex(rt, i++).asNumber());
      } break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_STOP_LISTENING_TO_ANIMATED_NODE_VALUE:
        self->stopListeningToAnimatedNodeValue(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_CONNECT_ANIMATED_NODES:
        self->connectAnimatedNodes(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_DISCONNECT_ANIMATED_NODES:
        self->disconnectAnimatedNodes(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_START_ANIMATING_NODE: {
        auto animationId = opsAndArgs.getValueAtIndex(rt, i++).asNumber();
        self->startAnimatingNode(
            animationId,
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            jsi::dynamicFromValue(rt, opsAndArgs.getValueAtIndex(rt, i++)),
            [self, &rt, animationId](bool finished) {
              self->emitAnimationEndedEvent(rt, animationId, finished);
            });
        break;
      }
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_STOP_ANIMATION:
        self->stopAnimation(opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_SET_ANIMATED_NODE_VALUE:
        self->setAnimatedNodeValue(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_SET_ANIMATED_NODE_OFFSET:
        self->setAnimatedNodeOffset(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_FLATTEN_ANIMATED_NODE_OFFSET:
        self->flattenAnimatedNodeOffset(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_EXTRACT_ANIMATED_NODE_OFFSET:
        self->extractAnimatedNodeOffset(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_CONNECT_ANIMATED_NODE_TO_VIEW:
        self->connectAnimatedNodeToView(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_DISCONNECT_ANIMATED_NODE_FROM_VIEW: {
        self->disconnectAnimatedNodeFromView(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
      } break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_RESTORE_DEFAULT_VALUES:
        self->restoreDefaultValues(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_DROP_ANIMATED_NODE:
        self->dropAnimatedNode(opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_ADD_ANIMATED_EVENT_TO_VIEW:
        self->addAnimatedEventToView(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++).asString(rt).utf8(rt),
            jsi::dynamicFromValue(rt, opsAndArgs.getValueAtIndex(rt, i++)));
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_REMOVE_ANIMATED_EVENT_FROM_VIEW:
        self->removeAnimatedEventFromView(
            opsAndArgs.getValueAtIndex(rt, i++).asNumber(),
            opsAndArgs.getValueAtIndex(rt, i++).asString(rt).utf8(rt),
            opsAndArgs.getValueAtIndex(rt, i++).asNumber());
        break;
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_ADD_LISTENER:
      case NativeAnimatedTurboModule::BatchExecutionOpCodes::
          OP_CODE_REMOVE_LISTENERS:
        i++;
        break;
    }
  }
  return facebook::jsi::Value::undefined();
}

NativeAnimatedTurboModule::NativeAnimatedTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : rnoh::ArkTSTurboModule(ctx, name),
      m_nativeDisplaySoloist(
          OH_DisplaySoloist_Create(false),
          &OH_DisplaySoloist_Destroy),
      m_isDisplaySoloistRegistered(false),
      m_animatedNodesManager(
          [this](int frameRate) {
            this->setDisplaySoloistFrameRate(frameRate);
          },
          [this]() { this->stopDisplaySoloist(); },
          [this]() { this->startDisplaySoloist(); }) {
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
       {1, rnoh::stopListeningToAnimatedNodeValue}},
      {"queueAndExecuteBatchedOperations",
       {1, rnoh::queueAndExecuteBatchedOperations}}

  };
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
  m_animatedNodesManager.createNode(tag, config);
}

void NativeAnimatedTurboModule::updateAnimatedNodeConfig(
    react::Tag tag,
    const jsi::Value& config) {}

void NativeAnimatedTurboModule::startAnimatingNode(
    react::Tag animationId,
    react::Tag nodeTag,
    folly::dynamic const& config,
    EndCallback&& endCallback) {
  m_animatedNodesManager.startAnimatingNode(
      animationId, nodeTag, config, std::move(endCallback));
}

double NativeAnimatedTurboModule::getValue(react::Tag tag) {
  auto output = m_animatedNodesManager.getNodeOutput(tag);
  RNOH_ASSERT(output.isDouble());
  return output.asDouble();
}

void NativeAnimatedTurboModule::startListeningToAnimatedNodeValue(
    jsi::Runtime& rt,
    react::Tag tag) {
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
  m_animatedNodesManager.stopListeningToAnimatedNodeValue(tag);
}

void NativeAnimatedTurboModule::connectAnimatedNodes(
    react::Tag parentNodeTag,
    react::Tag childNodeTag) {
  m_animatedNodesManager.connectNodes(parentNodeTag, childNodeTag);
}

void NativeAnimatedTurboModule::disconnectAnimatedNodes(
    react::Tag parentNodeTag,
    react::Tag childNodeTag) {
  m_animatedNodesManager.disconnectNodes(parentNodeTag, childNodeTag);
}

std::weak_ptr<facebook::react::CallbackWrapper> NativeAnimatedTurboModule::createCallbackWrapper(
    facebook::jsi::Function&& callback,
    facebook::jsi::Runtime& runtime) {
  return react::CallbackWrapper::createWeak(
      std::move(callback), runtime, this->jsInvoker_);
}

NativeAnimatedTurboModule::EndCallback NativeAnimatedTurboModule::wrapEndCallbackWithJSInvoker(
    EndCallback&& endCallback) {
  return [jsInvoker = this->jsInvoker_, endCallback = std::move(endCallback)](
             bool finished) mutable {
    jsInvoker->invokeAsync(
        [finished, endCallback = std::move(endCallback)] {
          endCallback(finished);
        });
  };
}

void NativeAnimatedTurboModule::stopAnimation(react::Tag animationId) {
  m_animatedNodesManager.stopAnimation(animationId);
}

void NativeAnimatedTurboModule::setAnimatedNodeValue(
    react::Tag nodeTag,
    double value) {
  m_animatedNodesManager.setValue(nodeTag, value);
}

void NativeAnimatedTurboModule::setAnimatedNodeOffset(
    react::Tag nodeTag,
    double offset) {
  m_animatedNodesManager.setOffset(nodeTag, offset);
}

void NativeAnimatedTurboModule::flattenAnimatedNodeOffset(react::Tag nodeTag) {
  m_animatedNodesManager.flattenOffset(nodeTag);
}

void NativeAnimatedTurboModule::extractAnimatedNodeOffset(react::Tag nodeTag) {
  m_animatedNodesManager.extractOffset(nodeTag);
}

void NativeAnimatedTurboModule::connectAnimatedNodeToView(
    react::Tag nodeTag,
    react::Tag viewTag) {
  m_animatedNodesManager.connectNodeToView(nodeTag, viewTag);
}

void NativeAnimatedTurboModule::disconnectAnimatedNodeFromView(
    react::Tag nodeTag,
    react::Tag viewTag) {
  m_animatedNodesManager.disconnectNodeFromView(nodeTag, viewTag);
}

void NativeAnimatedTurboModule::restoreDefaultValues(react::Tag nodeTag) {}

void NativeAnimatedTurboModule::dropAnimatedNode(react::Tag tag) {
  m_animatedNodesManager.dropNode(tag);
}

void NativeAnimatedTurboModule::addAnimatedEventToView(
    react::Tag viewTag,
    std::string const& eventName,
    folly::dynamic const& eventMapping) {
  initializeEventListener();
  m_animatedNodesManager.addAnimatedEventToView(
      viewTag, eventName, eventMapping);
}

void NativeAnimatedTurboModule::removeAnimatedEventFromView(
    facebook::react::Tag viewTag,
    std::string const& eventName,
    facebook::react::Tag animatedValueTag) {
  m_animatedNodesManager.removeAnimatedEventFromView(
      viewTag, eventName, animatedValueTag);
}

void NativeAnimatedTurboModule::addListener(const std::string& eventName) {}

void NativeAnimatedTurboModule::removeListeners(double count) {}

void NativeAnimatedTurboModule::runUpdates(long long frameTimeNanos) {
  ArkJS arkJS(m_ctx.env);
  try {
    if (m_ctx.taskExecutor->isOnTaskThread(TaskThread::MAIN)) {
      auto lock = this->acquireLock();
      auto tagsToUpdate =
          this->m_animatedNodesManager.runUpdates(frameTimeNanos);

      this->setNativeProps(tagsToUpdate);
      return;
    }
    m_ctx.taskExecutor->runTask(
        TaskThread::MAIN,
        [weakSelf = weak_from_this(),
         frameTimeNanos,
         taskScheduleTime = std::chrono::high_resolution_clock::now()] {
          auto now = std::chrono::high_resolution_clock::now();
          auto executionLagInNanos =
              std::chrono::duration_cast<std::chrono::nanoseconds>(
                  now - taskScheduleTime)
                  .count();
          auto self = weakSelf.lock();
          if (!self) {
            return;
          }
          auto lock = self->acquireLock();
          auto tagsToUpdate = self->m_animatedNodesManager.runUpdates(
              frameTimeNanos + executionLagInNanos);
          self->setNativeProps(tagsToUpdate);
        });
  } catch (std::exception& e) {
    LOG(ERROR) << "Error in animation update: " << e.what();
  }
}

void NativeAnimatedTurboModule::setNativeProps(
    PropUpdatesList const& tagsToUpdate) {
  if (auto instance = m_ctx.instance.lock(); instance != nullptr) {
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
  if (auto instance = m_ctx.instance.lock(); instance != nullptr) {
    instance->synchronouslyUpdateViewOnUIThread(tag, props);
    return;
  }
#endif
  // NOTE: in ArkTS architecture, we used to construct the `transform` property
  // in a way incompatible with RN's RawProps. With C-API we fix it in the
  // TransformAnimatedNode, but we pass it the old way to ArkTS to preserve
  // compatibility.
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
    bool finished) {
  emitDeviceEvent(
      rt,
      "onNativeAnimatedModuleAnimationFinished",
      [animationId, finished](
          facebook::jsi::Runtime& rt, std::vector<jsi::Value>& args) {
        jsi::Object param(rt);
        param.setProperty(rt, "animationId", animationId);
        param.setProperty(rt, "finished", finished);
        args.emplace_back(std::move(param));
      });
}

void NativeAnimatedTurboModule::emitAnimationGetValueEvent(
    facebook::jsi::Runtime& rt,
    facebook::react::Tag tag,
    double value) {
  emitDeviceEvent(
      rt,
      "onNativeAnimatedModuleGetValue",
      [tag, value](facebook::jsi::Runtime& rt, std::vector<jsi::Value>& args) {
        jsi::Object param(rt);
        param.setProperty(rt, "tag", tag);
        param.setProperty(rt, "value", value);
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
  auto propUpdates =
      m_animatedNodesManager.handleEvent(tag, eventName, payload);
  setNativeProps(propUpdates);
}
} // namespace rnoh
