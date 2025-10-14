/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/view/ViewEventEmitter.h>
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"

namespace rnoh {

enum ViewEventType {
  VIEW_UNSUPPORTED,
  VIEW_ON_CLICK,
};

ViewEventType getViewEventType(std::string const& eventName) {
  if (eventName == "onClick") {
    return ViewEventType::VIEW_ON_CLICK;
  } else {
    return ViewEventType::VIEW_UNSUPPORTED;
  }
}

class ViewEventEmitRequestHandler : public EventEmitRequestHandler {
  void handleEvent(EventEmitRequestHandler::Context const& ctx) override {
    auto eventType = getViewEventType(ctx.eventName);
    if (eventType == ViewEventType::VIEW_UNSUPPORTED) {
      return;
    }

    auto eventEmitter =
        ctx.shadowViewRegistry
            ->getEventEmitter<facebook::react::ViewEventEmitter>(ctx.tag);
    if (eventEmitter == nullptr) {
      return;
    }

    switch (eventType) {
      case ViewEventType::VIEW_ON_CLICK:
        eventEmitter->dispatchEvent(
            "click", [=](facebook::jsi::Runtime& runtime) {
              auto payload = facebook::jsi::Object(runtime);
              return payload;
            });
        break;
      default:
        break;
    }
  }
};

} // namespace rnoh