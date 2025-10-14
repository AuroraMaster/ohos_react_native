/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "AnimationDriver.h"

#include <folly/dynamic.h>

namespace rnoh {

class FrameBasedAnimationDriver : public AnimationDriver {
 public:
  FrameBasedAnimationDriver(
      facebook::react::Tag animationId,
      facebook::react::Tag animatedNode,
      AnimatedNodesManager& nodesManager,
      folly::dynamic const& config,
      AnimationEndCallback&& endCallback);

  void resetConfig(folly::dynamic const& config) override;

  void runAnimationStep(long long frameTimeNanos) override;

 private:
  long long m_startTimeNanos;
  std::vector<double> m_frames;
  std::vector<double> m_speeds;
  double m_toValue;
  double m_fromValue;
  int64_t m_iterations;
  uint64_t m_currentLoop;
};

} // namespace rnoh