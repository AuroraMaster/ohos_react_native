/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "CustomNode.h"

#include <glog/logging.h>
#include <memory>
#include "NativeNodeApi.h"

namespace rnoh {
CustomNode::CustomNode(const ArkUINode::Context::Shared& context)
    : ArkUINode(context, ArkUI_NodeType::ARKUI_NODE_CUSTOM),
      m_customNodeDelegate(nullptr) {
  userCallback_ = new UserCallback();

  userCallback_->callback = [this](ArkUI_NodeCustomEvent *event) {
    auto type = OH_ArkUI_NodeCustomEvent_GetEventType(event);
      switch (type) {
        case ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE:
          onMeasure(type);
          break;
        case ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT:
          onLayout();
        default:
          break;
        }
    };
  eventReceiver = [](ArkUI_NodeCustomEvent *event) {
    auto *userData = reinterpret_cast<UserCallback *>(OH_ArkUI_NodeCustomEvent_GetUserData(event));
    int32_t tagId = OH_ArkUI_NodeCustomEvent_GetEventTargetId(event);
    if (userData && (tagId == 89 || tagId == 90)) {
      userData->callback(event);
    }
  };
  maybeThrow(NativeNodeApi::getInstance()->addNodeCustomEventReceiver(
      m_nodeHandle, eventReceiver));

  maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(
      m_nodeHandle, NODE_ON_CLICK, 0, this));
  maybeThrow(NativeNodeApi::getInstance()->registerNodeEvent(
      m_nodeHandle, NODE_ON_HOVER, 0, this));
  maybeThrow(NativeNodeApi::getInstance()->registerNodeCustomEvent(
      m_nodeHandle, ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE, 89, userCallback_));
  maybeThrow(NativeNodeApi::getInstance()->registerNodeCustomEvent(
      m_nodeHandle, ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT, 90, userCallback_));
  /**
   * This is for 2in1 CustomNode focusing problem, focusing would 
   * raise the component and setting ZIndex as 2^31-1, which would
   * setting it at the top to display. 
   * Setting z-index ahead would preventing focusing drawing to 
   * set the z-index again. 
   */
  ArkUI_NumberValue zIndexValue[] = {{.i32 = 0}};
  ArkUI_AttributeItem zIndexItem = {.value = zIndexValue, .size = 1};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_Z_INDEX, &zIndexItem);
}

void CustomNode::onMeasure(ArkUI_NodeCustomEventType eventType) {
    // auto rect = NativeNodeApi::getInstance()->getAttribute(m_nodeHandle, NODE_LAYOUT_RECT);
    // if (!rect) {
    //     LOG(ERROR)<<"Custom Node Get layout Rect para error";
    //     return;
    // }
    // int32_t width = rect->value[2].i32;
    // int32_t height = rect->value[3].i32;
    int32_t width = getSavedWidth();
    int32_t height = getSavedHeight();
    m_nodeApi->setMeasuredSize(m_nodeHandle, width, height);
}

void CustomNode::onLayout() {}

void CustomNode::insertChild(ArkUINode& child, std::size_t index) {
  m_nodeApi->insertChildAt(
      m_nodeHandle, child.getArkUINodeHandle(), static_cast<int32_t>(index));
}

void CustomNode::addChild(ArkUINode& child) {
  m_nodeApi->addChild(m_nodeHandle, child.getArkUINodeHandle());
}
void CustomNode::removeChild(ArkUINode& child) {
  maybeThrow(NativeNodeApi::getInstance()->removeChild(
      m_nodeHandle, child.getArkUINodeHandle()));
}

void CustomNode::setCustomNodeDelegate(CustomNodeDelegate* customNodeDelegate) {
  m_customNodeDelegate = customNodeDelegate;
}

void CustomNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  ArkUINode::onNodeEvent(eventType, eventArgs);
  if (eventType == ArkUI_NodeEventType::NODE_ON_CLICK &&
      eventArgs[3].i32 != UI_INPUT_EVENT_SOURCE_TYPE_TOUCH_SCREEN &&
      eventArgs[3].i32 != UI_INPUT_EVENT_SOURCE_TYPE_MOUSE) {
    onClick();
  }
  if (eventType == ArkUI_NodeEventType::NODE_ON_HOVER) {
    if (m_customNodeDelegate != nullptr) {
      if (eventArgs[0].i32) {
        m_customNodeDelegate->onHoverIn();
      } else {
        m_customNodeDelegate->onHoverOut();
      }
    }
  }
}

void CustomNode::onClick() {
  if (m_customNodeDelegate != nullptr) {
    m_customNodeDelegate->onClick();
  }
}

CustomNode::~CustomNode() {
  NativeNodeApi::getInstance()->unregisterNodeEvent(
      m_nodeHandle, NODE_ON_CLICK);
  NativeNodeApi::getInstance()->unregisterNodeEvent(
      m_nodeHandle, NODE_ON_HOVER);
  NativeNodeApi::getInstance()->unregisterNodeCustomEvent(
      m_nodeHandle, ARKUI_NODE_CUSTOM_EVENT_ON_MEASURE);
  NativeNodeApi::getInstance()->unregisterNodeCustomEvent(
      m_nodeHandle, ARKUI_NODE_CUSTOM_EVENT_ON_LAYOUT);
  NativeNodeApi::getInstance()->removeNodeCustomEventReceiver(m_nodeHandle, eventReceiver);
  delete userCallback_;
}

CustomNode& CustomNode::setAlign(int32_t align) {
  ArkUI_NumberValue value[] = {{.i32 = align}};
  ArkUI_AttributeItem item = {.value = value, .size = 1};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_STACK_ALIGN_CONTENT, &item);
  return *this;
}

CustomNode& CustomNode::setFocusable(bool focusable) {
  int32_t focusableValue = focusable;
  ArkUI_NumberValue preparedFocusable[] = {{.i32 = focusableValue}};
  ArkUI_AttributeItem focusItem = {
      preparedFocusable, sizeof(preparedFocusable) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_FOCUSABLE, &focusItem);
  return *this;
}
} // namespace rnoh
