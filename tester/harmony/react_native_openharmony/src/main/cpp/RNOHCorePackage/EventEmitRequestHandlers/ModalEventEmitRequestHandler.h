/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <glog/logging.h>
#include <react/renderer/components/rncore/EventEmitters.h>

#include "RNOH/EventEmitRequestHandler.h"

namespace rnoh {

class ModalEventEmitRequestHandler : public EventEmitRequestHandler {
 public:
  void handleEvent(EventEmitRequestHandler::Context const& ctx) override {
    auto eventName = ctx.eventName;
    auto eventEmitter =
        ctx.shadowViewRegistry
            ->getEventEmitter<facebook::react::ModalHostViewEventEmitter>(
                ctx.tag);
    if (eventEmitter == nullptr) {
      return;
    }

    if (eventName == "onShow") {
      eventEmitter->onShow({});
    } else if (eventName == "onDismiss") {
      eventEmitter->onDismiss({});
    } else if (eventName == "onRequestClose") {
      eventEmitter->onRequestClose({});
    } else {
      LOG(ERROR) << "Unsupported modal event name: " << eventName;
    }
  }
};

} // namespace rnoh
