/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "RNOH/ArkTSTurboModule.h"

namespace rnoh {

class JSI_EXPORT SafeAreaTurboModule : public ArkTSTurboModule {
 public:
  SafeAreaTurboModule(
      const ArkTSTurboModule::Context ctx,
      const std::string name);
};

} // namespace rnoh