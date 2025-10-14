/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh {

class TextComponentJSIBinder : public ViewComponentJSIBinder {
  facebook::jsi::Object createNativeProps(facebook::jsi::Runtime& rt) override {
    auto nativeProps = ViewComponentJSIBinder::createNativeProps(rt);
    nativeProps.setProperty(rt, "selectionColor", "Color");
    nativeProps.setProperty(rt, "dataDetectorType", "string");
    nativeProps.setProperty(rt, "textDecorationStyle", "TextDecorationStyle");
    return nativeProps;
  };
};

} // namespace rnoh