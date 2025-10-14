/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "AnimatedNode.h"
#include "RNOHCorePackage/TurboModules/Animated/AnimatedNodesManager.h"
#include "TransformAnimatedNode.h"

#include <folly/dynamic.h>

namespace rnoh {

class StyleAnimatedNode : public AnimatedNode {
 public:
  StyleAnimatedNode(
      folly::dynamic const& config,
      AnimatedNodesManager& nodesManager)
      : m_nodesManager(nodesManager) {
    for (auto const& entry : config["style"].items()) {
      m_tagByPropName[entry.first.asString()] = entry.second.asDouble();
    }
  }

  folly::dynamic getStyle() const {
    folly::dynamic style = folly::dynamic::object;
    for (auto& [key, nodeTag] : m_tagByPropName) {
      auto node = &m_nodesManager.getNodeByTag(nodeTag);
      if (auto valueNode = dynamic_cast<ValueAnimatedNode*>(node);
          valueNode != nullptr) {
        style[key] = valueNode->getOutput();
      } else if (auto transformNode =
                     dynamic_cast<TransformAnimatedNode*>(node);
                 transformNode != nullptr) {
        style[key] = transformNode->getTransform();
      } else {
        throw std::runtime_error("Unsupported style animated node type");
      }
    }
    return style;
  }

 private:
  std::unordered_map<std::string, facebook::react::Tag> m_tagByPropName;
  AnimatedNodesManager& m_nodesManager;
};

} // namespace rnoh