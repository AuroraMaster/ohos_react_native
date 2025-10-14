/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/Performance/NativeTracing.h"
#include "hitrace/trace.h"

using namespace facebook::jsi;
using namespace facebook;

int32_t TRACE_TAG_REACT_APPS = 1 << 17;

void rnoh::setupTracing(jsi::Runtime& runtime) {
  auto nativeTraceIsTracing = Function::createFromHostFunction(
      runtime,
      PropNameID::forAscii(runtime, "nativeTraceIsTracing"),
      0,
      [](jsi::Runtime& runtime,
         const jsi::Value&,
         const jsi::Value* args,
         size_t count) {
#ifdef WITH_HITRACE_SYSTRACE
        return Value(true);
#endif
        return Value(false);
      });

  auto nativeTraceBeginSection = Function::createFromHostFunction(
      runtime,
      PropNameID::forAscii(runtime, "nativeTraceBeginSection"),
      3,
      [](jsi::Runtime& runtime,
         const jsi::Value&,
         const jsi::Value* args,
         size_t count) {       
        if (args[0].getNumber() != TRACE_TAG_REACT_APPS) {
          return Value::undefined();
        }
        auto traceMessage = args[1].asString(runtime).utf8(runtime);
        OH_HiTrace_StartTrace(traceMessage.c_str());       
        return Value::undefined();
      });

  auto nativeTraceEndSection = Function::createFromHostFunction(
      runtime,
      PropNameID::forAscii(runtime, "nativeTraceEndSection"),
      0,
      [](jsi::Runtime& runtime,
         const jsi::Value&,
         const jsi::Value* args,
         size_t count) {        
        if (args[0].getNumber() != TRACE_TAG_REACT_APPS) {
          return Value::undefined();
        }
        OH_HiTrace_FinishTrace();
        return Value::undefined();
      });
  auto nativeTraceBeginAsyncSection = Function::createFromHostFunction(
      runtime,
      PropNameID::forAscii(runtime, "nativeTraceBeginAsyncSection"),
      4,
      [](jsi::Runtime& runtime,
         const jsi::Value&,
         const jsi::Value* args,
         size_t count) {
        if (args[0].getNumber() != TRACE_TAG_REACT_APPS) {
          return Value::undefined();
        }
        auto traceMessage = args[1].asString(runtime).utf8(runtime);
        auto cookie = args[2].getNumber();
        OH_HiTrace_StartAsyncTrace(traceMessage.c_str(), cookie);   
        return Value::undefined();
      });
  auto nativeTraceEndAsyncSection = Function::createFromHostFunction(
      runtime,
      PropNameID::forAscii(runtime, "nativeTraceEndAsyncSection"),
      3,
      [](jsi::Runtime& runtime,
         const jsi::Value&,
         const jsi::Value* args,
         size_t count) {
        if (args[0].getNumber() != TRACE_TAG_REACT_APPS) {
          return Value::undefined();
        }
        auto traceMessage = args[1].asString(runtime).utf8(runtime);
        auto cookie = args[2].getNumber();
        OH_HiTrace_FinishAsyncTrace(traceMessage.c_str(), cookie);    
        return Value::undefined();
      });

  auto nativeTraceCounter = Function::createFromHostFunction(
      runtime,
      PropNameID::forAscii(runtime, "nativeTraceCounter"),
      3,
      [](jsi::Runtime& runtime,
         const jsi::Value&,
         const jsi::Value* args,
         size_t count) {
        if (args[0].getNumber() != TRACE_TAG_REACT_APPS) {
          return Value::undefined();
        }
        auto traceMessage = args[1].asString(runtime).utf8(runtime);
        auto value = args[2].getNumber();
        OH_HiTrace_CountTrace(traceMessage.c_str(), value);   
        return Value::undefined();
      });
  runtime.global().setProperty(
      runtime, "nativeTraceIsTracing", nativeTraceIsTracing);
  runtime.global().setProperty(
      runtime, "nativeTraceBeginSection", nativeTraceBeginSection);
  runtime.global().setProperty(
      runtime, "nativeTraceEndSection", nativeTraceEndSection);
  runtime.global().setProperty(
      runtime, "nativeTraceBeginAsyncSection", nativeTraceBeginAsyncSection);
  runtime.global().setProperty(
      runtime, "nativeTraceEndAsyncSection", nativeTraceEndAsyncSection);
  runtime.global().setProperty(
      runtime, "nativeTraceCounter", nativeTraceCounter);
}
