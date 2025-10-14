/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "NetworkingTurboModule.h"

namespace rnoh {

using namespace facebook;

NetworkingTurboModule::NetworkingTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_ASYNC_METHOD_METADATA(sendRequest, 2),
      ARK_METHOD_METADATA(abortRequest, 1),
      ARK_ASYNC_METHOD_METADATA(clearCookies, 1),
  };
}

} // namespace rnoh
