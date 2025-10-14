/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
 
#ifndef JSVM_RUMTIME_COMMON_H
#define JSVM_RUMTIME_COMMON_H
#include <glog/logging.h>
#include "jsi/jsi.h"

namespace rnjsvm {
using Runtime = facebook::jsi::Runtime;
using Value = facebook::jsi::Value;
using Pointer = facebook::jsi::Pointer;
using PropNameID = facebook::jsi::PropNameID;
using Symbol = facebook::jsi::Symbol;
using String = facebook::jsi::String;
using HostObject = facebook::jsi::HostObject;
using BigInt = facebook::jsi::BigInt;
using NativeState = facebook::jsi::NativeState;
using Function = facebook::jsi::Function;
using Object = facebook::jsi::Object;
using HostFunctionType = facebook::jsi::HostFunctionType;
using Buffer = facebook::jsi::Buffer;
using PreparedJavaScript = facebook::jsi::PreparedJavaScript;
using Instrumentation = facebook::jsi::Instrumentation;
using Array = facebook::jsi::Array;
using WeakObject = facebook::jsi::WeakObject;
using ArrayBuffer = facebook::jsi::ArrayBuffer;
using MutableBuffer = facebook::jsi::MutableBuffer;
using JSError = facebook::jsi::JSError;
} // namespace rnjsvm

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define GET_AND_LOG_STRING(status, result)                      \
  {                                                             \
    char str[200] = {0};                                        \
    OH_JSVM_GetValueStringUtf8(env, result, str, 200, nullptr); \
    LOG(INFO) << "call status: " << (uint32_t)(status) << str;  \
  }

#define CALL_JSVM(env, thecall)                                  \
  {                                                              \
    auto JSVM_STATUS = thecall;                                  \
    if (unlikely(JSVM_STATUS != JSVM_OK)) {                      \
      JSVM_Value ERROR_RESULT_JSVM;                              \
      OH_JSVM_GetAndClearLastException(env, &ERROR_RESULT_JSVM); \
    }                                                            \
  }

#define CALL_JSVM_AND_THROW(thecall)    \
  {                                     \
    if (unlikely((thecall) != JSVM_OK)) { \
      DLOG(INFO) << "call failed";      \
      ThrowError();                     \
    }                                   \
  }

#endif