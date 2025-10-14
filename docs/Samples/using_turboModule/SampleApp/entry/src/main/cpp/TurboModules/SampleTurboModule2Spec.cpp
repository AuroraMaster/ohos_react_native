/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// NOTE: This entire file should be codegen'ed.

#include "SampleTurboModule2Spec.h"

using namespace rnoh;
using namespace facebook;

static jsi::Value __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_test(jsi::Runtime &rt,
                                                                           react::TurboModule &turboModule,
                                                                           const jsi::Value *args, size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "test", args, count));
}

static jsi::Value __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_eatFruit(jsi::Runtime &rt,
                                                                           react::TurboModule &turboModule,
                                                                           const jsi::Value *args, size_t count) {
    auto self = static_cast<NativeSampleTurboModule2SpecJSI*>(&turboModule);
    self->callJS();
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "eatFruit", args, count));
}

static jsi::Value __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_callFunction(jsi::Runtime &rt,
                                                                           react::TurboModule &turboModule,
                                                                           const jsi::Value *args, size_t count) {
    auto self = static_cast<NativeSampleTurboModule2SpecJSI*>(&turboModule);
    self->callJS();
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "callFunction", args, count));
}

static jsi::Value __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_checkPwd(jsi::Runtime &rt,
                                                                               react::TurboModule &turboModule,
                                                                               const jsi::Value *args, size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "checkPwd", args, count));
}

static jsi::Value __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_getRequest(jsi::Runtime &rt,
                                                                               react::TurboModule &turboModule,
                                                                               const jsi::Value *args, size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).callAsync(rt, "getRequest", args, count));  // 异步方法使用callAsync
}

static jsi::Value __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_getObject(jsi::Runtime &rt,
                                                                               react::TurboModule &turboModule,
                                                                               const jsi::Value *args, size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getObject", args, count));
}

void NativeSampleTurboModule2SpecJSI::callJS() {
    if (auto instance = m_ctx.instance.lock(); instance != nullptr) {
        std::string module = "RCTDeviceEventEmitter";
        std::string method = "emit";
        folly::dynamic data = folly::dynamic::array;
        data.push_back("callFunction");
        
        folly::dynamic paramsObj = folly::dynamic::object("size", 20);
        folly::dynamic obj = folly::dynamic::object("params", paramsObj);
        data.push_back(obj);
        instance->callJSFunction(std::move(module), std::move(method), std::move(data));
    }
}

NativeSampleTurboModule2SpecJSI::NativeSampleTurboModule2SpecJSI(const ArkTSTurboModule::Context ctx,
                                                                 const std::string name)
    : ArkTSTurboModule(ctx, name) {
    methodMap_["test"] = MethodMetadata{0, __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_test};
    methodMap_["eatFruit"] = MethodMetadata{1, __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_eatFruit};
    methodMap_["checkPwd"] = MethodMetadata{3, __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_checkPwd};
    methodMap_["getObject"] = MethodMetadata{0, __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_getObject};
    methodMap_["getRequest"] = MethodMetadata{0, __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_getRequest};
    methodMap_["callFunction"] = MethodMetadata{1, __hostFunction_NativeSampleTurboCxxxxModuleSpec2JSI_callFunction};
}
