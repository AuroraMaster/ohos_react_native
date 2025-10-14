/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh {

class TextInputComponentJSIBinder : public ViewComponentJSIBinder {
  facebook::jsi::Object createNativeProps(facebook::jsi::Runtime& rt) override {
    auto nativeProps = ViewComponentJSIBinder::createNativeProps(rt);
    nativeProps.setProperty(rt, "text", "string");
    nativeProps.setProperty(rt, "editable", "boolean");
    nativeProps.setProperty(rt, "multiline", "boolean");
    nativeProps.setProperty(rt, "placeholder", "string");
    nativeProps.setProperty(rt, "placeholderTextColor", "Color");
    nativeProps.setProperty(rt, "cursorColor", "Color");
    nativeProps.setProperty(rt, "caretHidden", "boolean");
    nativeProps.setProperty(rt, "secureTextEntry", "boolean");
    nativeProps.setProperty(rt, "maxLength", "number");
    nativeProps.setProperty(rt, "selectionColor", "Color");
    nativeProps.setProperty(rt, "returnKeyType", "ReturnKeyType");
    nativeProps.setProperty(rt, "textAlign", "string");
    nativeProps.setProperty(rt, "autoFocus", "boolean");
    nativeProps.setProperty(rt, "keyboardType", "KeyboardType");
    nativeProps.setProperty(rt, "allowFontScaling", "boolean");
    nativeProps.setProperty(rt, "clearTextOnFocus", "boolean");
    nativeProps.setProperty(rt, "mostRecentEventCount", "number");
    nativeProps.setProperty(rt, "clearButtonMode", "string");
    nativeProps.setProperty(rt, "defaultValue", "string");
    nativeProps.setProperty(rt, "selectTextOnFocus", "boolean");
    nativeProps.setProperty(rt, "contextMenuHidden", "boolean");
    nativeProps.setProperty(rt, "passwordRules", "string");
    nativeProps.setProperty(rt, "underlineColorAndroid", "Color");
    nativeProps.setProperty(rt, "returnKeyLabel", "string");
    nativeProps.setProperty(rt, "textContentType", "string");
    nativeProps.setProperty(rt, "blurOnSubmit", "boolean");
    nativeProps.setProperty(rt, "showSoftInputOnFocus", "boolean");
    nativeProps.setProperty(rt, "textAlignVertical", "string");
    return nativeProps;
  };
  facebook::jsi::Object createDirectEventTypes(
      facebook::jsi::Runtime& rt) override {
    facebook::jsi::Object events =
        ViewComponentJSIBinder::createDirectEventTypes(rt);

    events.setProperty(rt, "topChange", createDirectEvent(rt, "onChange"));
    events.setProperty(
        rt, "topSubmitEditing", createDirectEvent(rt, "onSubmitEditing"));
    events.setProperty(
        rt, "topEndEditing", createDirectEvent(rt, "onEndEditing"));
    events.setProperty(rt, "topKeyPress", createDirectEvent(rt, "onKeyPress"));
    events.setProperty(
        rt, "topSelectionChange", createDirectEvent(rt, "onSelectionChange"));
    events.setProperty(
        rt, "topContentSizeChange", createDirectEvent(rt, "onContentSizeChange"));
    events.setProperty(
        rt, "topScroll", createDirectEvent(rt, "onScroll"));
    return events;
  }
};

} // namespace rnoh