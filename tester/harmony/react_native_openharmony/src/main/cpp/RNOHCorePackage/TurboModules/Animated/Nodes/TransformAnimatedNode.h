/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <folly/dynamic.h>
#include <react/renderer/graphics/Transform.h>
#include <variant>

#include "AnimatedNode.h"
#include "RNOHCorePackage/TurboModules/Animated/AnimatedNodesManager.h"

namespace rnoh {

class TransformAnimatedNode : public AnimatedNode {
 public:
  TransformAnimatedNode(
      folly::dynamic const& config,
      AnimatedNodesManager& nodesManager);

  folly::dynamic getTransform() const;

 private:
  using NodeTag = facebook::react::Tag;

  struct StaticTransformConfig {
    std::string property;
    double value;
  };

  struct AnimatedTransformConfig {
    std::string property;
    NodeTag nodeTag;
  };

  using TransformConfig =
      std::variant<StaticTransformConfig, AnimatedTransformConfig>;

  std::vector<TransformConfig> m_transforms;
  AnimatedNodesManager& m_nodesManager;
};

} // namespace rnoh
