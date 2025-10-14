/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <folly/dynamic.h>

#include "RNOHCorePackage/TurboModules/Animated/AnimatedNodesManager.h"
#include "RNOHCorePackage/TurboModules/Animated/Nodes/ValueAnimatedNode.h"

namespace rnoh {

class AnimatedNodesManager;

class AnimationDriver {
 public:
  using AnimationEndCallback = std::function<void(bool)>;

  AnimationDriver(
      facebook::react::Tag animationId,
      facebook::react::Tag animatedNode,
      AnimatedNodesManager& nodesManager,
      AnimationEndCallback&& endCallback);
  virtual ~AnimationDriver() {
    if (endCallback_) {
      endCallback_(false);
    }
  };

  virtual void runAnimationStep(long long frameTimeNanos) = 0;
  virtual void resetConfig(folly::dynamic const& config);

  facebook::react::Tag getId() const;
  ValueAnimatedNode& getAnimatedValue();
  facebook::react::Tag getAnimatedValueTag() const;
  bool hasFinished() const;

  AnimationEndCallback endCallback_;

 protected:
  facebook::react::Tag m_animationId;
  facebook::react::Tag m_animatedNodeTag;
  AnimatedNodesManager& m_nodesManager;
  bool m_hasFinished = false;
};

} // namespace rnoh
