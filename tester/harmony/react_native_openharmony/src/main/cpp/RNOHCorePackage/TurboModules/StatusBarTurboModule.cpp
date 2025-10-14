/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "StatusBarTurboModule.h"

namespace rnoh {
using namespace facebook;

StatusBarTurboModule::StatusBarTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(getConstants, 0),
      ARK_METHOD_METADATA(setColor, 1),
      ARK_METHOD_METADATA(setHidden, 1),
      ARK_ASYNC_METHOD_METADATA(setStyle, 1),
      ARK_METHOD_METADATA(setTranslucent, 1),
  };
}

} // namespace rnoh