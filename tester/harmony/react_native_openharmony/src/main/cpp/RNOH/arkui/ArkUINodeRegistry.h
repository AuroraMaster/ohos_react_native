/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <arkui/native_node.h>
#include <functional>
#include <unordered_map>
#include "RNOH/ArkTSBridge.h"

namespace rnoh {

class ArkUINode;

using EventArgs = ArkUI_NumberValue[MAX_COMPONENT_EVENT_ARG_NUM];

class TouchEventHandler {
 public:
  virtual void onTouchEvent(ArkUI_UIInputEvent* event) = 0;
  virtual ~TouchEventHandler() = default;
};

class ArkUINodeRegistry {
  static std::unique_ptr<ArkUINodeRegistry> instance;

 public:
  static void initialize(ArkTSBridge::Shared arkTSBridge);
  static ArkUINodeRegistry& getInstance();

  void registerNode(ArkUINode* node);
  void unregisterNode(ArkUINode* node);

  void registerTouchHandler(
      ArkUINode* node,
      TouchEventHandler* touchEventHandler);
  void unregisterTouchHandler(ArkUINode* node);

 private:
  ArkUINodeRegistry(ArkTSBridge::Shared arkTSBridge);

  void receiveEvent(ArkUI_NodeEvent* event);

  std::unordered_map<ArkUI_NodeHandle, ArkUINode*> m_nodeByHandle;
  std::unordered_map<ArkUI_NodeHandle, TouchEventHandler*>
      m_touchHandlerByNodeHandle;
  ArkTSBridge::Shared m_arkTSBridge;
};

} // namespace rnoh