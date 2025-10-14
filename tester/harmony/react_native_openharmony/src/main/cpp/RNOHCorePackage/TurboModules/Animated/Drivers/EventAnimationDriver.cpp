/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "EventAnimationDriver.h"

namespace rnoh {

EventAnimationDriver::EventAnimationDriver(
    std::string const& eventName,
    facebook::react::Tag viewTag,
    EventPath&& eventPath,
    facebook::react::Tag nodeTag,
    AnimatedNodesManager& nodesManager)
    : m_eventName(eventName),
      m_viewTag(viewTag),
      m_eventPath(std::move(eventPath)),
      m_nodeTag(nodeTag),
      m_nodesManager(nodesManager) {}

void EventAnimationDriver::updateWithEvent(folly::dynamic const& event) {
  auto currentEvent = &event;
  for (auto& key : m_eventPath) {
    currentEvent = &(*currentEvent)[key];
  }

  auto& valueNode = getValueNode();
  valueNode.setValue(currentEvent->asDouble());
  m_nodesManager.setNeedsUpdate(m_nodeTag);
  return;
}

ValueAnimatedNode& EventAnimationDriver::getValueNode() const {
  return m_nodesManager.getValueNodeByTag(m_nodeTag);
}

} // namespace rnoh