/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "InterpolationAnimatedNode.h"
#include <regex>
#include "RNOH/Assert.h"
#include "RNOH/Color.h"
#include "glog/logging.h"

using namespace facebook;

namespace rnoh {

size_t findRangeIndex(double value, folly::dynamic const& inputRange) {
  size_t index = 1;
  while (index < inputRange.size() - 1 &&
         inputRange[index].asDouble() < value) {
    index++;
  }
  return index - 1;
}

std::vector<double> parseNumericValues(const std::string& str) {
  std::vector<double> values;
  std::regex regex("([-+]?[0-9]*\\.?[0-9]+)");
  auto begin = std::sregex_iterator(str.begin(), str.end(), regex);
  auto end = std::sregex_iterator();

  for (auto it = begin; it != end; ++it) {
    auto match = *it;
    values.push_back(std::stod(match.str()));
  }

  return values;
}

std::string interpolateString(
    const std::string& start,
    const std::string& end,
    double ratio) {
  auto startValues = parseNumericValues(start);
  auto endValues = parseNumericValues(end);

  std::string result = start;
  std::regex regex("([-+]?[0-9]*\\.?[0-9]+)");
  auto begin = std::sregex_iterator(start.begin(), start.end(), regex);
  auto endIter = std::sregex_iterator();

  int index = 0;
  for (std::sregex_iterator it = begin; it != endIter; ++it) {
    std::smatch match = *it;
    double interpolatedValue =
        startValues[index] + ratio * (endValues[index] - startValues[index]);
    result.replace(
        match.position(), match.length(), std::to_string(interpolatedValue));
    ++index;
  }

  return result;
}

InterpolationAnimatedNode::InterpolationAnimatedNode(
    folly::dynamic const& config,
    AnimatedNodesManager& nodesManager)
    : m_nodesManager(nodesManager) {
  m_inputRange = config["inputRange"];
  m_outputRange = config["outputRange"];
  m_extrapolateLeft =
      extrapolateTypeFromString(config["extrapolateLeft"].asString());
  m_extrapolateRight =
      extrapolateTypeFromString(config["extrapolateRight"].asString());
  m_outputType = OutputType::Unknown;
  if (!config["outputType"].empty()) {
    auto outputType = config["outputType"].asString();
    if (outputType == "number") {
      m_outputType = OutputType::Number;
    } else if (outputType == "color") {
      m_outputType = OutputType::Color;
    } else if (outputType == "string") {
      m_outputType = OutputType::String;
    }
  }
  /**
   * Code on JS side responsible for detecting outputType is buggy.
   */
  if (m_outputType == OutputType::Unknown) {
    if (m_outputRange[0].isString()) {
      m_outputType = OutputType::String;
    } else if (m_outputRange[0].isNumber()) {
      m_outputType = OutputType::Number;
    }
  }
}

void InterpolationAnimatedNode::update() {
  if (m_parent == std::nullopt) {
    // this can occur if the graph is still being constructed
    // when an update fires - just ignore it
    return;
  }

  auto& parentNode = getParentNode();
  double value = parentNode.getOutputAsDouble();

  auto rangeIndex = findRangeIndex(value, m_inputRange);

  switch (m_outputType) {
    case OutputType::Number:
      this->setValue(interpolate(
          value,
          m_inputRange[rangeIndex].asDouble(),
          m_inputRange[rangeIndex + 1].asDouble(),
          m_outputRange[rangeIndex].asDouble(),
          m_outputRange[rangeIndex + 1].asDouble(),
          m_extrapolateLeft,
          m_extrapolateRight));
      break;
    case OutputType::String: {
      double rangeStart = m_inputRange[rangeIndex].asDouble();
      double rangeEnd = m_inputRange[rangeIndex + 1].asDouble();
      double ratio = (value - rangeStart) / (rangeEnd - rangeStart);
      auto startString = m_outputRange[rangeIndex].asString();
      auto endString = m_outputRange[rangeIndex + 1].asString();
      this->setValue(interpolateString(startString, endString, ratio));
      break;
    }
    case OutputType::Color: {
      auto colorA = Color::from(m_outputRange[rangeIndex].asInt());
      auto colorB = Color::from(m_outputRange[rangeIndex + 1].asInt());
      auto mixValue = (value - m_inputRange[rangeIndex].asDouble()) /
          (m_inputRange[rangeIndex + 1].asDouble() -
           m_inputRange[rangeIndex].asDouble());
      auto clampedMixValue = std::max(std::min(mixValue, 1.0), 0.0);
      auto newColor = colorA * (1 - clampedMixValue) + colorB * clampedMixValue;
      this->setValue(newColor.asColorValue());
      break;
    }
    default:
      DLOG(WARNING) << "Unknown output type";
  }
}

void InterpolationAnimatedNode::onAttachedToNode(facebook::react::Tag tag) {
  m_nodesManager.getValueNodeByTag(tag);
  m_parent = tag;
}

void InterpolationAnimatedNode::onDetachedFromNode(facebook::react::Tag tag) {
  if (tag != m_parent) {
    throw std::runtime_error(
        "Trying to detach animated node from node " + std::to_string(tag) +
        " which is not its parent");
  }
  m_parent = std::nullopt;
}

double InterpolationAnimatedNode::interpolate(
    double value,
    double inputMin,
    double inputMax,
    double outputMin,
    double outputMax,
    ExtrapolateType extrapolateLeft,
    ExtrapolateType extrapolateRight) const {
  double result = value;

  if (result < inputMin) {
    if (extrapolateLeft == ExtrapolateType::EXTRAPOLATE_TYPE_IDENTITY) {
      return result;
    } else if (extrapolateLeft == ExtrapolateType::EXTRAPOLATE_TYPE_CLAMP) {
      result = inputMin;
    } else if (extrapolateLeft == ExtrapolateType::EXTRAPOLATE_TYPE_EXTEND) {
      // noop
    } else {
      // this should never happen, since we've already checked the extrapolation
      // type
      throw std::runtime_error(
          "Invalid extrapolation type for left extrapolation");
    }
  }

  if (result > inputMax) {
    if (extrapolateRight == ExtrapolateType::EXTRAPOLATE_TYPE_IDENTITY) {
      return result;
    } else if (extrapolateRight == ExtrapolateType::EXTRAPOLATE_TYPE_CLAMP) {
      result = inputMax;
    } else if (extrapolateRight == ExtrapolateType::EXTRAPOLATE_TYPE_EXTEND) {
      // noop
    } else {
      // this should never happen
      throw std::runtime_error(
          "Invalid extrapolation type for right extrapolation");
    }
  }

  if (outputMin == outputMax) {
    return outputMin;
  }

  if (inputMin == inputMax) {
    if (value >= inputMax) {
      return outputMax;
    } else {
      return outputMin;
    }
  }

  double inputRange = inputMax - inputMin;
  double outputRange = outputMax - outputMin;

  return outputMin + outputRange * (result - inputMin) / inputRange;
}

ValueAnimatedNode& InterpolationAnimatedNode::getParentNode() const {
  if (m_parent == std::nullopt) {
    throw std::runtime_error("Parent animated node has not been set");
  }
  return m_nodesManager.getValueNodeByTag(m_parent.value());
}

InterpolationAnimatedNode::ExtrapolateType
InterpolationAnimatedNode::extrapolateTypeFromString(
    std::string const& extrapolateType) {
  if (extrapolateType == "identity") {
    return ExtrapolateType::EXTRAPOLATE_TYPE_IDENTITY;
  } else if (extrapolateType == "clamp") {
    return ExtrapolateType::EXTRAPOLATE_TYPE_CLAMP;
  } else if (extrapolateType == "extend") {
    return ExtrapolateType::EXTRAPOLATE_TYPE_EXTEND;
  } else {
    throw std::runtime_error(
        "Invalid extrapolation type " + extrapolateType + " provided.");
  }
}

} // namespace rnoh
