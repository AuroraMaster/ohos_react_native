/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "MarqueeViewEventEmitter.h"

namespace facebook {
namespace react {

void MarqueeViewEventEmitter::onStop(OnStop event) const {
    dispatchEvent("stop", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "isStop", event.isStop);
        payload.setProperty(runtime, "type", event.type);
        return payload;
    });
}

} // namespace react
} // namespace facebook
