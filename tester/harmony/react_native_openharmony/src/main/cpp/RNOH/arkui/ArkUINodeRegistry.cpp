/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ArkUINodeRegistry.h"

#include <glog/logging.h>
#include "ArkUINode.h"
#include "NativeNodeApi.h"
#include "RNOH/Assert.h"

namespace rnoh {

std::unique_ptr<ArkUINodeRegistry> ArkUINodeRegistry::instance = nullptr;

void ArkUINodeRegistry::initialize(ArkTSBridge::Shared arkTSBridge) {
  if (instance == nullptr) {
    instance = std::unique_ptr<ArkUINodeRegistry>(
      new ArkUINodeRegistry(std::move(arkTSBridge)));
  }
}

ArkUINodeRegistry& ArkUINodeRegistry::getInstance() {
  RNOH_ASSERT(instance != nullptr);
  return *instance;
}

void ArkUINodeRegistry::registerNode(ArkUINode* node) {
  auto [_it, inserted] =
      m_nodeByHandle.emplace(node->getArkUINodeHandle(), node);
  if (!inserted) {
    DLOG(WARNING) << "Node with handle: " << node->getArkUINodeHandle()
                 << " was already registered";
  }
}

void ArkUINodeRegistry::unregisterNode(ArkUINode* node) {
  auto it = m_nodeByHandle.find(node->getArkUINodeHandle());
  if (it == m_nodeByHandle.end()) {
    DLOG(WARNING) << "Node with handle: " << node->getArkUINodeHandle()
                 << " not found";
    return;
  }

  m_nodeByHandle.erase(it);
}

void ArkUINodeRegistry::registerTouchHandler(
    ArkUINode* node,
    TouchEventHandler* touchEventHandler) {
  DLOG(INFO) << "Registering touch handler for node handle "
             << node->getArkUINodeHandle();
  auto [_it, inserted] = m_touchHandlerByNodeHandle.emplace(
      node->getArkUINodeHandle(), touchEventHandler);
  if (!inserted) {
    DLOG(WARNING) << "Touch handler for node handle: "
                 << node->getArkUINodeHandle() << " was already registered";
  }
}

void ArkUINodeRegistry::unregisterTouchHandler(ArkUINode* node) {
  DLOG(INFO) << "Unregistering touch handler for node handle "
             << node->getArkUINodeHandle();
  auto it = m_touchHandlerByNodeHandle.find(node->getArkUINodeHandle());
  if (it == m_touchHandlerByNodeHandle.end()) {
    DLOG(WARNING) << "Touch handler for node handle: "
                 << node->getArkUINodeHandle() << " not found";
    return;
  }
  m_touchHandlerByNodeHandle.erase(it);
}

ArkUINodeRegistry::ArkUINodeRegistry(ArkTSBridge::Shared arkTSBridge)
    : m_arkTSBridge(std::move(arkTSBridge)) {
  NativeNodeApi::getInstance()->registerNodeEventReceiver(
      [](ArkUI_NodeEvent* event) {
        ArkUINodeRegistry::getInstance().receiveEvent(event);
      });
}

void ArkUINodeRegistry::receiveEvent(ArkUI_NodeEvent* event) {
#ifdef C_API_ARCH
  try {
    auto eventType = OH_ArkUI_NodeEvent_GetEventType(event);
    auto node = OH_ArkUI_NodeEvent_GetNodeHandle(event);

    if (eventType == ArkUI_NodeEventType::NODE_TOUCH_EVENT) {
      auto it = m_touchHandlerByNodeHandle.find(node);
      if (it == m_touchHandlerByNodeHandle.end()) {
        DLOG(WARNING) << "Touch event for node with handle: " << node
                     << " not found";
        return;
      }

      auto inputEvent = OH_ArkUI_NodeEvent_GetInputEvent(event);
      if (inputEvent == nullptr ||
          OH_ArkUI_UIInputEvent_GetType(inputEvent) !=
              ArkUI_UIInputEvent_Type::ARKUI_UIINPUTEVENT_TYPE_TOUCH) {
        return;
      }

      it->second->onTouchEvent(inputEvent);
      return;
    }

    auto it = m_nodeByHandle.find(node);
    if (it == m_nodeByHandle.end()) {
      DLOG(WARNING) << "Node with handle: " << node << " not found";
      return;
    }

    auto componentEvent = OH_ArkUI_NodeEvent_GetNodeComponentEvent(event);
    if (componentEvent != nullptr) {
      it->second->onNodeEvent(eventType, componentEvent->data);
      return;
    }
    auto eventString = OH_ArkUI_NodeEvent_GetStringAsyncEvent(event);
    if (eventString != nullptr) {
      it->second->onNodeEvent(eventType, std::string_view(eventString->pStr));
      return;
    }

  } catch (std::exception& e) {
    m_arkTSBridge->handleError(std::current_exception());
  }
#endif
}

} // namespace rnoh