/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <ReactCommon/TurboModule.h>
#include <glog/logging.h>
#include <memory>
#include "RNOH/ArkTSMessageHub.h"
#include "RNOH/RNInstance.h"

namespace rnoh {

class TurboModule : public facebook::react::TurboModule {
 public:
  struct Context {
    std::shared_ptr<facebook::react::CallInvoker> jsInvoker;
    /**
     * @deprecated Use `safeInstance` instead.
     */
    std::weak_ptr<RNInstance> instance;
    RNInstance::SafeWeak safeInstance;
    std::shared_ptr<ArkTSMessageHub> arkTSMessageHub;
  };

  using Shared = std::shared_ptr<TurboModule>;

  TurboModule(Context ctx, std::string name)
      : facebook::react::TurboModule(name, ctx.jsInvoker) {}

  virtual ~TurboModule();

  void set(
      facebook::jsi::Runtime& rt,
      const facebook::jsi::PropNameID& name,
      const facebook::jsi::Value& value) override {
   DLOG(INFO) << "Turbo Module: " << name_ << "::"
             << "set("
             << "_, \"" << name.utf8(rt) << "\", \""
             << value.toString(rt).utf8(rt) << "\")";
  }
};

} // namespace rnoh