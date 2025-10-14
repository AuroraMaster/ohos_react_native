/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <optional>

#include "AnimatedNode.h"
#include "RNOHCorePackage/TurboModules/Animated/AnimatedNodesManager.h"

namespace rnoh {

class DiffClampAnimatedNode : public ValueAnimatedNode {
 public:
  DiffClampAnimatedNode(
      folly::dynamic const& config,
      AnimatedNodesManager& nodesManager);
  virtual ~DiffClampAnimatedNode() = default;

  void update() override;

 private:
  double m_min;
  double m_max;
  double m_lastInputValue = 0.0;
  size_t m_inputNodeTag;

  AnimatedNodesManager& m_nodesManager;
};

} // namespace rnoh
