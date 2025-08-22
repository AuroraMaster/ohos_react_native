/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>

#include <folly/dynamic.h>
#include <jsi/jsi.h>
#include <react/renderer/core/ReactPrimitives.h>

#include "Drivers/AnimationDriver.h"
#include "Drivers/EventAnimationDriver.h"
#include "Nodes/AnimatedNode.h"
#include "RNOH/ApiVersionCheck.h"

namespace rnoh {
using PropUpdate = std::pair<facebook::react::Tag, folly::dynamic>;
using PropUpdatesList = std::vector<PropUpdate>;

class AnimatedNode;
class ValueAnimatedNode;
class AnimationDriver;
class EventAnimationDriver;

class AnimatedNodesManager {
 public:
  using EndCallback = std::function<void(bool)>;

  AnimatedNodesManager(
      const std::function<void(int)>& scheduleUpdateFn,
      const std::function<void()>& scheduleStartFn,
      const std::function<void()>& scheduleStopFn);

  void createNode(facebook::react::Tag tag, folly::dynamic const& config);
  void dropNode(facebook::react::Tag tag);

  void connectNodes(
      facebook::react::Tag parentTag,
      facebook::react::Tag childTag);
  void disconnectNodes(
      facebook::react::Tag parentTag,
      facebook::react::Tag childTag);

  void connectNodeToView(
      facebook::react::Tag nodeTag,
      facebook::react::Tag viewTag);
  void disconnectNodeFromView(
      facebook::react::Tag nodeTag,
      facebook::react::Tag viewTag);

  void addAnimatedEventToView(
      facebook::react::Tag viewTag,
      std::string const& eventName,
      folly::dynamic const& eventMapping);
  void removeAnimatedEventFromView(
      facebook::react::Tag viewTag,
      std::string const& eventName,
      facebook::react::Tag nodeTag);

  void startListeningToAnimatedNodeValue(
      facebook::react::Tag tag,
      ValueAnimatedNode::AnimatedNodeValueListener&& listener);
  void stopListeningToAnimatedNodeValue(facebook::react::Tag tag);

  void setValue(facebook::react::Tag tag, double value);
  void setOffset(facebook::react::Tag tag, double offset);
  void flattenOffset(facebook::react::Tag tag);
  void extractOffset(facebook::react::Tag tag);
  float getScaleRatioDpi(bool isAxisX) const;

  folly::dynamic getNodeOutput(facebook::react::Tag tag);

  void startAnimatingNode(
      facebook::react::Tag animationId,
      facebook::react::Tag nodeTag,
      folly::dynamic const& config,
      EndCallback&& endCallback);
  void stopAnimation(facebook::react::Tag animationId);

  PropUpdatesList runUpdates(long long frameTimeNanos);

  void setNeedsUpdate(facebook::react::Tag nodeTag);

  PropUpdatesList handleEvent(
      facebook::react::Tag targetTag,
      std::string const& eventName,
      folly::dynamic const& eventValue);

  AnimatedNode& getNodeByTag(facebook::react::Tag tag);
  ValueAnimatedNode& getValueNodeByTag(facebook::react::Tag tag);

 private:
  PropUpdatesList updateNodes();
  void stopAnimationsForNode(facebook::react::Tag tag);
  void maybeStartAnimations();
  int32_t getMinAcceptableFrameRate(
      const std::vector<facebook::react::Tag>& valueNodeTags);

  const std::function<void(int32_t)> m_scheduleUpdateFn;
  const std::function<void()> m_scheduleStartFn;
  const std::function<void()> m_scheduleStopFn;
  std::unordered_map<facebook::react::Tag, std::unique_ptr<AnimatedNode>>
      m_nodeByTag;
  std::unordered_map<facebook::react::Tag, std::unique_ptr<AnimationDriver>>
      m_animationById;
  std::vector<std::unique_ptr<EventAnimationDriver>> m_eventDrivers;
  std::unordered_set<facebook::react::Tag> m_nodeTagsToUpdate;
  bool m_isRunningAnimations = false;
};

} // namespace rnoh