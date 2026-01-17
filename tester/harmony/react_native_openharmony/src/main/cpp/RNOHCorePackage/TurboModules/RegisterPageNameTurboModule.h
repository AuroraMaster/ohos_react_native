/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "RNOH/TurboModule.h"
#include "RNOH/ArkTSMessageHandler.h"

namespace rnoh {
  
class JSI_EXPORT RegisterPageName : public TurboModule {
  public:
    RegisterPageName(const TurboModule::Context ctx, const std::string name);
    static double s_windowId;
    static std::string s_bundleCodeDir;
};

class RegisterPageNameMessageHandler : public ArkTSMessageHandler {
  public:
    void handleArkTSMessage(const Context& ctx) override;
};

}
