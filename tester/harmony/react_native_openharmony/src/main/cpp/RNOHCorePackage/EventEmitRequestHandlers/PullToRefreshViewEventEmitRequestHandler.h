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

class PullToRefreshViewEventEmitRequestHandler
    : public EventEmitRequestHandler {
  void handleEvent(EventEmitRequestHandler::Context const& ctx) override {
    if (ctx.eventName != "refresh") {
      return;
    }

    auto eventEmitter =
        ctx.shadowViewRegistry
            ->getEventEmitter<facebook::react::PullToRefreshViewEventEmitter>(
                ctx.tag);
    if (eventEmitter == nullptr) {
      return;
    }

    eventEmitter->onRefresh({});
  }
};

} // namespace rnoh