/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "DiffClampAnimatedNode.h"

namespace rnoh {

DiffClampAnimatedNode::DiffClampAnimatedNode(
    folly::dynamic const& config,
    AnimatedNodesManager& nodesManager)
    : m_nodesManager(nodesManager) {
  m_inputNodeTag = config["input"].asDouble();
  m_min = config["min"].asDouble();
  m_max = config["max"].asDouble();
}

void DiffClampAnimatedNode::update() {
  auto& inputNode = m_nodesManager.getValueNodeByTag(m_inputNodeTag);
  double inputValue = inputNode.getOutputAsDouble();
  double diff = inputValue - m_lastInputValue;
  m_lastInputValue = inputValue;
  this->setValue(
      std::min(std::max(this->getValueAsDouble() + diff, m_min), m_max));
}

} // namespace rnoh
