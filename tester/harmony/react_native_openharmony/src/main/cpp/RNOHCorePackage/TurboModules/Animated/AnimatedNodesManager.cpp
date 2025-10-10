/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "AnimatedNodesManager.h"

#include <glog/logging.h>
#include <functional>
#include <queue>
#include <thread>

#include "Nodes/AssociativeOperationNode.h"
#include "Nodes/DiffClampAnimatedNode.h"
#include "Nodes/InterpolationAnimatedNode.h"
#include "Nodes/ModulusAnimatedNode.h"
#include "Nodes/PropsAnimatedNode.h"
#include "Nodes/StyleAnimatedNode.h"
#include "Nodes/TrackingAnimatedNode.h"
#include "Nodes/TransformAnimatedNode.h"
#include "Nodes/ValueAnimatedNode.h"

#include "Drivers/DecayAnimationDriver.h"
#include "Drivers/EventAnimationDriver.h"
#include "Drivers/FrameBasedAnimationDriver.h"
#include "Drivers/SpringAnimationDriver.h"
#include "RNOH/ArkTSBridge.h"

using namespace facebook;

namespace rnoh {

AnimatedNodesManager::AnimatedNodesManager(
    const std::function<void(int)>& scheduleUpdateFn,
    const std::function<void()>& scheduleStartFn,
    const std::function<void()>& scheduleStopFn)
    : m_scheduleUpdateFn(scheduleUpdateFn),
      m_scheduleStartFn(scheduleStartFn),
      m_scheduleStopFn(scheduleStopFn) {}

void AnimatedNodesManager::createNode(
    facebook::react::Tag tag,
    folly::dynamic const& config) {
  auto type = config["type"].asString();
  std::unique_ptr<AnimatedNode> node;
  if (type == "props") {
    node = std::make_unique<PropsAnimatedNode>(config, *this);
  } else if (type == "style") {
    node = std::make_unique<StyleAnimatedNode>(config, *this);
  } else if (type == "value") {
    node = std::make_unique<ValueAnimatedNode>(config);
  } else if (type == "transform") {
    node = std::make_unique<TransformAnimatedNode>(config, *this);
  } else if (type == "interpolation") {
    node = std::make_unique<InterpolationAnimatedNode>(config, *this);
  } else if (type == "addition") {
    node = std::make_unique<AdditionAnimatedNode>(config, *this);
  } else if (type == "subtraction") {
    node = std::make_unique<SubtractionAnimatedNode>(config, *this);
  } else if (type == "multiplication") {
    node = std::make_unique<MultiplicationAnimatedNode>(config, *this);
  } else if (type == "division") {
    node = std::make_unique<DivisionAnimatedNode>(config, *this);
  } else if (type == "diffclamp") {
    node = std::make_unique<DiffClampAnimatedNode>(config, *this);
  } else if (type == "tracking") {
    node = std::make_unique<TrackingAnimatedNode>(config, *this);
  } else if (type == "modulus") {
    node = std::make_unique<ModulusAnimatedNode>(config, *this);
  } else {
    throw std::runtime_error("Unsupported animated node type: " + type);
  }

  node->tag_ = tag;
  m_nodeByTag.insert({tag, std::move(node)});
  m_nodeTagsToUpdate.insert(tag);
}

void AnimatedNodesManager::dropNode(facebook::react::Tag tag) {
  m_nodeTagsToUpdate.erase(tag);
  m_nodeByTag.erase(tag);
}

void AnimatedNodesManager::connectNodes(
    facebook::react::Tag parentTag,
    facebook::react::Tag childTag) {
  auto& parent = getNodeByTag(parentTag);
  auto& child = getNodeByTag(childTag);

  parent.addChild(child);
  m_nodeTagsToUpdate.insert(childTag);
}

void AnimatedNodesManager::disconnectNodes(
    facebook::react::Tag parentTag,
    facebook::react::Tag childTag) {
  auto& parent = getNodeByTag(parentTag);
  auto& child = getNodeByTag(childTag);

  parent.removeChild(child);
  m_nodeTagsToUpdate.insert(childTag);
}

void AnimatedNodesManager::connectNodeToView(
    facebook::react::Tag nodeTag,
    facebook::react::Tag viewTag) {
  auto& node = dynamic_cast<PropsAnimatedNode&>(getNodeByTag(nodeTag));
  node.connectToView(viewTag);
  m_nodeTagsToUpdate.insert(nodeTag);
  // make sure the new properties are applied immediately even when they're not
  // animated
  maybeStartAnimations();
}

void AnimatedNodesManager::disconnectNodeFromView(
    facebook::react::Tag nodeTag,
    facebook::react::Tag viewTag) {
  auto& node = dynamic_cast<PropsAnimatedNode&>(getNodeByTag(nodeTag));
  node.disconnectFromView(viewTag);
}

void AnimatedNodesManager::addAnimatedEventToView(
    react::Tag viewTag,
    const std::string& eventName,
    const folly::dynamic& eventMapping) {
  auto nodeTag = eventMapping["animatedValueTag"].asInt();
  auto dynamicNativeEventPath = eventMapping["nativeEventPath"];
  std::vector<std::string> nativeEventPath;
  for (auto& key : dynamicNativeEventPath) {
    nativeEventPath.push_back(key.asString());
  }
  m_eventDrivers.push_back(
      std::make_unique<EventAnimationDriver>(
          eventName, viewTag, std::move(nativeEventPath), nodeTag, *this));
}

void AnimatedNodesManager::removeAnimatedEventFromView(
    facebook::react::Tag viewTag,
    std::string const& eventName,
    facebook::react::Tag animatedValueTag) {
  if (!m_eventDrivers.empty()) {
    m_eventDrivers.erase(
        std::remove_if(
            m_eventDrivers.begin(),
            m_eventDrivers.end(),
            [&](auto& driver) {
              return driver->getViewTag() == viewTag &&
                  driver->getEventName() == eventName &&
                  driver->getNodeTag() == animatedValueTag;
            }),
        m_eventDrivers.end());
  }
}

void AnimatedNodesManager::startListeningToAnimatedNodeValue(
    facebook::react::Tag tag,
    ValueAnimatedNode::AnimatedNodeValueListener&& listener) {
  auto& node = getValueNodeByTag(tag);
  node.setValueListener(std::move(listener));
}

void AnimatedNodesManager::stopListeningToAnimatedNodeValue(
    facebook::react::Tag tag) {
  auto& node = getValueNodeByTag(tag);
  node.removeValueListener();
}

PropUpdatesList AnimatedNodesManager::handleEvent(
    facebook::react::Tag targetTag,
    std::string const& eventName,
    folly::dynamic const& eventValue) {
  bool someDriverNeedsUpdate = false;
  for (auto& driver : m_eventDrivers) {
    if (driver->getViewTag() == targetTag &&
        driver->getEventName() == eventName) {
      someDriverNeedsUpdate = true;
      driver->updateWithEvent(eventValue);
    }
  }

  if (someDriverNeedsUpdate) {
    // NOTE: we don't update frame-based drivers here, only the newly-dirty ones
    // from the event
    return updateNodes();
  }
  return {};
}

void AnimatedNodesManager::setValue(facebook::react::Tag tag, double value) {
  auto& node = getValueNodeByTag(tag);
  stopAnimationsForNode(tag);
  m_nodeTagsToUpdate.insert(tag);
  node.setValue(value);
  maybeStartAnimations();
}

void AnimatedNodesManager::setOffset(facebook::react::Tag tag, double offset) {
  auto& node = getValueNodeByTag(tag);
  m_nodeTagsToUpdate.insert(tag);
  node.setOffset(offset);
  maybeStartAnimations();
}

float AnimatedNodesManager::getScaleRatioDpi(bool isAxisX) const {
  return isAxisX ? ArkTSBridge::getInstance()->getScaleRatioDpiX()
                 : ArkTSBridge::getInstance()->getScaleRatioDpiY();
}

void AnimatedNodesManager::flattenOffset(facebook::react::Tag tag) {
  auto& node = getValueNodeByTag(tag);
  node.flattenOffset();
}

void AnimatedNodesManager::extractOffset(facebook::react::Tag tag) {
  auto& node = getValueNodeByTag(tag);
  node.extractOffset();
}

folly::dynamic AnimatedNodesManager::getNodeOutput(facebook::react::Tag tag) {
  auto& node = getValueNodeByTag(tag);
  return node.getOutput();
}

void AnimatedNodesManager::startAnimatingNode(
    facebook::react::Tag animationId,
    facebook::react::Tag nodeTag,
    folly::dynamic const& config,
    std::function<void(bool)>&& endCallback) {
  auto type = config["type"].asString();
  auto& node = getValueNodeByTag(nodeTag);

  if (auto it = m_animationById.find(animationId);
      it != m_animationById.end()) {
    it->second->resetConfig(config);
    return;
  }

  std::unique_ptr<AnimationDriver> driver;
  if (type == "frames") {
    driver = std::make_unique<FrameBasedAnimationDriver>(
        animationId, nodeTag, *this, config, std::move(endCallback));
  } else if (type == "spring") {
    driver = std::make_unique<SpringAnimationDriver>(
        animationId, nodeTag, *this, config, std::move(endCallback));
  } else if (type == "decay") {
    driver = std::make_unique<DecayAnimationDriver>(
        animationId, nodeTag, *this, config, std::move(endCallback));
  } else {
    throw std::runtime_error("Unsupported animation type: " + type);
  }

  m_animationById.insert({animationId, std::move(driver)});
  maybeStartAnimations();
}

void AnimatedNodesManager::stopAnimation(facebook::react::Tag animationId) {
  if (auto it = m_animationById.find(animationId);
      it != m_animationById.end()) {
    if (it->second->getId() == animationId) {
      m_animationById.erase(animationId);
    }
  }
}

PropUpdatesList AnimatedNodesManager::runUpdates(long long frameTimeNanos) {
  // we don't want to enter this while updating nodes (which can happen if a
  // tracking node starts a new animation)
  m_isRunningAnimations = true;
  std::vector<facebook::react::Tag> finishedAnimations;
  std::vector<facebook::react::Tag> valueNodeTags;
  for (auto& [animationId, driver] : m_animationById) {
    if (!driver) {
      continue;
    }
    driver->runAnimationStep(frameTimeNanos);
    auto nodeTag = driver->getAnimatedValueTag();
    valueNodeTags.push_back(nodeTag);
    m_nodeTagsToUpdate.insert(nodeTag);
    if (driver->hasFinished()) {
      finishedAnimations.push_back(animationId);
    }
  }

  auto propUpdatesList = updateNodes();

  int32_t finalFrameRate;
  if (IsAtLeastApi20()) {
    finalFrameRate = getMinAcceptableFrameRate(valueNodeTags);
  }

  for (auto animationId : finishedAnimations) {
    m_animationById.at(animationId)->endCallback_(true);
    m_animationById.at(animationId)->endCallback_ = nullptr;
    m_animationById.erase(animationId);
  }
  if (m_animationById.empty()) {
    if (IsAtLeastApi20()) {
      m_scheduleStopFn();
    }
    m_isRunningAnimations = false;
  } else {
    m_isRunningAnimations = true;
    if (IsAtLeastApi20()) {
      m_scheduleUpdateFn(finalFrameRate);
    } else {
      m_scheduleStartFn();
    }
  }
  return propUpdatesList;
}

void AnimatedNodesManager::setNeedsUpdate(facebook::react::Tag nodeTag) {
  m_nodeTagsToUpdate.insert(nodeTag);
}

PropUpdatesList AnimatedNodesManager::updateNodes() {
  std::vector<facebook::react::Tag> nodeTags(
      m_nodeTagsToUpdate.begin(), m_nodeTagsToUpdate.end());
  m_nodeTagsToUpdate.clear();

  uint64_t activeNodesCount = 0;
  uint64_t updatedNodesCount = 0;

  std::queue<react::Tag> nodeTagsQueue;
  for (auto node : nodeTags) {
    nodeTagsQueue.push(node);
  }
  std::unordered_set<react::Tag> visitedNodeTags;
  std::unordered_map<react::Tag, uint64_t> incomingEdgesCount;

  // first, we traverse the nodes graph to find all active nodes
  // and count incoming edges for each node
  while (!nodeTagsQueue.empty()) {
    auto tag = nodeTagsQueue.front();
    nodeTagsQueue.pop();

    if (visitedNodeTags.find(tag) != visitedNodeTags.end()) {
      continue;
    }

    visitedNodeTags.insert(tag);
    try {
      auto& node = getNodeByTag(tag);
      activeNodesCount++;

      for (auto childTag : node.getChildrenTags()) {
        nodeTagsQueue.push(childTag);
        incomingEdgesCount[childTag]++;
      }
    } catch (std::out_of_range& _e) {
      // if a node is not found we skip over it and proceed with the
      // animation to maintain consistency with other platforms
      continue;
    }
  }

  // second, we visit all active nodeTags with no incoming edges
  // (meaning they're either the root nodeTags, or their parents were already
  // visited) and perform the updates
  for (auto node : nodeTags) {
    if (incomingEdgesCount[node] == 0) {
      nodeTagsQueue.push(node);
    }
  }

  PropUpdatesList propUpdatesList;
  while (!nodeTagsQueue.empty()) {
    auto tag = nodeTagsQueue.front();
    nodeTagsQueue.pop();

    try {
      auto& node = getNodeByTag(tag);
      node.update();

      if (auto propsNode = dynamic_cast<PropsAnimatedNode*>(&node);
          propsNode != nullptr) {
        auto propUpdate = propsNode->updateView();
        if (propUpdate.has_value()) {
          propUpdatesList.push_back(std::move(propUpdate.value()));
        }
      }

      if (auto valueNode = dynamic_cast<ValueAnimatedNode*>(&node);
          valueNode != nullptr) {
        valueNode->onValueUpdate();
      }

      updatedNodesCount++;

      for (auto childTag : node.getChildrenTags()) {
        incomingEdgesCount[childTag]--;
        if (incomingEdgesCount[childTag] == 0) {
          nodeTagsQueue.push(childTag);
        }
      }

    } catch (std::out_of_range& _e) {
      // if a node is not found we skip over it and proceed with the
      // animation to maintain consistency with other platforms
      continue;
    }
  }
  if (activeNodesCount != updatedNodesCount) {
    // if not all active nodes were updated, it means there's a cycle in the
    // graph
    throw std::runtime_error(
        "There were " + std::to_string(activeNodesCount) +
        " active nodes, but only " + std::to_string(updatedNodesCount) +
        " were updated");
  }

  return propUpdatesList;
}

void AnimatedNodesManager::stopAnimationsForNode(facebook::react::Tag tag) {
  std::vector<facebook::react::Tag> animationsToStop;
  for (auto& [animationId, driver] : m_animationById) {
    if (driver->getAnimatedValueTag() == tag) {
      animationsToStop.push_back(animationId);
    }
  }
  for (auto id : animationsToStop) {
    stopAnimation(id);
  }
}

void AnimatedNodesManager::maybeStartAnimations() {
  if (!m_isRunningAnimations) {
    m_scheduleStartFn();
  }
}

int32_t AnimatedNodesManager::getMinAcceptableFrameRate(
    const std::vector<facebook::react::Tag>& valueNodeTags) {
  int32_t finalFrameRate = 30;
  for (const auto& tag : valueNodeTags) {
    auto& node = getValueNodeByTag(tag);
    int32_t currentFrameRate = node.getFrameRate();
    finalFrameRate = std::max(finalFrameRate, currentFrameRate);
  }
  return finalFrameRate;
}

AnimatedNode& AnimatedNodesManager::getNodeByTag(facebook::react::Tag tag) {
  try {
    return *m_nodeByTag.at(tag);
  } catch (std::out_of_range& e) {
    std::throw_with_nested(
        std::out_of_range(
            "Animated node with tag " + std::to_string(tag) +
            " does not exist"));
  }
}

ValueAnimatedNode& AnimatedNodesManager::getValueNodeByTag(
    facebook::react::Tag tag) {
  auto& node = getNodeByTag(tag);

  try {
    return dynamic_cast<ValueAnimatedNode&>(node);
  } catch (std::bad_cast& e) {
    std::throw_with_nested(
        std::out_of_range(
            "Animated node with tag " + std::to_string(tag) +
            " is not a value node"));
  }
}

} // namespace rnoh
