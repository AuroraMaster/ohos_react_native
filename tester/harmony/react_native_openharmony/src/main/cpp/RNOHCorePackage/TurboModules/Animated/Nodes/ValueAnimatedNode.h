/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "AnimatedNode.h"

#include <folly/dynamic.h>
#include "RNOH/Assert.h"

namespace rnoh {

class ValueAnimatedNode : public AnimatedNode {
 public:
  using AnimatedNodeValueListener = std::function<void(double)>;
  using VirtualPixelsPerSecond = double;

  static constexpr int32_t FRAME_RATE_30 = 30.0;
  static constexpr int32_t FRAME_RATE_60 = 60.0;
  static constexpr double SPEED_THRESHOLD = 10.0;

  ValueAnimatedNode() {}

  ValueAnimatedNode(folly::dynamic const& config) {
    RNOH_ASSERT(config.count("value") > 0);
    m_value = config["value"];
    RNOH_ASSERT(config["offset"].isDouble());
    m_offset = config["offset"].getDouble();
  }

  folly::dynamic getOutput() override {
    if (m_value.isDouble()) {
      folly::dynamic result = this->getOutputAsDouble();
      return result;
    }
    return m_value;
  }

  double getOutputAsDouble() {
    return this->getValueAsDouble() + m_offset;
  }

  void setValue(folly::dynamic value) {
    m_value = value;
  }

  void setValue(double value) {
    m_value = value;
  }

  void setOffset(double offset) {
    m_offset = offset;
  }

  void setVelocity(VirtualPixelsPerSecond velocity) {
    m_velocity = velocity;
  }

  int32_t getFrameRate() const {
    return m_frameRate;
  }

  void recalculateMinAcceptableFrameRate(float scaleRatioDpi) {
    /**
     * Note:
     * - `m_velocity`: Velocity in virtual pixels units per second (vp/s).
     * - `25.4`: Converts inches to millimeters (1 inch = 25.4 mm).
     * - `scaleRatioDpi`: `1px = (25.4 / Dpi) mm`, where `Dpi` is the display's
     * dots per inch. `1vp = scale * px`, so `scaleRatioDpi` is `scale / Dpi`.
     *
     * This formula converts velocity from vp/s to mm/s, giving a real-world
     * measurement of animation speed.
     */
    double animationSpeed = m_velocity * 25.4 * scaleRatioDpi;
    m_frameRate =
        animationSpeed <= SPEED_THRESHOLD ? FRAME_RATE_30 : FRAME_RATE_60;
  }

  void flattenOffset() {
    m_value = this->getOutputAsDouble();
    m_offset = 0;
  }

  void extractOffset() {
    m_offset += this->getValueAsDouble();
    m_value = 0;
  }

  void onValueUpdate() {
    if (m_valueListener.has_value()) {
      /**
       * NOTE: ValueListenerCallback on JS side expects only numbers.
       */
      m_valueListener.value()(getOutputAsDouble());
    }
  }

  void setValueListener(AnimatedNodeValueListener&& listener) {
    if (m_valueListener.has_value()) {
      throw std::runtime_error("AnimatedNode already has a value listener");
    }
    m_valueListener = std::move(listener);
  }

  void removeValueListener() {
    m_valueListener = std::nullopt;
  }

  double getValueAsDouble() {
    if (m_value.isNull()) {
      // Special case for diffClamp
      return 0;
    }
    if (!m_value.isDouble()) {
      LOG(ERROR) << "Unexpected value type: " << m_value.typeName();
      RNOH_ASSERT(false);
      return 0;
    }
    return m_value.getDouble();
  }

 protected:
  folly::dynamic m_value;
  VirtualPixelsPerSecond m_velocity = 0.0;
  int32_t m_frameRate = 60;
  double m_offset = 0.0;
  std::optional<AnimatedNodeValueListener> m_valueListener;
};

} // namespace rnoh