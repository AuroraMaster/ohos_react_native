/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
 
#include "JSVMConverter.h"

namespace rnjsvm {
Value JSVMConverter::JSVMToJsi(JSVM_Env env, const JSVM_Value value) {
    if (!value) {
        return {};
    }
    JSVM_ValueType type;
    OH_JSVM_Typeof(env, value, &type);
    switch(type) {
        case JSVM_UNDEFINED:
            return Value::undefined();
        case JSVM_NULL:
            return Value::null();
        case JSVM_BOOLEAN:
            return JsiValueFromBool(env, value);
        case JSVM_NUMBER:
            return JsiValueFromNumber(env, value);
        case JSVM_STRING:
            return JsiValueFromPointerValue<String>(env, value);
        case JSVM_SYMBOL:
            return JsiValueFromPointerValue<Symbol>(env, value);
        case JSVM_OBJECT:
        case JSVM_FUNCTION:
        case JSVM_EXTERNAL:
            return JsiValueFromPointerValue<Object>(env, value);
        case JSVM_BIGINT:
            return JsiValueFromPointerValue<BigInt>(env, value);
        default:
            return Value::undefined();
    }
}

JSVM_Value JSVMConverter::JsiToJSVM(JSVM_Env env, const Value& value) {
    JSVM_Value result;

    if (value.isUndefined()) {
        OH_JSVM_GetUndefined(env, &result);
    } else if (value.isNull()) {
        OH_JSVM_GetNull(env, &result);
    } else if (value.isBool()) {
        OH_JSVM_GetBoolean(env, value.getBool(), &result);
    } else if (value.isNumber()) {
        OH_JSVM_CreateDouble(env, value.getNumber(), &result);
    } else {
        auto *pointerValue = JSVMRuntime::getPointerValue(value);
        result = static_cast<const JSVMPointerValue *>(pointerValue)->GetValue();
    }

    return result;
}
}