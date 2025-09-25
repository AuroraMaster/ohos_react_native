/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "TouchEventDispatcher.h"
#include <glog/logging.h>
#include <set>
#include "RNOH/Assert.h"

namespace rnoh {
using Point = facebook::react::Point;

static std::pair<TouchTarget::Shared, Point> findTargetForTouchPoint(
    Point const& point,
    TouchTarget::Shared const& target) {
  bool canHandleTouch = target->canHandleTouch() &&
      target->containsPoint(point) &&
      (target->getTouchEventEmitter() != nullptr);
  bool canChildrenHandleTouch = target->canChildrenHandleTouch() &&
      target->containsPointInBoundingBox(point);

  if (canChildrenHandleTouch) {
    auto children = target->getTouchTargetChildren();
    // we want to check the children in reverse order, since the last child is
    // the topmost one
    std::reverse(children.begin(), children.end());
    for (auto const& child : children) {
      if (child == nullptr) {
        RNOH_ASSERT(child != nullptr);
        continue;
      }
      // If the child is used as a root touch target, we shouldn't attempt to
      // find the touch point there. This prevents detecting unnecessary touches
      // on components that are intended to receive touches only from ArkTS.
      if (child->isRootTouchTarget()) {
        continue;
      }
      auto childPoint = target->computeChildPoint(point, child);
      auto result = findTargetForTouchPoint(childPoint, child);
      if (result.first != nullptr) {
        return result;
      }
    }
  }

  if (canHandleTouch) {
    return std::make_pair(target, point);
  }

  return std::make_pair(nullptr, Point{});
}

std::optional<facebook::react::Touch> convertTouchPointToReactTouch(
    TouchPoint const& touchPoint,
    TouchTarget::Shared const& target,
    double timestampSeconds,
    TouchTarget::Shared const& rootTarget) {
  Point rootPoint{
      .x = static_cast<facebook::react::Float>(touchPoint.nodeX),
      .y = static_cast<facebook::react::Float>(touchPoint.nodeY)};

  Point screenPoint{
      .x = static_cast<facebook::react::Float>(touchPoint.screenX),
      .y = static_cast<facebook::react::Float>(touchPoint.screenY)};

  facebook::react::Point touchTargetPoint;
  auto shouldCancelTouch = false;
  std::vector<TouchTarget::Shared> parents;
  auto currentTarget = target;
  while (currentTarget != rootTarget) {
    if (currentTarget == nullptr) {
      shouldCancelTouch = true;
      break;
    }
    parents.push_back(currentTarget);
    currentTarget = currentTarget->getTouchTargetParent();
  }
  parents.push_back(currentTarget);

  if (shouldCancelTouch) {
    return std::nullopt;
  } else {
    touchTargetPoint = rootPoint;
    for (auto i = parents.size() - 1; i > 0; i--) {
      touchTargetPoint =
          parents[i]->computeChildPoint(touchTargetPoint, parents[i - 1]);
    }
  }

  facebook::react::Touch touch = {
      .pagePoint = rootPoint,
      .offsetPoint = touchTargetPoint,
      .screenPoint = screenPoint,
      .identifier = touchPoint.id,
      .target = target->getTouchTargetTag(),
      .force = touchPoint.force,
      .timestamp = timestampSeconds};

  return touch;
}

bool isAncestorHandlingTouches(
    TouchTarget::Shared touchTarget,
    TouchTarget::Shared const& rootTarget) {
  auto tmpTouchTarget = touchTarget;
  while (tmpTouchTarget && tmpTouchTarget != rootTarget) {
    if (tmpTouchTarget->isHandlingTouches()) {
      return true;
    }
    tmpTouchTarget = tmpTouchTarget->getTouchTargetParent();
  }
  return false;
}

bool TouchEventDispatcher::canIgnoreMoveEvent(
    facebook::react::TouchEvent currentEvent) {
  if (m_previousEvent.touches.empty()) {
    return false;
  }
  for (const auto& touch : currentEvent.changedTouches) {
    auto previousTouch = m_previousEvent.touches.find(touch);
    if (previousTouch != m_previousEvent.touches.end()) {
      auto dx = previousTouch->pagePoint.x - touch.pagePoint.x;
      auto dy = previousTouch->pagePoint.y - touch.pagePoint.y;
      if ((dx * dx + dy * dy) > 1) {
        return false;
      }
    } else {
      LOG(ERROR) << "Moved touch with id: " << touch.identifier
                 << " could not be found in previous touch event.";
    }
  }
  return true;
}

void TouchEventDispatcher::findTargetAndSendTouchEvent(
    TouchTarget::Shared const& rootTarget,
    const TouchEvent& touchEvent) {
  // react-native expects a timestamp in seconds (because rn multiplies the
  // value by 1e3). The timestamp passed by ArkUI is in nanoseconds. We convert
  // it first to miliseconds before casting to lose unnecessary precision. Then
  // we cast it to a double and convert it to seconds.
  double timestampSeconds = static_cast<double>(touchEvent.timestamp) / 1e9;

  facebook::react::Touches touches(m_previousEvent.touches);
  facebook::react::Touches changedTouches;
  facebook::react::Touches cancelTouches;

  for (const auto& activeTouch : touchEvent.activeTouchPoints) {
    if (touchEvent.action == UI_TOUCH_EVENT_ACTION_DOWN) {
      auto [touchTarget, touchPoint] = findTargetForTouchPoint(
          Point{
              .x = static_cast<facebook::react::Float>(activeTouch.nodeX),
              .y = static_cast<facebook::react::Float>(activeTouch.nodeY)},
          rootTarget);
      if (touchTarget == nullptr) {
        continue;
      }
      if (isAncestorHandlingTouches(touchTarget, rootTarget)) {
        continue;
      }
      registerTargetForTouch(activeTouch, touchTarget);
    } else if (auto touchTargetEntry =
                   m_touchTargetByTouchId.find(activeTouch.id);
               touchTargetEntry != m_touchTargetByTouchId.end()) {
      const auto& touchTarget = touchTargetEntry->second;
      if (touchTarget == nullptr) {
        continue;
      }
      if (isAncestorHandlingTouches(touchTarget, rootTarget)) {
        cancelPreviousTouchEvent(timestampSeconds, touchTarget);
        m_touchTargetByTouchId.erase(activeTouch.id);
        continue;
      }
    }

    auto it = m_touchTargetByTouchId.find(activeTouch.id);
    if (it == m_touchTargetByTouchId.end()) {
      LOG(WARNING) << "No target for current touch event with id: "
              << activeTouch.id;
      continue;
    }
    const auto& eventTarget = it->second;
    if (eventTarget == nullptr) {
      LOG(WARNING) << "Target for current touch event has been deleted";
      m_touchTargetByTouchId.erase(it);
      continue;
    }

    auto touch = convertTouchPointToReactTouch(
        activeTouch, eventTarget, timestampSeconds, rootTarget);
    if (!touch.has_value() ||
        touchEvent.action == UI_TOUCH_EVENT_ACTION_CANCEL) {
      Point rootPoint{
          .x = static_cast<facebook::react::Float>(activeTouch.nodeX),
          .y = static_cast<facebook::react::Float>(activeTouch.nodeY)};

      Point screenPoint{
          .x = static_cast<facebook::react::Float>(activeTouch.screenX),
          .y = static_cast<facebook::react::Float>(activeTouch.screenY)};

      facebook::react::Touch cancelTouch = {
          .pagePoint = rootPoint,
          .offsetPoint = {0, 0},
          .screenPoint = screenPoint,
          .identifier = activeTouch.id,
          .target = eventTarget->getTouchTargetTag(),
          .force = activeTouch.force,
          .timestamp = timestampSeconds};

      cancelTouches.insert(cancelTouch);
      continue;
    }

    changedTouches.insert(touch.value());
  }

  if (!cancelTouches.empty()) {
    sendEvent({}, cancelTouches, UI_TOUCH_EVENT_ACTION_CANCEL);
  }

  if (changedTouches.empty()) {
    return;
  }

  if (touchEvent.action == UI_TOUCH_EVENT_ACTION_MOVE) {
    touches = changedTouches;
  } else {
    const auto& touch = *(changedTouches.begin());
    if (touchEvent.action == UI_TOUCH_EVENT_ACTION_UP) {
      touches.erase(touch);
    } else {
      // update touches and targetTouches with the new touch object
      touches.erase(touch);
      touches.insert(touch);
    }
  }

  sendEvent(touches, changedTouches, touchEvent.action);
}

void TouchEventDispatcher::dispatchTouchEvent(
    ArkUI_UIInputEvent* event,
    TouchTarget::Shared const& rootTarget) {
  TouchEvent touchEvent(event);
  findTargetAndSendTouchEvent(rootTarget, touchEvent);
}

void TouchEventDispatcher::dispatchTouchEvent(
    const TouchEvent& event,
    TouchTarget::Shared const& rootTarget) {
  findTargetAndSendTouchEvent(rootTarget, event);
}

TouchTarget::Shared TouchEventDispatcher::registerTargetForTouch(
    TouchPoint activeTouch,
    TouchTarget::Shared const& touchTarget) {
  auto id = activeTouch.id;
  if (m_touchTargetByTouchId.find(id) != m_touchTargetByTouchId.end()) {
    LOG(ERROR) << "Touch with id " << id << " already exists";
    return nullptr;
  }

  if (touchTarget) {
    m_touchTargetByTouchId.emplace(activeTouch.id, touchTarget);
    VLOG(2) << "Touch with id " << id << " started on target with tag "
            << touchTarget->getTouchTargetTag();
    return touchTarget;
  }
  return nullptr;
}

void TouchEventDispatcher::cancelPreviousTouchEvent(
    double timestampInSecs,
    TouchTarget::Shared touchTarget) {
  // create new touch event based on the previously emitted event
  auto touchCancelEvent = m_previousEvent;
  touchCancelEvent.targetTouches = {};
  touchCancelEvent.changedTouches = {};
  touchCancelEvent.touches = {};

  for (auto touch : m_previousEvent.touches) {
    if (touch.target != touchTarget->getTouchTargetTag())
      continue;

    if (timestampInSecs > 0) {
      touch.timestamp = timestampInSecs;
    }
    touchCancelEvent.changedTouches.insert(touch);
  }

  // emit cancel event
  DLOG(INFO) << "TOUCH::CANCEL";
  touchTarget->getTouchEventEmitter()->onTouchCancel(touchCancelEvent);
}

void TouchEventDispatcher::sendEvent(
    facebook::react::Touches const& touches,
    facebook::react::Touches const& changedTouches,
    int32_t action) {
  std::unordered_map<TouchTarget::Shared, facebook::react::Touches>
      touchesByTarget;

  for (auto const& touch : changedTouches) {
    auto touchTargetIt = m_touchTargetByTouchId.find(touch.identifier);
    if (touchTargetIt == m_touchTargetByTouchId.end()) {
      LOG(ERROR) << "Touch with id " << touch.identifier
                 << " has no target associated with it";
      continue;
    }

    const auto& touchTarget = touchTargetIt->second;
    if (touchTarget == nullptr) {
      m_touchTargetByTouchId.erase(touchTargetIt);
      continue;
    }

    touchesByTarget[touchTarget].insert(touch);

    if (action == UI_TOUCH_EVENT_ACTION_UP ||
        action == UI_TOUCH_EVENT_ACTION_CANCEL) {
      m_touchTargetByTouchId.erase(touch.identifier);
    }
  }

  for (auto const& touch : touches) {
    auto touchTargetIt = m_touchTargetByTouchId.find(touch.identifier);
    if (touchTargetIt == m_touchTargetByTouchId.end()) {
      LOG(ERROR) << "Touch with id " << touch.identifier
                 << " has no target associated with it";
      continue;
    }

    const auto& touchTarget = touchTargetIt->second;
    if (touchTarget == nullptr) {
      m_touchTargetByTouchId.erase(touchTargetIt);
      continue;
    }
    auto targetIter = touchesByTarget.find(touchTarget);
    if (targetIter != touchesByTarget.end()) {
      targetIter->second.insert(touch);
    }
  }

  for (auto const& [eventTarget, targetTouches] : touchesByTarget) {
    facebook::react::TouchEvent touchEvent{
        .touches = touches,
        .changedTouches = changedTouches,
        .targetTouches = targetTouches};

    if (action == UI_TOUCH_EVENT_ACTION_MOVE &&
        canIgnoreMoveEvent(touchEvent)) {
      continue;
    }

    m_previousEvent = touchEvent;
    switch (action) {
      case UI_TOUCH_EVENT_ACTION_DOWN:
        DLOG(INFO) << "TouchEventDispatcher::sendEvent DOWN";
        eventTarget->getTouchEventEmitter()->onTouchStart(touchEvent);
        break;
      case UI_TOUCH_EVENT_ACTION_MOVE:
        DLOG(INFO) << "TouchEventDispatcher::sendEvent MOVE";
        eventTarget->getTouchEventEmitter()->onTouchMove(touchEvent);
        break;
      case UI_TOUCH_EVENT_ACTION_UP:
        DLOG(INFO) << "TouchEventDispatcher::sendEvent UP";
        eventTarget->getTouchEventEmitter()->onTouchEnd(touchEvent);
        break;
      case UI_TOUCH_EVENT_ACTION_CANCEL:
      default:
        DLOG(INFO) << "TouchEventDispatcher::sendEvent CANCEL";
        eventTarget->getTouchEventEmitter()->onTouchCancel(touchEvent);
        break;
    }
  }
}

void TouchEventDispatcher::cancelActiveTouches() {
  for (const auto& touch : m_previousEvent.changedTouches) {
    if (m_touchTargetByTouchId.find(touch.identifier) ==
        m_touchTargetByTouchId.end()) {
      continue;
    }
    auto touchIdAndTouchTarget = m_touchTargetByTouchId.find(touch.identifier);
    auto touchTarget = touchIdAndTouchTarget->second;
    if (touchTarget) {
      if (m_previousEvent.changedTouches.size() > 0) {
        DLOG(INFO) << "TOUCH::CANCEL";
        m_touchTargetByTouchId.erase(touchIdAndTouchTarget->first);
        touchTarget->getTouchEventEmitter()->onTouchCancel(m_previousEvent);
      }
    }
  }
}

} // namespace rnoh
