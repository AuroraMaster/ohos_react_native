/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ButtonViewEventEmitter.h"

namespace facebook {
namespace react {

void ButtonViewEventEmitter::onButtonClick(OnButtonClick event) const {
    dispatchEvent("topButtonClick", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "isButtonClick", event.isButtonClick);
        payload.setProperty(runtime, "type", event.type);
        return payload;
    });
}

} // namespace react
} // namespace facebook
