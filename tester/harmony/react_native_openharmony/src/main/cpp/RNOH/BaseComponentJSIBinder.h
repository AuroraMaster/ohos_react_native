/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "RNOH/UIManagerModule.h"

namespace rnoh {
class BaseComponentJSIBinder : public ComponentJSIBinder {
 public:
  facebook::jsi::Object createBindings(facebook::jsi::Runtime& rt) override {
    facebook::jsi::Object baseManagerConfig(rt);
    baseManagerConfig.setProperty(
        rt, "NativeProps", this->createNativeProps(rt));
    baseManagerConfig.setProperty(rt, "Constants", this->createConstants(rt));
    baseManagerConfig.setProperty(rt, "Commands", this->createCommands(rt));
    baseManagerConfig.setProperty(
        rt, "bubblingEventTypes", this->createBubblingEventTypes(rt));
    baseManagerConfig.setProperty(
        rt, "directEventTypes", this->createDirectEventTypes(rt));
    return baseManagerConfig;
  };

 protected:
  virtual facebook::jsi::Object createNativeProps(facebook::jsi::Runtime& rt) {
    facebook::jsi::Object nativeProps(rt);
    nativeProps.setProperty(rt, "nativeID", true);
    nativeProps.setProperty(rt, "testID", true);
    nativeProps.setProperty(rt, "hitSlop", true);
    nativeProps.setProperty(rt, "onLayout", true);
    nativeProps.setProperty(rt, "bottom", true);
    nativeProps.setProperty(rt, "height", true);
    nativeProps.setProperty(rt, "left", true);
    nativeProps.setProperty(rt, "right", true);
    nativeProps.setProperty(rt, "top", true);
    nativeProps.setProperty(rt, "width", true);
    nativeProps.setProperty(rt, "opacity", true);
    nativeProps.setProperty(rt, "backgroundColor", "Color");
    nativeProps.setProperty(rt, "pointerEvents", "string");
    nativeProps.setProperty(rt, "collapsable", "boolean");
    nativeProps.setProperty(rt, "focusable", "boolean");
    nativeProps.setProperty(rt, "accessible", "boolean");
    nativeProps.setProperty(rt, "accessibilityElementsHidden", "boolean");
    nativeProps.setProperty(rt, "accessibilityLabel", "string");
    nativeProps.setProperty(rt, "accessibilityHint", "string");
    nativeProps.setProperty(rt, "accessibilityRole", true);
    nativeProps.setProperty(rt, "accessibilityState", true);
    nativeProps.setProperty(rt, "accessibilityActions", true);
    nativeProps.setProperty(rt, "accessibilityLabelledBy", true);
    nativeProps.setProperty(rt, "aria-hidden", "boolean");
    nativeProps.setProperty(rt, "aria-label", "string");
    nativeProps.setProperty(rt, "aria-labelledby", "string");
    nativeProps.setProperty(rt, "aria-valuemax", "number");
    nativeProps.setProperty(rt, "aria-valuemin", "number");
    nativeProps.setProperty(rt, "aria-valuenow", "number");
    nativeProps.setProperty(rt, "aria-valuetext", "string");
    nativeProps.setProperty(rt, "aria-busy", "boolean");
    nativeProps.setProperty(rt, "aria-disabled", "boolean");
    nativeProps.setProperty(rt, "aria-expanded", "boolean");
    nativeProps.setProperty(rt, "aria-live", "string");
    nativeProps.setProperty(rt, "aria-modal", "boolean");
    nativeProps.setProperty(rt, "aria-selected", "boolean");
    nativeProps.setProperty(rt, "aria-checked", "string");
    nativeProps.setProperty(rt, "role", "string");
    nativeProps.setProperty(rt, "importantForAccessibility", "number");
    nativeProps.setProperty(rt, "needsOffscreenAlphaCompositing", "boolean");
    nativeProps.setProperty(rt, "shouldRasterizeIOS", "boolean");
    return nativeProps;
  }

  virtual facebook::jsi::Object createConstants(facebook::jsi::Runtime& rt) {
    return facebook::jsi::Object(rt);
  }

  virtual facebook::jsi::Object createCommands(facebook::jsi::Runtime& rt) {
    return facebook::jsi::Object(rt);
  }

  virtual facebook::jsi::Object createBubblingEventTypes(
      facebook::jsi::Runtime& rt) {
    return facebook::jsi::Object(rt);
  }

  virtual facebook::jsi::Object createDirectEventTypes(
      facebook::jsi::Runtime& rt) {
    facebook::jsi::Object events(rt);
    events.setProperty(rt, "topLayout", createDirectEvent(rt, "onLayout"));
    return events;
  }

  static facebook::jsi::Object createBubblingCapturedEvent(
      facebook::jsi::Runtime& runtime,
      std::string name) {
    facebook::jsi::Object event(runtime);
    facebook::jsi::Object phasedRegistrationNames(runtime);
    phasedRegistrationNames.setProperty(runtime, "bubbled", name);
    phasedRegistrationNames.setProperty(runtime, "captured", name + "Capture");
    event.setProperty(
        runtime, "phasedRegistrationNames", phasedRegistrationNames);
    return event;
  }

  static facebook::jsi::Object createDirectEvent(
      facebook::jsi::Runtime& runtime,
      std::string name) {
    facebook::jsi::Object event(runtime);
    event.setProperty(runtime, "registrationName", name);
    return event;
  }
};
} // namespace rnoh