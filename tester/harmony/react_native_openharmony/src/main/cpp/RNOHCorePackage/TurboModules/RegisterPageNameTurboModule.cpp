/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RegisterPageNameTurboModule.h"
#include <dlfcn.h>
#include "AbilityKit/ability_runtime/application_context.h"
#include "RNOH/ApiVersionCheck.h"
#include "RNOH/ArkTSMessageHandler.h"
#include "RNOH/RNInstance.h"
#include "napi/native_api.h"

namespace rnoh {
using namespace facebook;

double RegisterPageName::s_windowId = 0;
std::string RegisterPageName::s_bundleCodeDir;

void RegisterPageNameMessageHandler::handleArkTSMessage(
    const ArkTSMessageHandler::Context& ctx) {
  if (ctx.messageName == "WINDOW_ID") {
    RegisterPageName::s_windowId = ctx.messagePayload.asDouble();
  }
  if (ctx.messageName == "BUNDLE_CODE_DIR") {
    RegisterPageName::s_bundleCodeDir = ctx.messagePayload.asString();
  }
}

RegisterPageName::RegisterPageName(
    const TurboModule::Context ctx,
    const std::string name)
    : TurboModule(ctx, name) {
  methodMap_["sendPageName"] = MethodMetadata{
      1,
      [](jsi::Runtime& rt,
         react::TurboModule& turboModule,
         const jsi::Value* args,
         size_t count) -> jsi::Value {
        constexpr int API_LEVEL_23 = 23;
        if (OH_GetSdkApiVersion() < API_LEVEL_23) {
          return jsi::Value::undefined();
        }
        if (count > 0 && args[0].isString()) {
          std::string param = RegisterPageName::s_bundleCodeDir + "/" +
              args[0].asString(rt).utf8(rt);
          const char* pageName = param.c_str();
          int32_t pageNameLen = param.length();
          int32_t windowId = RegisterPageName::s_windowId;
          using NotifyPageChangedFunc =
              int32_t (*)(const char*, int32_t, int32_t);
          auto notifyPageChanged = (NotifyPageChangedFunc)dlsym(
              RTLD_DEFAULT,
              "OH_AbilityRuntime_ApplicationContextNotifyPageChanged");
          if (notifyPageChanged != nullptr) {
            int32_t result = notifyPageChanged(pageName, pageNameLen, windowId);
            return jsi::String::createFromUtf8(rt, std::to_string(result));
          } else {
            LOG(ERROR) << "DPI scaling capability is not supported.";
          }
        }
      }};
}

} // namespace rnoh