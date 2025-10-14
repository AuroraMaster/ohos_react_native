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
#include "ValueAnimatedNode.h"

namespace rnoh {

class InterpolationAnimatedNode : public ValueAnimatedNode {
 public:
  InterpolationAnimatedNode(
      folly::dynamic const& config,
      AnimatedNodesManager& nodesManager);
  virtual ~InterpolationAnimatedNode() = default;

  void update() override;
  void onAttachedToNode(facebook::react::Tag tag) override;
  void onDetachedFromNode(facebook::react::Tag tag) override;

 private:
  enum ExtrapolateType {
    EXTRAPOLATE_TYPE_IDENTITY,
    EXTRAPOLATE_TYPE_CLAMP,
    EXTRAPOLATE_TYPE_EXTEND
  };

  enum OutputType {
    Number,
    Color,
    String,
    Unknown,
  };

  static ExtrapolateType extrapolateTypeFromString(
      std::string const& extrapolateType);

  ExtrapolateType m_extrapolateLeft;
  ExtrapolateType m_extrapolateRight;

  double interpolate(
      double value,
      double inputMin,
      double inputMax,
      double outputMin,
      double outputMax,
      ExtrapolateType extrapolateLeft,
      ExtrapolateType extrapolateRight) const;

  ValueAnimatedNode& getParentNode() const;

  folly::dynamic m_inputRange;
  folly::dynamic m_outputRange;
  OutputType m_outputType;
  std::optional<facebook::react::Tag> m_parent;
  AnimatedNodesManager& m_nodesManager;
};

} // namespace rnoh
