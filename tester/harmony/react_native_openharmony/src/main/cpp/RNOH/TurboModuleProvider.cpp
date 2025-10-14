/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/TurboModuleProvider.h"

#include <ReactCommon/LongLivedObject.h>
#include <ReactCommon/TurboModuleBinding.h>
#include <glog/logging.h>
#include <mutex>

using namespace rnoh;
using namespace facebook;

TurboModuleProvider::TurboModuleProvider(
    std::shared_ptr<react::CallInvoker> jsInvoker,
    TurboModuleFactory&& turboModuleFactory,
    std::shared_ptr<EventDispatcher> eventDispatcher,
    std::shared_ptr<MessageQueueThread> jsQueue,
    RNInstance::SafeWeak const& instance)
    : m_jsInvoker(jsInvoker),
      m_instance(instance),
      m_createTurboModule(
          [eventDispatcher,
           jsQueue,
           instance = m_instance,
           factory = std::move(turboModuleFactory)](
              std::string const& moduleName,
              std::shared_ptr<react::CallInvoker> jsInvoker,
              std::shared_ptr<react::Scheduler> scheduler)
              -> std::shared_ptr<react::TurboModule> {
            return factory.create(
                jsInvoker,
                moduleName,
                eventDispatcher,
                jsQueue,
                scheduler,
                instance);
          }) {}

void TurboModuleProvider::installJSBindings(
    react::RuntimeExecutor runtimeExecutor) {
  if (!runtimeExecutor) {
    LOG(ERROR)
        << "Turbo Modules couldn't be installed. Invalid RuntimeExecutor.";
    return;
  }
  auto turboModuleProvider =
      [self = this->shared_from_this()](std::string const& moduleName) {
        auto turboModule = self->getTurboModule(moduleName);
        return turboModule;
      };
  runtimeExecutor([turboModuleProvider = std::move(turboModuleProvider)](
                      facebook::jsi::Runtime& runtime) {
    react::TurboModuleBinding::install(
        runtime,
        react::TurboModuleBindingMode::HostObject,
        std::move(turboModuleProvider));
  });
}

std::shared_ptr<react::TurboModule> TurboModuleProvider::getTurboModule(
    std::string const& moduleName) {
  {
    std::unique_lock lock(m_cacheMtx);
    if (auto it = m_cache.find(moduleName); it != m_cache.end()) {
      DLOG(INFO) << "Cache hit. Providing '" << moduleName << "' Turbo Module";
      return it->second;
    }
  }
  auto turboModule = m_createTurboModule(moduleName, m_jsInvoker, m_scheduler);
  if (turboModule != nullptr) {
    std::unique_lock lock(m_cacheMtx);
    auto [it, inserted] = m_cache.emplace(moduleName, std::move(turboModule));
    return it->second;
  }
  LOG(ERROR) << "Couldn't provide turbo module \"" << moduleName << "\"";
  return nullptr;
}
