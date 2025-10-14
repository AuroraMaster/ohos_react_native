/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <vector>
#include "RNOH/TurboModuleFactory.h"
#include "jsi/jsi.h"

namespace rnoh {
class GlobalJSIBinder {
 public:
  struct Context {};
  GlobalJSIBinder(Context ctx) : m_ctx(ctx) {}

  virtual void createBindings(
      facebook::jsi::Runtime&,
      std::shared_ptr<TurboModuleProvider> provider) = 0;

 private:
  Context m_ctx;
};

using GlobalJSIBinders = std::vector<std::shared_ptr<GlobalJSIBinder>>;

} // namespace rnoh