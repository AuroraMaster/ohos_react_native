/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ColumnNode.h"
#include "ArkUINode.h"

#include <glog/logging.h>
#include <memory>
#include "NativeNodeApi.h"

namespace rnoh {

ColumnNode::ColumnNode(const ArkUINode::Context::Shared& context)
    : ArkUINode(context, ArkUI_NodeType::ARKUI_NODE_COLUMN),
      m_columnNodeDelegate(nullptr) {
  #ifdef ALL_CONTAINERS_CLICKABLE
  registerNodeEvent(NODE_ON_CLICK);
  #endif
}

void ColumnNode::insertChild(ArkUINode& child, std::size_t index) {
  m_nodeApi->insertChildAt(
      m_nodeHandle, child.getArkUINodeHandle(), static_cast<int32_t>(index));
}

void ColumnNode::removeChild(ArkUINode& child) {
  maybeThrow(NativeNodeApi::getInstance()->removeChild(
      m_nodeHandle, child.getArkUINodeHandle()));
}

ArkUI_NodeHandle ColumnNode::getChildAt(std::size_t index) {
  return NativeNodeApi::getInstance()->getChildAt(
      m_nodeHandle, static_cast<int32_t>(index));
}

void ColumnNode::setColumnNodeDelegate(ColumnNodeDelegate* columnNodeDelegate) {
  m_columnNodeDelegate = columnNodeDelegate;
}

void ColumnNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  ArkUINode::onNodeEvent(eventType, eventArgs);
  if (eventType == ArkUI_NodeEventType::NODE_ON_CLICK &&
      eventArgs[3].i32 != UI_INPUT_EVENT_SOURCE_TYPE_TOUCH_SCREEN &&
      eventArgs[3].i32 != UI_INPUT_EVENT_SOURCE_TYPE_MOUSE) {
    onClick();
  }
}

void ColumnNode::onClick() {
  if (m_columnNodeDelegate != nullptr) {
    m_columnNodeDelegate->onClick();
  }
}

ColumnNode::~ColumnNode() {
  unregisterNodeEvent(NODE_ON_CLICK);
}
} // namespace rnoh