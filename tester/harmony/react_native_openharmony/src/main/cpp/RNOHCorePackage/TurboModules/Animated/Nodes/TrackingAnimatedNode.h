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

class TrackingAnimatedNode : public AnimatedNode {
 public:
  TrackingAnimatedNode(
      folly::dynamic const& config,
      AnimatedNodesManager& nodesManager);
  virtual ~TrackingAnimatedNode() = default;

  void update() override;
  void onDetachedFromNode(facebook::react::Tag tag) override;

 private:
  size_t m_animationId;
  size_t m_valueNodeTag;
  size_t m_toValueNodeTag;
  folly::dynamic m_animationConfig;

  AnimatedNodesManager& m_nodesManager;
};

} // namespace rnoh
