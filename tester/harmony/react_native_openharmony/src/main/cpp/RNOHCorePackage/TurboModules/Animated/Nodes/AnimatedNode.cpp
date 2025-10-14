/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "AnimatedNode.h"

using namespace facebook;

namespace rnoh {

void AnimatedNode::update() {}

folly::dynamic AnimatedNode::getOutput() {
  return nullptr;
}

void AnimatedNode::onAttachedToNode(facebook::react::Tag tag) {}

void AnimatedNode::onDetachedFromNode(facebook::react::Tag tag) {}

void AnimatedNode::addChild(AnimatedNode& child) {
  m_childrenTags.push_back(child.tag_);
  child.onAttachedToNode(tag_);
}

void AnimatedNode::removeChild(AnimatedNode& child) {
  child.onDetachedFromNode(tag_);
  m_childrenTags.erase(
      std::remove(m_childrenTags.begin(), m_childrenTags.end(), child.tag_),
      m_childrenTags.end());
}

std::vector<facebook::react::Tag> const& AnimatedNode::getChildrenTags() const {
  return m_childrenTags;
}
} // namespace rnoh