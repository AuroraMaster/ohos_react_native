/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
 
#ifndef JSVM_CONVERTER_H
#define JSVM_CONVERTER_H

#include "ark_runtime/jsvm.h"
#include "common.h"
#include "JSVMRuntime.h"

namespace rnjsvm {
class JSVMConverter{
public:
    static Value JSVMToJsi(JSVM_Env env, const JSVM_Value value);
    
    static Value JsiValueFromBool(JSVM_Env env, const JSVM_Value value) {
        bool result;
        OH_JSVM_GetValueBool(env, value, &result);
        return Value(result);
    }
    
    static Value JsiValueFromNumber(JSVM_Env env, const JSVM_Value value) {
        double result;
        OH_JSVM_GetValueDouble(env, value, &result);
        return Value(result);
    }
    
    template<typename T>
    static Value JsiValueFromPointerValue(JSVM_Env env, const JSVM_Value value) {
        return JSVMRuntime::make<T>(JSVMPointerValue::New<false>(env, value));
    }

    template<typename T>
    static JSVM_Value PointerValueToJSVM(JSVM_Env env, const T &pointer) {
        return static_cast<const JSVMPointerValue *>(JSVMRuntime::getPointerValue(pointer))->GetValue();
    }

    template <typename T>
    static T make(JSVM_Env env, JSVM_Value value) {
      if (!value) {
        OH_JSVM_GetUndefined(env, &value);
      }
      return JSVMRuntime::make<T>(JSVMPointerValue::New<false>(env, value));
    }

    static JSVM_Value JsiToJSVM(JSVM_Env env, const Value& value);
};

} // namespace rnjsvm

#endif