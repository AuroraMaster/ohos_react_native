/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <RNOH/ArkTSTurboModule.h>
#include <folly/dynamic.h>
#include <native_display_soloist/native_display_soloist.h>
#include <react/renderer/core/EventListener.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <mutex>

#include "AnimatedNodesManager.h"
#include "RNOH/EventEmitRequestHandler.h"

namespace rnoh {

class NativeAnimatedTurboModule
    : public rnoh::ArkTSTurboModule,
      public rnoh::EventEmitRequestHandler,
      public std::enable_shared_from_this<NativeAnimatedTurboModule> {
 public:
  using Context = rnoh::ArkTSTurboModule::Context;

  using EndCallback = AnimatedNodesManager::EndCallback;

  enum class BatchExecutionOpCodes {
    OP_CODE_CREATE_ANIMATED_NODE = 1,
    OP_CODE_UPDATE_ANIMATED_NODE_CONFIG = 2,
    OP_CODE_GET_VALUE = 3,
    OP_START_LISTENING_TO_ANIMATED_NODE_VALUE = 4,
    OP_STOP_LISTENING_TO_ANIMATED_NODE_VALUE = 5,
    OP_CODE_CONNECT_ANIMATED_NODES = 6,
    OP_CODE_DISCONNECT_ANIMATED_NODES = 7,
    OP_CODE_START_ANIMATING_NODE = 8,
    OP_CODE_STOP_ANIMATION = 9,
    OP_CODE_SET_ANIMATED_NODE_VALUE = 10,
    OP_CODE_SET_ANIMATED_NODE_OFFSET = 11,
    OP_CODE_FLATTEN_ANIMATED_NODE_OFFSET = 12,
    OP_CODE_EXTRACT_ANIMATED_NODE_OFFSET = 13,
    OP_CODE_CONNECT_ANIMATED_NODE_TO_VIEW = 14,
    OP_CODE_DISCONNECT_ANIMATED_NODE_FROM_VIEW = 15,
    OP_CODE_RESTORE_DEFAULT_VALUES = 16,
    OP_CODE_DROP_ANIMATED_NODE = 17,
    OP_CODE_ADD_ANIMATED_EVENT_TO_VIEW = 18,
    OP_CODE_REMOVE_ANIMATED_EVENT_FROM_VIEW = 19,
    OP_CODE_ADD_LISTENER = 20, // ios only
    OP_CODE_REMOVE_LISTENERS = 21 // ios only
  };

  NativeAnimatedTurboModule(
      const ArkTSTurboModule::Context ctx,
      const std::string name);
  ~NativeAnimatedTurboModule() override;

  void startOperationBatch();

  void finishOperationBatch();

  void createAnimatedNode(
      facebook::react::Tag tag,
      folly::dynamic const& config);

  void updateAnimatedNodeConfig(
      facebook::react::Tag tag,
      facebook::jsi::Value const& config);

  double getValue(facebook::react::Tag tag);

  void startListeningToAnimatedNodeValue(
      facebook::jsi::Runtime& rt,
      facebook::react::Tag tag);

  void stopListeningToAnimatedNodeValue(facebook::react::Tag tag);

  void connectAnimatedNodes(
      facebook::react::Tag parentTag,
      facebook::react::Tag childTag);

  void disconnectAnimatedNodes(
      facebook::react::Tag parentTag,
      facebook::react::Tag childTag);

  void startAnimatingNode(
      facebook::react::Tag animationId,
      facebook::react::Tag nodeTag,
      folly::dynamic const& config,
      EndCallback&& endCallback);

  void stopAnimation(facebook::react::Tag animationId);

  void setAnimatedNodeValue(facebook::react::Tag nodeTag, double value);

  void setAnimatedNodeOffset(facebook::react::Tag nodeTag, double offset);

  void flattenAnimatedNodeOffset(facebook::react::Tag nodeTag);

  void extractAnimatedNodeOffset(facebook::react::Tag nodeTag);

  void connectAnimatedNodeToView(
      facebook::react::Tag nodeTag,
      facebook::react::Tag viewTag);

  void disconnectAnimatedNodeFromView(
      facebook::react::Tag nodeTag,
      facebook::react::Tag viewTag);

  void restoreDefaultValues(facebook::react::Tag nodeTag);

  void dropAnimatedNode(facebook::react::Tag tag);

  void addAnimatedEventToView(
      facebook::react::Tag viewTag,
      std::string const& eventName,
      folly::dynamic const& eventMapping);

  void removeAnimatedEventFromView(
      facebook::react::Tag viewTag,
      std::string const& eventName,
      facebook::react::Tag animatedValueTag);

  void addListener(std::string const& eventName);

  void removeListeners(double count);

  void runUpdates(long long frameTimeNanos);

  void setNativeProps(facebook::react::Tag tag, folly::dynamic const& props);
  void setNativeProps(PropUpdatesList const& propUpdatesList);

  void emitAnimationEndedEvent(
      facebook::jsi::Runtime& rt,
      facebook::react::Tag animationId,
      bool completed);

  void emitAnimationGetValueEvent(
      facebook::jsi::Runtime& rt,
      facebook::react::Tag animationId,
      double value);

  // EventEmitRequestHandler
  void handleEvent(EventEmitRequestHandler::Context const& ctx) override;

  void handleComponentEvent(
      facebook::react::Tag tag,
      std::string const& eventName,
      folly::dynamic payload);

  std::weak_ptr<facebook::react::CallbackWrapper> createCallbackWrapper(
      facebook::jsi::Function&& callback,
      facebook::jsi::Runtime& runtime);

  EndCallback wrapEndCallbackWithJSInvoker(EndCallback&& callback);

  std::unique_lock<std::mutex> acquireLock() {
    return std::unique_lock(m_nodesManagerLock);
  }

 private:
  void setDisplaySoloistFrameRate(int32_t frameRate);

  void stopDisplaySoloist();

  void startDisplaySoloist();

  bool isDisplaySoloistRegistered() const;

  // `shared_from_this` cannot be used in constructor,
  // so we defer the initialization of the event listener
  // until the first animated event is registered.
  void initializeEventListener();

  int32_t m_currentFrameRate = 0;
  std::unique_ptr<OH_DisplaySoloist, decltype(&OH_DisplaySoloist_Destroy)>
      m_nativeDisplaySoloist;
  std::atomic<bool> m_isDisplaySoloistRegistered{false};
  AnimatedNodesManager m_animatedNodesManager;
  std::mutex m_nodesManagerLock;
  bool m_initializedEventListener = false;
};

} // namespace rnoh