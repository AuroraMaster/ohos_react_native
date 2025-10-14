/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ToastAndroidTurboModule.h"
#include "RNOH/ArkTSTurboModule.h"

using namespace facebook;

namespace rnoh {
ToastAndroidTurboModule::ToastAndroidTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(getConstants, 0),
      ARK_METHOD_METADATA(show, 2),
      ARK_METHOD_METADATA(showWithGravity, 3),
      ARK_METHOD_METADATA(showWithGravityAndOffset, 5),
  };
}
} // namespace rnoh
