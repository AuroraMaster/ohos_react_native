/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/rncore/EventEmitters.h>
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"

namespace rnoh {

facebook::react::SwitchEventEmitter::OnChange convertSwitchEvent(
    ArkJS& arkJs,
    napi_value eventObject) {
  auto value = arkJs.getBoolean(arkJs.getObjectProperty(eventObject, "value"));
  auto target =
      arkJs.getInteger(arkJs.getObjectProperty(eventObject, "target"));

  return {value, target};
}

class SwitchEventEmitRequestHandler : public EventEmitRequestHandler {
  void handleEvent(EventEmitRequestHandler::Context const& ctx) override {
    if (ctx.eventName != "onChange") {
      return;
    }

    auto eventEmitter =
        ctx.shadowViewRegistry
            ->getEventEmitter<facebook::react::SwitchEventEmitter>(ctx.tag);
    if (eventEmitter == nullptr) {
      return;
    }

    ArkJS arkJs(ctx.env);
    auto event = convertSwitchEvent(arkJs, ctx.payload);
    eventEmitter->onChange(event);
  }
};

} // namespace rnoh