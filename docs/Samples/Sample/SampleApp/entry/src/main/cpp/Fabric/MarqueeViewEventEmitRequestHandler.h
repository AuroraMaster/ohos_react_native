/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <napi/native_api.h>
#include "MarqueeViewEventEmitter.h"
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"

using namespace facebook;
namespace rnoh {

    enum MarqueeViewEventType {
        MARQUEE_VIEW_ON_STOP = 0,
    };

    MarqueeViewEventType getMarqueeViewEventType(ArkJS &arkJs, napi_value eventObject, std::string eventName) {
        auto eventType = eventName;
        LOG(INFO) << "getMarqueeViewEventType = " + eventType;
        if (eventType == "onStop") {
            return MarqueeViewEventType::MARQUEE_VIEW_ON_STOP;
        } else {
            throw std::runtime_error("Unknown Page event type");
        }
    }

    class MarqueeViewEventEmitRequestHandler : public EventEmitRequestHandler {
        public:
        void handleEvent(EventEmitRequestHandler::Context const &ctx) override {
            ArkJS arkJs(ctx.env);
            auto eventEmitter = ctx.shadowViewRegistry->getEventEmitter<react::MarqueeViewEventEmitter>(ctx.tag);
            if (eventEmitter == nullptr) {
                return;
            }

            MarqueeViewEventType type = getMarqueeViewEventType(arkJs, ctx.payload, ctx.eventName);
            switch (type) {
            case MarqueeViewEventType::MARQUEE_VIEW_ON_STOP: {
                bool isStop = (bool)arkJs.getBoolean(arkJs.getObjectProperty(ctx.payload, "isStop"));
                std::string type = arkJs.getString(arkJs.getObjectProperty(ctx.payload, "type"));
                LOG(INFO) << "MarqueeViewEventEmitter OnStop " << isStop;
                LOG(INFO) << "MarqueeViewEventEmitter OnStop " << type;

                react::MarqueeViewEventEmitter::OnStop event = {isStop, type};
                eventEmitter->onStop(event);
                break;
            }
            default:
                break;
            }
        };
    };
} // namespace rnoh