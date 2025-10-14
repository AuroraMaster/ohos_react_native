/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOHCorePackage/TurboModules/VibrationTurboModule.h"

using namespace facebook;

namespace rnoh {
VibrationTurboModule::VibrationTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(vibrate, 1),
      ARK_METHOD_METADATA(cancel, 0),
  };
}
} // namespace rnoh
