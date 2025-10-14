/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <napi/native_api.h>
#include <react/renderer/components/view/TouchEvent.h>
#include <unordered_map>
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"

namespace rnoh {

enum TouchType { DOWN, UP, MOVE, CANCEL };

class TouchEventEmitRequestHandler : public EventEmitRequestHandler {
 public:
  void handleEvent(TouchEventEmitRequestHandler::Context const& ctx) override;

 private:
  facebook::react::Touch convertTouchObject(
      ArkJS& arkJs,
      napi_value touchObject);

  facebook::react::Touches convertTouches(
      ArkJS& arkJs,
      facebook::react::Tag tag,
      facebook::react::Float timestamp,
      napi_value touchArray);
};

} // namespace rnoh