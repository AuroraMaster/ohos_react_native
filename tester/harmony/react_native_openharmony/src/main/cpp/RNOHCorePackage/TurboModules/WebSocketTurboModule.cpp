/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "WebSocketTurboModule.h"

namespace rnoh {
using namespace facebook;

WebSocketTurboModule::WebSocketTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(connect, 4),
      ARK_METHOD_METADATA(send, 2),
      ARK_METHOD_METADATA(sendBinary, 2),
      ARK_METHOD_METADATA(ping, 1),
      ARK_METHOD_METADATA(close, 3),
      // event emitters
      ARK_METHOD_METADATA(addListener, 1),
      ARK_METHOD_METADATA(removeListeners, 1),
  };
}

} // namespace rnoh
