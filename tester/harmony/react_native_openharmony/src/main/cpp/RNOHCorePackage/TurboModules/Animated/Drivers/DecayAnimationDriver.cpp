/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "DecayAnimationDriver.h"

namespace rnoh {

DecayAnimationDriver::DecayAnimationDriver(
    facebook::react::Tag animationId,
    facebook::react::Tag animatedNode,
    AnimatedNodesManager& nodesManager,
    folly::dynamic const& config,
    AnimationEndCallback&& endCallback)
    : AnimationDriver(
          animationId,
          animatedNode,
          nodesManager,
          std::move(endCallback)) {
  m_velocity = config["velocity"].getDouble(); // initial velocity
  resetConfig(config);
}

void DecayAnimationDriver::resetConfig(folly::dynamic const& config) {
  m_deceleration = config["deceleration"].getDouble();
  m_iterations =
      config.count("iterations") ? config["iterations"].getDouble() : 1;
  m_currentLoop = 1;
  m_hasStarted = false;
  m_hasFinished = m_iterations == 0;
  m_fromValue = 0;
  m_lastValue = 0;
}

void DecayAnimationDriver::runAnimationStep(long long frameTimeNanos) {
  int64_t frameTimeMillis = frameTimeNanos / 1e6;
  try {
    auto& animatedValue = m_nodesManager.getValueNodeByTag(m_animatedNodeTag);

    if (!m_hasStarted) {
      // since this is the first frame of the animation,
      // we set the start time to the previous frame.
      // Since we don't know the actual framerate, we use 16ms as the frame
      // time.
      m_startTimeMillis = frameTimeMillis - 16;
      if (m_currentLoop ==
          1) { // first iteration, assign fromValue from the animatedValue
        m_fromValue = animatedValue.getValueAsDouble();
      } else { // otherwise, reset the animatedValue based on fromValue
        animatedValue.setValue(m_fromValue);
      }
      m_lastValue = m_fromValue;
      m_hasStarted = true;
    }

    auto timeFromStartMillis = frameTimeMillis - m_startTimeMillis;
    double value = m_fromValue +
        (m_velocity / (1 - m_deceleration)) *
            (1 - std::exp(-(1 - m_deceleration) * timeFromStartMillis));
    double velocity = m_velocity *
        std::exp(-(1 - m_deceleration) * timeFromStartMillis) * 1e3;

    if (std::abs(m_lastValue - value) < 0.1) {
      if (m_iterations == -1 || m_currentLoop < m_iterations) {
        // reset animation for the next loop
        m_hasStarted = false;
        m_currentLoop++;
      } else {
        m_hasFinished = true;
        return;
      }
    }

    m_lastValue = value;
    animatedValue.setValue(value);
    animatedValue.setVelocity(std::abs(velocity));
  } catch (std::out_of_range& _e) {
    // if a node is not found we skip over it and proceed with the
    // animation to maintain consistency with other platforms
    m_hasFinished = true;
  }
}
} // namespace rnoh
