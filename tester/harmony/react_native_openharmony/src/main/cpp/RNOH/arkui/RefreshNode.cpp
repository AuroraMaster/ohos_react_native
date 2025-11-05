/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RefreshNode.h"

static constexpr ArkUI_NodeEventType REFRESH_NODE_EVENT_TYPES[] = {
    NODE_REFRESH_ON_REFRESH,
    NODE_REFRESH_STATE_CHANGE,
    NODE_REFRESH_ON_OFFSET_CHANGE};

namespace rnoh {
RefreshNode::RefreshNode(const ArkUINode::Context::Shared& context)
    : ArkUINode(context, ArkUI_NodeType::ARKUI_NODE_REFRESH) {
  for (auto eventType : REFRESH_NODE_EVENT_TYPES) {
    registerNodeEvent(eventType);
  }
}

RefreshNode::~RefreshNode() {
  for (auto eventType : REFRESH_NODE_EVENT_TYPES) {
    unregisterNodeEvent(eventType);
  }
}

RefreshNode& RefreshNode::setRefreshContent(ArkUINode& refreshContent) {
  ArkUI_AttributeItem loadingItem = {
      .object = refreshContent.getArkUINodeHandle()};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_REFRESH_CONTENT, &loadingItem);
  return *this;
}

RefreshNode& RefreshNode::setRefreshNodeDelegate(
    RefreshNodeDelegate* refreshNodeDelegate) {
  m_refreshNodeDelegate = refreshNodeDelegate;
  return *this;
}

void RefreshNode::insertChild(ArkUINode& child, std::size_t index) {
  m_nodeApi->addChild(m_nodeHandle, child.getArkUINodeHandle());
}

void RefreshNode::removeChild(ArkUINode& child) {
  maybeThrow(NativeNodeApi::getInstance()->removeChild(
      m_nodeHandle, child.getArkUINodeHandle()));
}

RefreshNode& RefreshNode::setNativeRefreshing(bool isRefreshing) {
  ArkUI_NumberValue refreshingValue[] = {{.u32 = isRefreshing}};
  ArkUI_AttributeItem refreshingItem = {
      refreshingValue, sizeof(refreshingValue) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_REFRESH_REFRESHING, &refreshingItem);
  return *this;
}

 RefreshNode& RefreshNode::setRefreshPullDownRation(float pullDownRatio) {
   ArkUI_NumberValue pullDownRatioValue[] = {{.f32 = pullDownRatio}};
   ArkUI_AttributeItem pullDownRatioItem = {
       pullDownRatioValue, sizeof(pullDownRatioValue) /
       sizeof(ArkUI_NumberValue)};
   m_nodeApi->setAttribute(
       m_nodeHandle, NODE_REFRESH_PULL_DOWN_RATIO, &pullDownRatioItem);
   return *this;
 }

void RefreshNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  ArkUINode::onNodeEvent(eventType, eventArgs);
  if (m_refreshNodeDelegate == nullptr) {
    return;
  }
  switch (eventType) {
    case ArkUI_NodeEventType::NODE_REFRESH_ON_REFRESH: {
      m_refreshNodeDelegate->onRefresh();
      break;
    }
    case ArkUI_NodeEventType::NODE_REFRESH_STATE_CHANGE: {
      auto state =
          static_cast<RefreshNodeDelegate::RefreshStatus>(eventArgs[0].i32);
      m_refreshNodeDelegate->onRefreshStateChanged(state);
      break;
    }
    case ArkUI_NodeEventType::NODE_REFRESH_ON_OFFSET_CHANGE: {
      m_refreshNodeDelegate->onRefreshNodeOffsetChange(this, eventArgs[0].f32);
      break;
    }
    default: {
    }
  }
}
} // namespace rnoh
