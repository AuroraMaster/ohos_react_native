/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "LinkingManagerTurboModule.h"

namespace rnoh {
using namespace facebook;

LinkingManagerTurboModule::LinkingManagerTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_ASYNC_METHOD_METADATA(getInitialURL, 0),
      ARK_ASYNC_METHOD_METADATA(canOpenURL, 1),
      ARK_ASYNC_METHOD_METADATA(openURL, 1),
      ARK_ASYNC_METHOD_METADATA(openSettings, 0)};
}

} // namespace rnoh