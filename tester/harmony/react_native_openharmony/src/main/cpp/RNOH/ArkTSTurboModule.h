/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <ReactCommon/TurboModuleUtils.h>
#include <jsi/JSIDynamic.h>
#include <jsi/jsi.h>
#include <react/renderer/scheduler/Scheduler.h>
#include "TaskExecutor/TaskExecutor.h"

#include "ArkJS.h"
#include "RNOH/EventDispatcher.h"
#include "RNOH/MessageQueueThread.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "RNOH/TurboModule.h"

/**
 * @internal
 * Used by codegen.
 */
#define ARK_METHOD_CALLER(name)                        \
  [](facebook::jsi::Runtime& rt,                       \
     facebook::react::TurboModule& turboModule,        \
     const facebook::jsi::Value* args,                 \
     size_t count) {                                   \
    return static_cast<ArkTSTurboModule&>(turboModule) \
        .call(rt, #name, args, count);                 \
  }

/**
 * @internal
 * Used by codegen.
 */
#define ARK_ASYNC_METHOD_CALLER(name)                  \
  [](facebook::jsi::Runtime& rt,                       \
     facebook::react::TurboModule& turboModule,        \
     const facebook::jsi::Value* args,                 \
     size_t count) {                                   \
    return static_cast<ArkTSTurboModule&>(turboModule) \
        .callAsync(rt, #name, args, count);            \
  }

/**
 * @internal
 * Used by codegen.
 */
#define ARK_SCHEDULE_METHOD_CALLER(name)        \
  [](facebook::jsi::Runtime& rt,                \
     facebook::react::TurboModule& turboModule, \
     const facebook::jsi::Value* args,          \
     size_t count) {                            \
    static_cast<ArkTSTurboModule&>(turboModule) \
        .scheduleCall(rt, #name, args, count);  \
    return facebook::jsi::Value::undefined();   \
  }

/**
 * @internal
 * Used by codegen.
 */
#define ARK_METHOD_METADATA(name, argc)      \
  {                                          \
#name, { argc, ARK_METHOD_CALLER(name) } \
  }

/**
 * @internal
 * Used by codegen.
 */
#define ARK_ASYNC_METHOD_METADATA(name, argc)      \
  {                                                \
#name, { argc, ARK_ASYNC_METHOD_CALLER(name) } \
  }

/**
 * @internal
 * Used by codegen.
 */
#define ARK_SCHEDULE_METHOD_METADATA(name, argc)      \
  {                                                   \
#name, { argc, ARK_SCHEDULE_METHOD_CALLER(name) } \
  }

namespace rnoh {

/**
 * @internal
 * Used by codegen.
 */
class ArkTSTurboModule : public TurboModule {
 public:
  struct Context : public TurboModule::Context {
    napi_env env;
    NapiRef arkTSTurboModuleInstanceRef;
    TaskThread turboModuleThread;
    TaskExecutor::Shared taskExecutor;
    std::shared_ptr<EventDispatcher> eventDispatcher;
    std::shared_ptr<MessageQueueThread> jsQueue;
    std::shared_ptr<facebook::react::Scheduler> scheduler;
  };

  ArkTSTurboModule(Context ctx, std::string name);
  ~ArkTSTurboModule() noexcept override;

  facebook::jsi::Value call(
      facebook::jsi::Runtime& runtime,
      const std::string& methodName,
      const facebook::jsi::Value* args,
      size_t argsCount);

  folly::dynamic callSync(
      const std::string& methodName,
      std::vector<ArkJS::IntermediaryArg> args);

  void scheduleCall(
      facebook::jsi::Runtime& runtime,
      const std::string& methodName,
      const facebook::jsi::Value* args,
      size_t argsCount);

  facebook::jsi::Value callAsync(
      facebook::jsi::Runtime& runtime,
      const std::string& methodName,
      const facebook::jsi::Value* args,
      size_t argsCount);

  static std::vector<ArkJS::IntermediaryArg>
  convertJSIValuesToIntermediaryValues(
      facebook::jsi::Runtime& runtime,
      std::shared_ptr<facebook::react::CallInvoker> jsInvoker,
      const facebook::jsi::Value* jsiArgs,
      size_t argsCount);
  
  Context getContext() {
    return m_ctx;    
  }

 protected:
  Context m_ctx;
};
} // namespace rnoh
