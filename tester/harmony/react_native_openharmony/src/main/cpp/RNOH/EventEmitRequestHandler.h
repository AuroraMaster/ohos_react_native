/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <napi/native_api.h>
#include <memory>
#include <string>
#include <vector>
#include "RNOH/ShadowViewRegistry.h"
namespace rnoh {

class EventEmitRequestHandler {
 public:
  using Weak = std::weak_ptr<EventEmitRequestHandler>;
  using Shared = std::shared_ptr<EventEmitRequestHandler>;

  struct Context {
    napi_env env;
    facebook::react::Tag tag;
    std::string eventName;
    napi_value payload;
    std::shared_ptr<ShadowViewRegistry> shadowViewRegistry;
  };

  virtual void handleEvent(Context const& ctx) = 0;
};

using EventEmitRequestHandlers = std::vector<EventEmitRequestHandler::Shared>;

} // namespace rnoh