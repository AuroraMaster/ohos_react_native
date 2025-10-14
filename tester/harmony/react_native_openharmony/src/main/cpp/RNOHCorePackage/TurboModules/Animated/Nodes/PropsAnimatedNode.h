/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "AnimatedNode.h"
#include "RNOHCorePackage/TurboModules/Animated/AnimatedNodesManager.h"
#include "StyleAnimatedNode.h"

#include <folly/dynamic.h>
#include <glog/logging.h>
#include <unordered_map>

namespace rnoh {

class PropsAnimatedNode : public AnimatedNode {
 public:
  PropsAnimatedNode(
      folly::dynamic const& config,
      AnimatedNodesManager& nodesManager)
      : m_nodesManager(nodesManager) {
    for (auto const& entry : config["props"].items()) {
      m_tagByPropName[entry.first.asString()] = entry.second.asDouble();
    }
  }

  void connectToView(facebook::react::Tag viewTag) {
    m_viewTag = viewTag;
  }

  void disconnectFromView(facebook::react::Tag viewTag) {
    if (m_viewTag != viewTag) {
      throw std::runtime_error(
          "Attempting to disconnect view that has not been connected with the given animated node");
    }
    m_viewTag = std::nullopt;
  }

  std::optional<PropUpdate> updateView() {
    if (m_viewTag == std::nullopt) {
      DLOG(WARNING)
          << "PropsAnimatedNode::updateView() called on unconnected node";
      return std::nullopt;
    }
    folly::dynamic props = folly::dynamic::object;
    for (auto& [key, nodeTag] : m_tagByPropName) {
      auto node = &m_nodesManager.getNodeByTag(nodeTag);
      if (auto styleNode = dynamic_cast<StyleAnimatedNode*>(node);
          styleNode != nullptr) {
        props.update(styleNode->getStyle());
      } else if (auto valueNode = dynamic_cast<ValueAnimatedNode*>(node);
                 valueNode != nullptr) {
        props[key] = valueNode->getOutput();
      } else {
        throw std::runtime_error("Unsupported property animated node type");
      }
    }
    return std::make_pair(m_viewTag.value(), props);
  }

 private:
  std::optional<facebook::react::Tag> m_viewTag;
  std::unordered_map<std::string, facebook::react::Tag> m_tagByPropName;
  AnimatedNodesManager& m_nodesManager;
};

} // namespace rnoh