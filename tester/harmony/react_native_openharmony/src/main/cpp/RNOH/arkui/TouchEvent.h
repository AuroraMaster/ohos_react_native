/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <arkui/ui_input_event.h>
#include "RNOH/TouchTarget.h"

namespace rnoh {

constexpr int32_t ARKUI_MOUSE_POINTER_ID = 1001;
constexpr int32_t RN_MOUSE_POINTER_ID = 0;
constexpr int32_t TOUCH_IDENTIFIER_POOL_OFFSET = 1;

enum class ArkTsTouchType : int32_t {
  Down = 0,
  Up = 1,
  Move = 2,
  Cancel = 3
};

struct TouchPoint {
  int32_t id;
  float force;
  int32_t nodeX;
  int32_t nodeY;
  int32_t screenX;
  int32_t screenY;
};

struct TouchEvent {
  uint32_t action;
  uint64_t timestamp;
  std::vector<TouchPoint> activeTouchPoints;

  explicit TouchEvent(const folly::dynamic& event) {
    this->action = convertFromArkTSToUITouchEventAction(event["type"].asInt());
    this->timestamp = event["timestamp"].asInt();

    if (action == UI_TOUCH_EVENT_ACTION_MOVE) {
      this->activeTouchPoints = getTouchesFromArkTSTouchEvent(event);
    } else {
      this->activeTouchPoints = {getActiveTouchFromArkTSTouchEvent(event)};
    }
  }

  explicit TouchEvent(ArkUI_UIInputEvent* event) {
    this->action = OH_ArkUI_UIInputEvent_GetAction(event);
    this->timestamp = OH_ArkUI_UIInputEvent_GetEventTime(event);

    if (action == UI_TOUCH_EVENT_ACTION_MOVE) {
      this->activeTouchPoints = getTouchesFromUIInputEvent(event);
    } else {
      this->activeTouchPoints = {getActiveTouchFromUIInputEvent(event)};
    }
  }

 private:
  int32_t convertFromArkTSToUITouchEventAction(int32_t type) {
    if (type == static_cast<int32_t>(ArkTsTouchType::Down)) {
      return UI_TOUCH_EVENT_ACTION_DOWN;
    } else if (type == static_cast<int32_t>(ArkTsTouchType::Up)) {
      return UI_TOUCH_EVENT_ACTION_UP;
    } else if (type == static_cast<int32_t>(ArkTsTouchType::Move)) {
      return UI_TOUCH_EVENT_ACTION_MOVE;
    } else if (type == static_cast<int32_t>(ArkTsTouchType::Cancel)) {
      return UI_TOUCH_EVENT_ACTION_CANCEL;
    }
    return type;
  }

  int32_t generatedTouchPointIdentifier(int32_t pointerId) {
    if (pointerId == ARKUI_MOUSE_POINTER_ID) {
      return RN_MOUSE_POINTER_ID;
    } else {
      return pointerId + TOUCH_IDENTIFIER_POOL_OFFSET;
    }
  }

  int32_t generatedTouchPointIdentifier(
      const ArkUI_UIInputEvent* event,
      uint32_t idx) {
    auto pointerId = OH_ArkUI_PointerEvent_GetPointerId(event, idx);
    return generatedTouchPointIdentifier(pointerId);
  }

  std::vector<TouchPoint> getTouchesFromArkTSTouchEvent(
      const folly::dynamic& event) {
    std::vector<TouchPoint> result;
    auto touchPointCount = event["touches"].size();
    result.reserve(touchPointCount);
    for (auto idx = 0; idx < touchPointCount; idx++) {
      result.emplace_back(TouchPoint{
          .id = generatedTouchPointIdentifier(
              int32_t(event["touches"][idx]["id"].asInt())),
          .force = float(event["pressure"].asDouble()),
          .nodeX = int32_t(event["touches"][idx]["x"].asDouble()),
          .nodeY = int32_t(event["touches"][idx]["y"].asDouble()),
          .screenX = int32_t(event["touches"][idx]["displayX"].asDouble()),
          .screenY = int32_t(event["touches"][idx]["displayY"].asDouble())});
    }
    return result;
  }

  std::vector<TouchPoint> getTouchesFromUIInputEvent(
      ArkUI_UIInputEvent* event) {
    std::vector<TouchPoint> result;
    auto touchPointCount = OH_ArkUI_PointerEvent_GetPointerCount(event);
    result.reserve(touchPointCount);
    for (auto idx = 0; idx < touchPointCount; idx++) {
      result.emplace_back(TouchPoint{
          .id = generatedTouchPointIdentifier(event, idx),
          .force = OH_ArkUI_PointerEvent_GetPressure(event, idx),
          .nodeX = int32_t(OH_ArkUI_PointerEvent_GetXByIndex(event, idx)),
          .nodeY = int32_t(OH_ArkUI_PointerEvent_GetYByIndex(event, idx)),
          .screenX =
              int32_t(OH_ArkUI_PointerEvent_GetDisplayXByIndex(event, idx)),
          .screenY =
              int32_t(OH_ArkUI_PointerEvent_GetDisplayYByIndex(event, idx))});
    }
    return result;
  }

  TouchPoint getActiveTouchFromUIInputEvent(ArkUI_UIInputEvent* event) {
    TouchPoint actionTouch{};
    auto screenX = int32_t(OH_ArkUI_PointerEvent_GetDisplayX(event));
    auto screenY = int32_t(OH_ArkUI_PointerEvent_GetDisplayY(event));
    auto touchPointCount = OH_ArkUI_PointerEvent_GetPointerCount(event);

    for (auto idx = 0; idx < touchPointCount; idx++) {
      if (screenX ==
              int32_t(OH_ArkUI_PointerEvent_GetDisplayXByIndex(event, idx)) &&
          screenY ==
              int32_t(OH_ArkUI_PointerEvent_GetDisplayYByIndex(event, idx))) {
        actionTouch = TouchPoint{
            .id = generatedTouchPointIdentifier(event, idx),
            .force = OH_ArkUI_PointerEvent_GetPressure(event, idx),
            .nodeX = int32_t(OH_ArkUI_PointerEvent_GetX(event)),
            .nodeY = int32_t(OH_ArkUI_PointerEvent_GetY(event)),
            .screenX = int32_t(OH_ArkUI_PointerEvent_GetDisplayX(event)),
            .screenY = int32_t(OH_ArkUI_PointerEvent_GetDisplayY(event))};
        break;
      }
    }
    return actionTouch;
  }

  TouchPoint getActiveTouchFromArkTSTouchEvent(const folly::dynamic& event) {
    // Since the ArkTS API does not provide information about the active touch
    // point, the touch point at index 0 is returned.
    TouchPoint actionTouch{};
    if (event["touches"].size() == 0) {
      return actionTouch;
    }

    actionTouch = TouchPoint{
        .id = generatedTouchPointIdentifier(
            int32_t(event["touches"][0]["id"].asInt())),
        .force = float(event["pressure"].asDouble()),
        .nodeX = int32_t(event["touches"][0]["x"].asDouble()),
        .nodeY = int32_t(event["touches"][0]["y"].asDouble()),
        .screenX = int32_t(event["touches"][0]["displayX"].asDouble()),
        .screenY = int32_t(event["touches"][0]["displayY"].asDouble())};
    return actionTouch;
  }
};

} // namespace rnoh
