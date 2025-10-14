/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "RNOH/BaseComponentJSIBinder.h"
#include "RNOH/UIManagerModule.h"

namespace rnoh {
class ViewComponentJSIBinder : public BaseComponentJSIBinder {
 protected:
  facebook::jsi::Object createNativeProps(facebook::jsi::Runtime& rt) override {
    auto nativeProps = BaseComponentJSIBinder::createNativeProps(rt);

    nativeProps.setProperty(rt, "removeClippedSubviews", "boolean");

    // #region ViewEvents

    // Pointer events
    nativeProps.setProperty(rt, "onPointerEnter", "boolean");
    nativeProps.setProperty(rt, "onPointerMove", "boolean");
    nativeProps.setProperty(rt, "onPointerLeave", "boolean");

    // PanResponder callbacks
    nativeProps.setProperty(rt, "onMoveShouldSetResponder", "boolean");
    nativeProps.setProperty(rt, "onMoveShouldSetResponderCapture", "boolean");
    nativeProps.setProperty(rt, "onStartShouldSetResponder", "boolean");
    nativeProps.setProperty(rt, "onStartShouldSetResponderCapture", "boolean");
    nativeProps.setProperty(rt, "onResponderGrant", "boolean");
    nativeProps.setProperty(rt, "onResponderReject", "boolean");
    nativeProps.setProperty(rt, "onResponderStart", "boolean");
    nativeProps.setProperty(rt, "onResponderEnd", "boolean");
    nativeProps.setProperty(rt, "onResponderRelease", "boolean");
    nativeProps.setProperty(rt, "onResponderMove", "boolean");
    nativeProps.setProperty(rt, "onResponderTerminate", "boolean");
    nativeProps.setProperty(rt, "onResponderTerminationRequest", "boolean");
    nativeProps.setProperty(rt, "onShouldBlockNativeResponder", "boolean");

    // Touch events
    nativeProps.setProperty(rt, "onTouchStart", "boolean");
    nativeProps.setProperty(rt, "onTouchMove", "boolean");
    nativeProps.setProperty(rt, "onTouchEnd", "boolean");
    nativeProps.setProperty(rt, "onTouchCancel", "boolean");

    // W3C Pointer Events
    nativeProps.setProperty(rt, "onPointerEnter", "boolean");
    nativeProps.setProperty(rt, "onPointerLeave", "boolean");
    nativeProps.setProperty(rt, "onPointerMove", "boolean");
    nativeProps.setProperty(rt, "onPointerOver", "boolean");
    nativeProps.setProperty(rt, "onPointerOut", "boolean");
    nativeProps.setProperty(rt, "onPointerEnterCapture", "boolean");
    nativeProps.setProperty(rt, "onPointerLeaveCapture", "boolean");
    nativeProps.setProperty(rt, "onPointerMoveCapture", "boolean");

    // #endregion

    return nativeProps;
  }
  facebook::jsi::Object createBubblingEventTypes(
      facebook::jsi::Runtime& rt) override {
    facebook::jsi::Object events(rt);
    events.setProperty(
        rt, "topTouchStart", createBubblingCapturedEvent(rt, "onTouchStart"));
    events.setProperty(
        rt, "topTouchMove", createBubblingCapturedEvent(rt, "onTouchMove"));
    events.setProperty(
        rt, "topTouchEnd", createBubblingCapturedEvent(rt, "onTouchEnd"));
    events.setProperty(
        rt, "topTouchCancel", createBubblingCapturedEvent(rt, "onTouchCancel"));
    events.setProperty(
        rt, "topFocus", createBubblingCapturedEvent(rt, "onFocus"));
    events.setProperty(
        rt, "topBlur", createBubblingCapturedEvent(rt, "onBlur"));
    return events;
  }
  facebook::jsi::Object createDirectEventTypes(
      facebook::jsi::Runtime& rt) override {
    facebook::jsi::Object events =
        BaseComponentJSIBinder::createDirectEventTypes(rt);
    events.setProperty(rt, "topClick", createDirectEvent(rt, "onClick"));
    events.setProperty(
        rt, "topPointerEnter", createDirectEvent(rt, "onPointerEnter"));
    events.setProperty(
        rt, "topPointerLeave", createDirectEvent(rt, "onPointerLeave"));
    return events;
  }
};

} // namespace rnoh