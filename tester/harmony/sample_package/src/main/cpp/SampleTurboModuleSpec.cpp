/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// NOTE: This entire file should be codegen'ed.

#include "SampleTurboModuleSpec.h"

using namespace rnoh;
using namespace facebook;

static jsi::Value
__hostFunction_NativeSampleTurboCxxModuleSpecJSI_displayRNOHError(
    jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const jsi::Value* args,
    size_t count) {
  return jsi::Value(static_cast<ArkTSTurboModule&>(turboModule)
                        .call(rt, "displayRNOHError", args, count));
}

static jsi::Value
__hostFunction_NativeSampleTurboCxxModuleSpecJSI_throwExceptionCpp(
    jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const jsi::Value* args,
    size_t count) {
  throw std::runtime_error("Exception thrown from C++");
}

static jsi::Value
__hostFunction_NativeSampleTurboCxxModuleSpecJSI_throwExceptionArk(
    jsi::Runtime& rt,
    react::TurboModule& turboModule,
    const jsi::Value* args,
    size_t count) {
  return jsi::Value(static_cast<ArkTSTurboModule&>(turboModule)
                        .call(rt, "throwExceptionArk", args, count));
}

NativeSampleTurboModuleSpecJSI::NativeSampleTurboModuleSpecJSI(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
    methodMap_ = {
        ARK_METHOD_METADATA(voidFunc, 0),
        ARK_METHOD_METADATA(getBool, 1),
        ARK_METHOD_METADATA(getString, 1),
        ARK_METHOD_METADATA(getObject, 1),
        ARK_METHOD_METADATA(registerFunction, 1),
        ARK_ASYNC_METHOD_METADATA(doAsyncJob, 1),
        ARK_ASYNC_METHOD_METADATA(getPromisedArray, 0),
        ARK_METHOD_METADATA(getUnionValue, 1),
        ARK_METHOD_METADATA(getEnum, 3),
        ARK_METHOD_METADATA(getUnknown, 1),
        ARK_METHOD_METADATA(getRootTag, 1),
        ARK_METHOD_METADATA(getNumber, 1),
        ARK_METHOD_METADATA(getValue, 3),
        ARK_METHOD_METADATA(getValueWithCallback, 1),
        ARK_ASYNC_METHOD_METADATA(getValueWithPromise, 1),
        ARK_METHOD_METADATA(getUnsafeObject, 1),
        ARK_METHOD_METADATA(getConstants, 0),
        ARK_METHOD_METADATA(getNull, 1),
        ARK_METHOD_METADATA(getArray, 1),
    };

  methodMap_["displayRNOHError"] = MethodMetadata{
      1, __hostFunction_NativeSampleTurboCxxModuleSpecJSI_displayRNOHError};
  methodMap_["throwExceptionCpp"] = MethodMetadata{
      0, __hostFunction_NativeSampleTurboCxxModuleSpecJSI_throwExceptionCpp};
  methodMap_["throwExceptionArk"] = MethodMetadata{
      0, __hostFunction_NativeSampleTurboCxxModuleSpecJSI_throwExceptionArk};
}