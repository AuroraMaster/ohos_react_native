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
#include "ArkUINodeRegistry.h"
#include "TouchEventDispatcher.h"
#include "RNOH/ComponentInstance.h"
#include "RNOH/CppComponentInstance.h"
#include "RNOH/ApiVersionCheck.h"
#include <chrono>

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
  
  #ifdef ALL_CONTAINERS_CLICKABLE
  registerNodeEvent(NODE_ON_CLICK);
  #endif
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
    int32_t width = getSavedWidth();
    int32_t height = getSavedHeight();
    m_nodeApi->setMeasuredSize(m_nodeHandle, width, height);
}

void CustomNode::onLayout() {}

void CustomNode::insertChild(ArkUINode& child, std::size_t index) {
  m_nodeApi->insertChildAt(
      m_nodeHandle, child.getArkUINodeHandle(), static_cast<int32_t>(index));
  startDebounceTimer();
}

void CustomNode::addChild(ArkUINode& child) {
  m_nodeApi->addChild(m_nodeHandle, child.getArkUINodeHandle());
}

void CustomNode::removeChild(ArkUINode& child) {
  if (child.isFocused()) {
    child.setFocusStatus(0);
  }
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

  // Handle custom overflow scroll event
  if (eventType == static_cast<ArkUI_NodeEventType>(CUSTOM_OVERFLOW_SCROLL_EVENT)) {
    int32_t currentSourceId = eventArgs[0].i32;
    float offset = eventArgs[1].f32;

    if (m_activeScrollChildId == -1) {
      m_activeScrollChildId = currentSourceId;
    }

    if (currentSourceId != m_activeScrollChildId) {
      return; 
    }

    auto* delegate = this->getArkUINodeDelegate();
    if (delegate != nullptr) {
      auto* componentInstance = dynamic_cast<rnoh::ComponentInstance*>(delegate);
      if (componentInstance != nullptr) {
        facebook::react::Tag rnTag = componentInstance->getTag();
        LOG(ERROR) << "CustomNode Scroll Tag: " << rnTag << " SourceId: " << currentSourceId << " Offset: " << offset;
        m_customNodeDelegate->onScroll(rnTag, offset);
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
  stopDebounceTimer();
  unregisterNodeEvent(NODE_ON_CLICK);
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
  #ifndef ALL_CONTAINERS_CLICKABLE
  if (focusable) {
    registerNodeEvent(NODE_ON_CLICK);
  } else {
    unregisterNodeEvent(NODE_ON_CLICK);
  }
  #endif
  int32_t focusableValue = focusable;
  ArkUI_NumberValue preparedFocusable[] = {{.i32 = focusableValue}};
  ArkUI_AttributeItem focusItem = {
      preparedFocusable, sizeof(preparedFocusable) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_FOCUSABLE, &focusItem);
  return *this;
}

void CustomNode::startDebounceTimer() {
  // Cancel any existing timer
  m_timerCancelled = true;
  m_timerCV.notify_all();
  
  // Wait for previous thread to finish
  if (m_debounceThread.joinable()) {
    m_debounceThread.join();
  }
  
  // Start new timer
  m_timerCancelled = false;
  m_timerRunning = true;
  m_debounceThread = std::thread(&CustomNode::debounceTimerFunc, this);
}

void CustomNode::stopDebounceTimer() {
  m_timerCancelled = true;
  m_timerRunning = false;
  m_timerCV.notify_all();
  
  if (m_debounceThread.joinable()) {
    m_debounceThread.join();
  }
}

void CustomNode::debounceTimerFunc() {
  std::unique_lock<std::mutex> lock(m_timerMutex);
  
  // Wait for 100ms or until cancelled
  if (m_timerCV.wait_for(lock, std::chrono::milliseconds(100), 
      [this]() { return m_timerCancelled.load(); })) {
    // Timer was cancelled (new insertChild called or object being destroyed)
    return;
  }
  
  // Timer completed without being cancelled - this is the last insertChild
  if (m_timerRunning && !m_timerCancelled) {
     NativeNodeApi::getInstance()->markDirty(getArkUINodeHandle(), ArkUI_NodeDirtyFlag::NODE_NEED_MEASURE);
  }
  
  m_timerRunning = false;
}
} // namespace rnoh