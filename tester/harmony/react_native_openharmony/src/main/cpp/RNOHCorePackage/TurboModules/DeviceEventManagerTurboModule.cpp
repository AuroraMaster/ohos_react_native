/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOHCorePackage/TurboModules/DeviceEventManagerTurboModule.h"

namespace rnoh {
using namespace facebook;

DeviceEventManagerTurboModule::DeviceEventManagerTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(invokeDefaultBackPressHandler, 0),
  };
}

} // namespace rnoh