/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh {
    class MarqueeViewJSIBinder : public ViewComponentJSIBinder {
        facebook::jsi::Object createNativeProps(facebook::jsi::Runtime &rt) override {
            auto object = ViewComponentJSIBinder::createNativeProps(rt);
            object.setProperty(rt, "src", "string");
            return object;
        }
    
        facebook::jsi::Object createDirectEventTypes(facebook::jsi::Runtime &rt) override {
            facebook::jsi::Object events(rt);
        
            events.setProperty(rt, "topStop", createDirectEvent(rt, "onStop"));
            return events;
        }
    };
} // namespace rnoh