/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "QDGestureFloatComponentInstance.h"

namespace rnoh {

QDGestureFloatComponentInstance::QDGestureFloatComponentInstance(Context context) : Super(std::move(context)) {
  m_scrollNode.setScrollNodeDelegate(this);
  getLocalRootArkUINode().insertChild(m_scrollNode, 0);
  m_scrollNode.insertChild(m_scrollContentNode);
  m_scrollContentNode.insertChild(m_transparentNode, 0);
  m_scrollContentNode.insertChild(m_contentNode, 1);

  m_scrollNode.setScrollBarDisplayMode(ArkUI_ScrollBarDisplayMode::ARKUI_SCROLL_BAR_DISPLAY_MODE_OFF);
}

facebook::react::Point rnoh::QDGestureFloatComponentInstance::getCurrentOffset() const {
  auto offset = m_scrollNode.getScrollOffset();
  offset.y -= m_height;
  return offset;
}

StackNode &QDGestureFloatComponentInstance::getLocalRootArkUINode() { return m_scrollContainerNode; }

void QDGestureFloatComponentInstance::onPropsChanged(SharedConcreteProps const &props){
  if (props->stopPercent) {
    m_stopPercent = props->stopPercent;
  }
  if (props->stopPercentMax) {
    m_stopPercentMax = props->stopPercentMax;
  }
};

void QDGestureFloatComponentInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance,
                                                      std::size_t index) {
  CppComponentInstance::onChildInserted(childComponentInstance, index);
  m_contentNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
  if (!m_isInit) {
    auto layoutMetrics = getLayoutMetrics();
    m_width = layoutMetrics.frame.size.width;
    m_height = layoutMetrics.frame.size.height;
    m_transparentNode.setSize(facebook::react::Size{m_width, m_height});
    m_scrollNode.scrollTo(0, m_height * m_stopPercent, false);
  }
}

void QDGestureFloatComponentInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
  CppComponentInstance::onChildRemoved(childComponentInstance);
  m_contentNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}

void QDGestureFloatComponentInstance::onScroll() {
  auto offsetY = m_scrollNode.getScrollOffset().y;
  facebook::react::QDGestureFloatEventEmitter::OnScroll event = {offsetY};
  m_eventEmitter->onScroll(event);
}

void QDGestureFloatComponentInstance::onScrollStop() {
  auto y = m_scrollNode.getScrollOffset().y;
  if (y <= m_height && y > m_height * m_stopPercentMax) {
    m_scrollNode.scrollTo(0, m_height, true);
  } else if (y < m_height * m_stopPercent || (y <= m_height && y > m_height * m_stopPercent)) {
    m_scrollNode.scrollTo(0, m_height * m_stopPercent, true);
  }
}

void QDGestureFloatComponentInstance::onScrollToCommand(float offsetY, bool animated) {
  m_scrollNode.scrollTo(0, offsetY, animated);
}
} // namespace rnoh
