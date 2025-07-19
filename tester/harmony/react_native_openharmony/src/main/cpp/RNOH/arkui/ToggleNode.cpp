/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ToggleNode.h"
#include "NativeNodeApi.h"
#include "glog/logging.h"
namespace rnoh {

ToggleNode::ToggleNode(Context context)
    : ArkUINode(context, ArkUI_NodeType::ARKUI_NODE_TOGGLE),
      m_childArkUINodeHandle(nullptr),
      m_toggleNodeDelegate(nullptr) {
  registerNodeEvent(NODE_TOGGLE_ON_CHANGE);
}

ToggleNode::~ToggleNode() {
  unregisterNodeEvent(NODE_TOGGLE_ON_CHANGE);
}

void ToggleNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  ArkUINode::onNodeEvent(eventType, eventArgs);
  if (eventType == ArkUI_NodeEventType::NODE_TOGGLE_ON_CHANGE) {
    auto value = eventArgs[0].i32;
    if (m_toggleNodeDelegate != nullptr) {
      m_toggleNodeDelegate->onValueChange(value);
    }
  }
}

void ToggleNode::setToggleNodeDelegate(ToggleNodeDelegate* toggleNodeDelegate) {
  m_toggleNodeDelegate = toggleNodeDelegate;
}

ToggleNode& ToggleNode::setSelectedColor(
    facebook::react::SharedColor const& color) {
  if (!color) {
    // restore default value
    maybeThrow(m_context.nodeApi.resetAttribute(
        m_nodeHandle, NODE_TOGGLE_SELECTED_COLOR));
    return *this;
  }
  uint32_t colorValue = *color;
  ArkUI_NumberValue preparedColorValue[] = {{.u32 = colorValue}};
  ArkUI_AttributeItem colorItem = {
      preparedColorValue,
      sizeof(preparedColorValue) / sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TOGGLE_SELECTED_COLOR, &colorItem));
  return *this;
}

ToggleNode& ToggleNode::setUnselectedColor(
    facebook::react::SharedColor const& color) {
  if (!color) {
    maybeThrow(m_context.nodeApi.resetAttribute(
        m_nodeHandle, NODE_TOGGLE_UNSELECTED_COLOR));
    return *this;
  }
  uint32_t colorValue = *color;
  ArkUI_NumberValue preparedColorValue[] = {{.u32 = colorValue}};
  ArkUI_AttributeItem colorItem = {
      preparedColorValue,
      sizeof(preparedColorValue) / sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TOGGLE_UNSELECTED_COLOR, &colorItem));
  return *this;
}

ToggleNode& ToggleNode::setThumbColor(
    facebook::react::SharedColor const& color) {
  uint32_t colorValue = *color;
  ArkUI_NumberValue preparedColorValue[] = {{.u32 = colorValue}};
  ArkUI_AttributeItem colorItem = {
      preparedColorValue,
      sizeof(preparedColorValue) / sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TOGGLE_SWITCH_POINT_COLOR, &colorItem));
  return *this;
}

ToggleNode& ToggleNode::setFocusable(bool focusable) {
  int32_t focusableValue = focusable;
  ArkUI_NumberValue preparedFocusable[] = {{.i32 = focusableValue}};
  ArkUI_AttributeItem focusItem = {
      preparedFocusable, sizeof(preparedFocusable) / sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_FOCUSABLE, &focusItem));
  return *this;
}

bool ToggleNode::getValue() {
  auto attribute = NativeNodeApi::getInstance()->getAttribute(
      m_nodeHandle, NODE_TOGGLE_VALUE);
  auto value = attribute->value[0].i32;
  return value != 0;
}

ToggleNode& ToggleNode::setValue(bool value) {
  int32_t valueValue = value;
  ArkUI_NumberValue preparedValue[] = {{.i32 = valueValue}};
  ArkUI_AttributeItem valueItem = {
      preparedValue, sizeof(preparedValue) / sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TOGGLE_VALUE, &valueItem));
  return *this;
}

} // namespace rnoh
