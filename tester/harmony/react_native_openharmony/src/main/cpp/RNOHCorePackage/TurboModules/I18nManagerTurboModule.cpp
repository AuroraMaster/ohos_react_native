/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOHCorePackage/TurboModules/I18nManagerTurboModule.h"

using namespace rnoh;
using namespace facebook;

I18nManagerTurboModule::I18nManagerTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(getConstants, 0),
      ARK_SCHEDULE_METHOD_METADATA(allowRTL, 1),
      ARK_SCHEDULE_METHOD_METADATA(forceRTL, 1)};
}