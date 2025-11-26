/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "FrameBasedAnimationDriver.h"

#include <glog/logging.h>

namespace rnoh {

// 60 FPS is hardcoded in React Native
double FRAME_TIME_MILLIS = 1000.0 / 60.0;

FrameBasedAnimationDriver::FrameBasedAnimationDriver(
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
  resetConfig(config);
}

void FrameBasedAnimationDriver::resetConfig(folly::dynamic const& config) {
  auto frames = config["frames"];
  auto numberOfFrames = frames.size();
  m_frames.resize(numberOfFrames);
  m_speeds.resize(numberOfFrames - 1);
  for (auto i = 0; i < numberOfFrames - 1; i++) {
    m_speeds[i] =
        (frames[i + 1].asDouble() - frames[i].asDouble()) / FRAME_TIME_MILLIS;
    m_frames[i] = frames[i].asDouble();
  }

  m_frames[numberOfFrames - 1] = frames[numberOfFrames - 1].asDouble();
  m_toValue = config.getDefault("toValue", 0.0).asDouble();
  m_iterations = config.getDefault("iterations", 1).asInt();
  m_currentLoop = 1;
  m_startTimeNanos = 0;
  m_hasFinished = m_iterations == 0;
}

void FrameBasedAnimationDriver::runAnimationStep(long long frameTimeNanos) {
  if (m_hasFinished) {
    return;
  }
  try {
    auto& animatedValue = getAnimatedValue();
    if (m_startTimeNanos == 0) {
      m_startTimeNanos = frameTimeNanos;
      if (m_currentLoop == 1) {
        m_fromValue = animatedValue.getValueAsDouble();
      }
    }
    auto timeFromStart = frameTimeNanos - m_startTimeNanos;
    double timeFromStartMillis = timeFromStart / 1e6;
    auto frameIndex =
        static_cast<uint64_t>((timeFromStartMillis) / FRAME_TIME_MILLIS);
    double nextValue = 0.0;
    double velocity = 0.0;
    if (frameIndex >= m_frames.size() - 1) {
      if (!m_speeds.empty()) {
        velocity = m_speeds.back() * 1e3 * abs(m_toValue - m_fromValue);
      }
      nextValue = m_toValue;
      if (m_iterations == -1 || m_currentLoop < m_iterations) {
        m_startTimeNanos = 0;
        m_currentLoop++;
      } else {
        m_hasFinished = true;
      }
    } else {
      double timeStartMillis = frameIndex * FRAME_TIME_MILLIS;
      double distanceStart = m_frames[frameIndex];

      if (!m_speeds.empty()) {
        velocity = m_speeds[frameIndex] * 1e3 * abs(m_toValue - m_fromValue);
      }

      nextValue = m_fromValue +
          (m_toValue - m_fromValue) *
              (distanceStart +
               (timeFromStartMillis - timeStartMillis) * m_speeds[frameIndex]);
    }

    animatedValue.setVelocity(velocity);
    animatedValue.setValue(nextValue);
  } catch (const AnimatedNodeNotFoundError& _e) {
    // if a node is not found we skip over it and proceed with the
    // animation to maintain consistency with other platforms
    m_hasFinished = true;
  }
}

} // namespace rnoh
