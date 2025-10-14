/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ButtonNode.h"

static constexpr ArkUI_NodeEventType Button_NODE_EVENT_TYPES[] = {
    NODE_ON_CLICK};

namespace rnoh {
ButtonNode::ButtonNode()
    : ArkUINode(NativeNodeApi::getInstance()->createNode(
          ArkUI_NodeType::ARKUI_NODE_BUTTON)) {
  for (auto eventType : Button_NODE_EVENT_TYPES) {
    maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(
        m_nodeHandle, eventType, eventType, this));
  }
}

ButtonNode::~ButtonNode() {
  for (auto eventType : Button_NODE_EVENT_TYPES) {
    NativeNodeApi::getInstance()->unregisterNodeEvent(m_nodeHandle, eventType);
  }
}

ButtonNode& ButtonNode::setButtonNodeDelegate(
    ButtonNodeDelegate* buttonNodeDelegate) {
  m_buttonNodeDelegate = buttonNodeDelegate;
  return *this;
}

void ButtonNode::insertChild(ArkUINode& child, std::size_t index) {
  maybeThrow(NativeNodeApi::getInstance()->addChild(
      m_nodeHandle, child.getArkUINodeHandle()));
}

void ButtonNode::removeChild(ArkUINode& child) {
  maybeThrow(NativeNodeApi::getInstance()->removeChild(
      m_nodeHandle, child.getArkUINodeHandle()));
}

ButtonNode& ButtonNode::setLabel(const std::string &src) {
  ArkUI_AttributeItem labelItem = {.string = src.c_str()};
  maybeThrow(NativeNodeApi::getInstance()->setAttribute(
      m_nodeHandle, NODE_BUTTON_LABEL, &labelItem));
  return *this;
}

void ButtonNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  if (eventType == ArkUI_NodeEventType::NODE_ON_CLICK) {
    if (m_buttonNodeDelegate != nullptr) {
      m_buttonNodeDelegate->onButtonClick();
    }
  }
}
} // namespace rnoh