/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "SourceCodeTurboModule.h"

using namespace rnoh;
using namespace facebook;

static jsi::Value __hostFunction_SourceCodeTurboModule_getConstants(
    jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const jsi::Value* args,
    size_t count) {
  return static_cast<ArkTSTurboModule&>(turboModule)
      .call(rt, "getConstants", args, count);
}

SourceCodeTurboModule::SourceCodeTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_["getConstants"] =
      MethodMetadata{0, __hostFunction_SourceCodeTurboModule_getConstants};
}