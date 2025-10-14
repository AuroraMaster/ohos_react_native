/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <arkui/ui_input_event.h>
#include <unordered_map>
#include "RNOH/TouchTarget.h"
#include "TouchEvent.h"

namespace rnoh {
/**
 * TouchEventDispatcher handles the dispatching of touch events from root target
 * components(e.g., ViewComponentInstance, ModalHostViewComponentInstance)
 * to the JavaScript layer.
 * @actor RNOH_LIBRARY
 */
class TouchEventDispatcher {
 public:
  using TouchId = int;

  /**
   * @brief Dispatch the touch events to JS.
   * @param ｛ArkUI_UIInputEvent*｝ event The UI input event.
   * @param rootTarget The root target that listens to touch (NODE_TOUCH_EVENT)
   * events.
   */
  void dispatchTouchEvent(
      ArkUI_UIInputEvent* event,
      TouchTarget::Shared const& rootTarget);

  /**
   * @brief Dispatch the touch events to JS.
   * @param ｛TouchEvent&｝ event The touch event.
   * @param rootTarget The root target that listens to touch (NODE_TOUCH_EVENT)
   * events.
   */
  void dispatchTouchEvent(
      const TouchEvent& event,
      TouchTarget::Shared const& rootTarget);

  /**
   * @internal
   * Native apps can place RNSurface inside a Scroll component. When that Scroll
   * is being scrolled, RN shouldn't recognize swipe gestures as clicks. To
   * achieve this, RN needs to receive a CANCEL event. At the time of creating
   * this method, the platform didn't send the CANCEL touch event, so RNOH
   * provides a way for RN developers to cancel those events manually. They can
   * register an appropriate onScroll callback and call
   * rnohCoreContext.cancelTouches().
   */
  void cancelActiveTouches();

 private:
  void findTargetAndSendTouchEvent(
      TouchTarget::Shared const& rootTarget,
      const TouchEvent& touchEvent);

  TouchTarget::Shared registerTargetForTouch(
      TouchPoint touchPoint,
      TouchTarget::Shared const& rootTarget);
  bool canIgnoreMoveEvent(facebook::react::TouchEvent currentEvent);
  void cancelPreviousTouchEvent(
      double timestampInMs,
      TouchTarget::Shared touchTarget);
  void sendEvent(
      facebook::react::Touches const& touches,
      facebook::react::Touches const& changedTouches,
      int32_t action);

  std::unordered_map<TouchId, TouchTarget::Shared> m_touchTargetByTouchId;
  facebook::react::TouchEvent m_previousEvent;
};
} // namespace rnoh
