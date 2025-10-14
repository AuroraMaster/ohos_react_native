/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOHCorePackage/TurboModules/AppearanceTurboModule.h"

namespace rnoh {
using namespace facebook;

AppearanceTurboModule::AppearanceTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(getColorScheme, 0),
      ARK_METHOD_METADATA(setColorScheme, 1),
  };
}

} // namespace rnoh