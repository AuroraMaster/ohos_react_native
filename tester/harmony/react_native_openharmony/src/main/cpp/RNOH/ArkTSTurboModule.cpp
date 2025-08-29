/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include <ReactCommon/CallbackWrapper.h>
#include <ReactCommon/TurboModuleUtils.h>
#include <glog/logging.h>
#include <jsi/JSIDynamic.h>
#include <react/renderer/debug/SystraceSection.h>
#include <exception>

#include "ArkTSTurboModule.h"
#include "RNOH/JsiConversions.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"

using namespace rnoh;
using namespace facebook;

using IntermediaryArg = ArkJS::IntermediaryArg;
using IntermediaryCallback = ArkJS::IntermediaryCallback;

IntermediaryCallback createIntermediaryCallback(
    std::weak_ptr<react::CallbackWrapper> weakCallback,
    std::shared_ptr<react::CallInvoker> const& jsInvoker);
const std::vector<facebook::jsi::Value> convertDynamicsToJSIValues(
    facebook::jsi::Runtime& rt,
    const std::vector<folly::dynamic>& dynamics);
facebook::jsi::Value preparePromiseResolverResult(
    facebook::jsi::Runtime& rt,
    const std::vector<folly::dynamic> args);
std::string preparePromiseRejectionResult(
    const std::vector<folly::dynamic> args);

TurboModule::~TurboModule() {};

ArkTSTurboModule::ArkTSTurboModule(Context ctx, std::string name)
    : m_ctx(ctx), TurboModule(ctx, name) {}

ArkTSTurboModule::~ArkTSTurboModule() noexcept {
  auto taskExecutor = m_ctx.taskExecutor;
  taskExecutor->runTask(
      m_ctx.turboModuleThread,
      [ref = std::move(m_ctx.arkTSTurboModuleInstanceRef)] {});
}

// calls a TurboModule method and blocks until it returns, returning its result
jsi::Value ArkTSTurboModule::call(
    jsi::Runtime& runtime,
    const std::string& methodName,
    const jsi::Value* jsiArgs,
    size_t argsCount) {
  react::SystraceSection s(std::string(
                               "#RNOH::ArkTSTurboModule::call (" + this->name_ +
                               "::" + methodName + ")")
                               .c_str());
  auto args = convertJSIValuesToIntermediaryValues(
      runtime, m_ctx.jsInvoker, jsiArgs, argsCount);
  return jsi::valueFromDynamic(runtime, callSync(methodName, std::move(args)));
}

// the cpp side calls a ArkTs TurboModule method and blocks until it returns,
// returning its result
folly::dynamic ArkTSTurboModule::callSync(
    const std::string& methodName,
    std::vector<IntermediaryArg> args) {
  react::SystraceSection s(std::string(
                               "#RNOH::ArkTSTurboModule::callSync (" +
                               this->name_ + "::" + methodName + ")")
                               .c_str());
  auto start = std::chrono::high_resolution_clock::now();

  if (!m_ctx.arkTSTurboModuleInstanceRef) {
    auto errorMsg = "Couldn't find turbo module '" + name_ +
        "' on ArkUI side. Did you link RNPackage that provides this turbo module?";
    LOG(FATAL) << errorMsg;
    throw std::runtime_error(errorMsg);
  }
  folly::dynamic result;
  m_ctx.taskExecutor->runSyncTask(
      m_ctx.turboModuleThread, [ctx = m_ctx, &methodName, &args, &result]() {
        ArkJS arkJS(ctx.env);
        auto napiArgs =
            arkJS.convertIntermediaryValuesToNapiValues(std::move(args));
        auto napiTurboModuleObject =
            arkJS.getObject(ctx.arkTSTurboModuleInstanceRef);
        auto napiResult = napiTurboModuleObject.call(methodName, napiArgs);
        result = arkJS.getDynamic(napiResult);
      });
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  if (duration.count() > 2) {
    DLOG(WARNING) << "ArkTSTurboModule::callSync: execution time — "
                  << duration.count()
                  << " ms (" + this->name_ + "::" + methodName + ")";
  }
  return result;
}

// calls a TurboModule method without blocking and ignores its result
void rnoh::ArkTSTurboModule::scheduleCall(
    facebook::jsi::Runtime& runtime,
    const std::string& methodName,
    const facebook::jsi::Value* jsiArgs,
    size_t argsCount) {
  react::SystraceSection s(std::string(
                               "#RNOH::ArkTSTurboModule::scheduleCall (" +
                               this->name_ + "::" + methodName + ")")
                               .c_str());
  if (!m_ctx.arkTSTurboModuleInstanceRef) {
    auto errorMsg = "Couldn't find turbo module '" + name_ +
        "' on ArkUI side. Did you link RNPackage that provides this turbo module?";
    LOG(FATAL) << errorMsg;
    throw std::runtime_error(errorMsg);
  }
  auto args = convertJSIValuesToIntermediaryValues(
      runtime, m_ctx.jsInvoker, jsiArgs, argsCount);
  m_ctx.taskExecutor->runTask(
      m_ctx.turboModuleThread,
      [ctx = m_ctx,
       name = name_,
       methodName,
       args = std::move(args),
       &runtime]() mutable {
        try {
          ArkJS arkJS(ctx.env);
          auto napiArgs =
              arkJS.convertIntermediaryValuesToNapiValues(std::move(args));
          auto napiTurboModuleObject =
              arkJS.getObject(ctx.arkTSTurboModuleInstanceRef);
          napiTurboModuleObject.call(methodName, napiArgs);
        } catch (const std::exception& e) {
          LOG(ERROR) << "Exception thrown while calling " << name
                     << " TurboModule method " << methodName << ": "
                     << e.what();
        }
      });
}

// calls an async TurboModule method and returns a Promise
jsi::Value ArkTSTurboModule::callAsync(
    jsi::Runtime& runtime,
    const std::string& methodName,
    const jsi::Value* jsiArgs,
    size_t argsCount) {
  react::SystraceSection s(std::string(
                               "#RNOH::ArkTSTurboModule::callAsync (" +
                               this->name_ + "::" + methodName + ")")
                               .c_str());
  if (!m_ctx.arkTSTurboModuleInstanceRef) {
    auto errorMsg = "Couldn't find turbo module '" + name_ +
        "' on ArkUI side. Did you link RNPackage that provides this turbo module?";
    LOG(FATAL) << errorMsg;
    throw std::runtime_error(errorMsg);
  }
  auto args = convertJSIValuesToIntermediaryValues(
      runtime, m_ctx.jsInvoker, jsiArgs, argsCount);
  return react::createPromiseAsJSIValue(
      runtime,
      [&, args = std::move(args)](
          jsi::Runtime& runtime2,
          std::shared_ptr<react::Promise> jsiPromise) mutable {
        react::LongLivedObjectCollection::get(runtime2).add(jsiPromise);
        m_ctx.taskExecutor->runTask(
            m_ctx.turboModuleThread,
            [name = this->name_,
             methodName,
             args = std::move(args),
             env = m_ctx.env,
             arkTSTurboModuleInstanceRef = m_ctx.arkTSTurboModuleInstanceRef,
             jsInvoker = m_ctx.jsInvoker,
             &runtime2,
             weakJsiPromise =
                 std::weak_ptr<react::Promise>(jsiPromise)]() mutable {
              ArkJS arkJS(env);
              try {
                auto n_promisedResult =
                    arkJS.getObject(arkTSTurboModuleInstanceRef)
                        .call(
                            methodName,
                            arkJS.convertIntermediaryValuesToNapiValues(
                                std::move(args)));
                Promise(env, n_promisedResult)
                    .then(
                        [&runtime2, weakJsiPromise, env, jsInvoker](auto args) {
                          jsInvoker->invokeAsync([&runtime2,
                                                  weakJsiPromise,
                                                  args = std::move(args)]() {
                            auto jsiPromise = weakJsiPromise.lock();
                            if (!jsiPromise) {
                              return;
                            }
                            jsiPromise->resolve(
                                preparePromiseResolverResult(runtime2, args));
                            jsiPromise->allowRelease();
                          });
                        })
                    .catch_([&runtime2, weakJsiPromise, env, jsInvoker](
                                auto args) {
                      jsInvoker->invokeAsync([&runtime2,
                                              weakJsiPromise,
                                              args = std::move(args)]() {
                        auto jsiPromise = weakJsiPromise.lock();
                        if (!jsiPromise) {
                          return;
                        }
                        jsiPromise->reject(preparePromiseRejectionResult(args));
                        jsiPromise->allowRelease();
                      });
                    });
              } catch (const std::exception& e) {
                LOG(ERROR) << std::string("#RNOH::ArkTSTurboModule::callAsync (" + 
                    name + "::" + methodName + ") failed.")
                    .c_str();
                jsInvoker->invokeAsync(
                    [message = std::string(e.what()), weakJsiPromise] {
                      auto jsiPromise = weakJsiPromise.lock();
                      if (!jsiPromise) {
                        return;
                      }
                      jsiPromise->reject(message);
                      jsiPromise->allowRelease();
                    });
                return;
              }
            });
      });
}

std::vector<IntermediaryArg>
ArkTSTurboModule::convertJSIValuesToIntermediaryValues(
    jsi::Runtime& runtime,
    std::shared_ptr<react::CallInvoker> jsInvoker,
    const jsi::Value* jsiArgs,
    size_t argsCount) {
  std::vector<IntermediaryArg> args(argsCount);
  for (int argIdx = 0; argIdx < argsCount; argIdx++) {
    if (jsiArgs[argIdx].isObject()) {
      auto obj = jsiArgs[argIdx].getObject(runtime);
      if (obj.isFunction(runtime)) {
        args[argIdx] = createIntermediaryCallback(
            react::CallbackWrapper::createWeak(
                std::move(obj.getFunction(runtime)), runtime, jsInvoker),
            jsInvoker);
        continue;
      }
    }
    args[argIdx] = jsi::dynamicFromValue(runtime, jsiArgs[argIdx]);
  }
  return args;
}

IntermediaryCallback createIntermediaryCallback(
    std::weak_ptr<react::CallbackWrapper> weakCallback,
    std::shared_ptr<react::CallInvoker> const& jsInvoker) {
  auto weakInvoker = std::weak_ptr(jsInvoker);
  return std::function(
      [weakCallback, weakInvoker](std::vector<folly::dynamic> cbArgs) -> void {
        auto jsInvoker = weakInvoker.lock();
        if (!jsInvoker) {
          return;
        }
        jsInvoker->invokeAsync(
            [weakCallback, callbackArgs = std::move(cbArgs)]() {
              auto callbackWrapper = weakCallback.lock();
              if (!callbackWrapper) {
                return;
              }

              const auto jsArgs = convertDynamicsToJSIValues(
                  callbackWrapper->runtime(), callbackArgs);
              callbackWrapper->callback().call(
                  callbackWrapper->runtime(), jsArgs.data(), jsArgs.size());
              callbackWrapper->allowRelease();
            });
      });
}

const std::vector<jsi::Value> convertDynamicsToJSIValues(
    jsi::Runtime& rt,
    const std::vector<folly::dynamic>& dynamics) {
  std::vector<jsi::Value> values;
  for (auto dynamic : dynamics) {
    values.push_back(jsi::valueFromDynamic(rt, dynamic));
  }
  return values;
}

jsi::Value preparePromiseResolverResult(
    jsi::Runtime& rt,
    const std::vector<folly::dynamic> args) {
  if (args.size() == 0) {
    return jsi::Value::undefined();
  }
  if (args.size() > 1) {
    throw std::invalid_argument("`resolve` accepts only one argument");
  }
  return jsi::valueFromDynamic(rt, args[0]);
}

std::string preparePromiseRejectionResult(
    const std::vector<folly::dynamic> args) {
  if (args.size() == 0) {
    return "";
  }
  if (args.size() > 1) {
    throw std::invalid_argument("`reject` accepts only one argument");
  }

  auto error = args[0];
  if (error.isObject()) {
    auto message = error["message"];
    if (message.isString()) {
      return message.getString();
    }
  } else if (error.isString()) {
    return error.getString();
  }
  throw std::invalid_argument(
      "The type of argument provided `reject` must be string or contain a string 'message' "
      "field. It's going to be used as an error message");
}