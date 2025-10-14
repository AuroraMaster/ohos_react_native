/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOHCorePackage/TurboModules/DevLoadingViewTurboModule.h"

using namespace facebook;

namespace rnoh {
DevLoadingViewTurboModule::DevLoadingViewTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(showMessage, 3),
      ARK_METHOD_METADATA(hide, 0),
  };
}
} // namespace rnoh
