/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <ReactCommon/TurboModule.h>
#include "glog/logging.h"

namespace rnoh {
class StubModule : public facebook::react::TurboModule {
 public:
  StubModule(
      std::string const& name,
      std::shared_ptr<facebook::react::CallInvoker> jsInvoker)
      : facebook::react::TurboModule(name, jsInvoker) {}

  facebook::jsi::Value get(
      facebook::jsi::Runtime& runtime,
      const facebook::jsi::PropNameID& propName) override {
    DLOG(INFO) << "StubModule " << name_ << " get " << propName.utf8(runtime)
              << "\n";
    return this->TurboModule::get(runtime, propName);
  }

  void set(
      facebook::jsi::Runtime& runtime,
      const facebook::jsi::PropNameID& name,
      const facebook::jsi::Value& value) override {
    DLOG(INFO) << "StubModule " << name_ << " set " << name.utf8(runtime) << " "
              << value.toString(runtime).utf8(runtime) << "\n";
  }
};
} // namespace rnoh
