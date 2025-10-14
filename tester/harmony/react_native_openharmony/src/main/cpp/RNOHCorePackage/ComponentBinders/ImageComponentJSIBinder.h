/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh {
class ImageComponentJSIBinder : public ViewComponentJSIBinder {
  facebook::jsi::Object createNativeProps(facebook::jsi::Runtime& rt) override {
    auto object = ViewComponentJSIBinder::createNativeProps(rt);
    object.setProperty(rt, "source", "array");
    object.setProperty(rt, "alt", "string");
    object.setProperty(rt, "resizeMode", "string");
    object.setProperty(rt, "resizeMethod", "string");
    object.setProperty(rt, "blurRadius", "number");
    object.setProperty(rt, "defaultSource", "RCTImageSource");
    object.setProperty(rt, "loadingIndicatorSource", "RCTImageSource");
    object.setProperty(rt, "fadeDuration", "number");
    object.setProperty(rt, "capInsets", "number");
    object.setProperty(rt, "tintColor", "Color");
    return object;
  }
  facebook::jsi::Object createDirectEventTypes(
      facebook::jsi::Runtime& rt) override {
    facebook::jsi::Object events =
        ViewComponentJSIBinder::createDirectEventTypes(rt);
    events.setProperty(
        rt, "topLoadStart", createDirectEvent(rt, "onLoadStart"));
    events.setProperty(rt, "topLoad", createDirectEvent(rt, "onLoad"));
    events.setProperty(rt, "topError", createDirectEvent(rt, "onError"));
    events.setProperty(rt, "topLoadEnd", createDirectEvent(rt, "onLoadEnd"));
    events.setProperty(rt, "topProgress", createDirectEvent(rt, "onProgress"));
    return events;
  }
};
} // namespace rnoh
