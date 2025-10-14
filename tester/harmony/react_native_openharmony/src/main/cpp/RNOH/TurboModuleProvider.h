/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/LongLivedObject.h>
#include <ReactCommon/RuntimeExecutor.h>
#include <ReactCommon/TurboModule.h>
#include <butter/map.h>
#include <react/renderer/scheduler/Scheduler.h>
#include <functional>
#include "EventDispatcher.h"
#include "RNOH/TurboModuleFactory.h"

namespace rnoh {

class TurboModuleProvider
    : public std::enable_shared_from_this<TurboModuleProvider> {
 public:
  using Shared = std::shared_ptr<TurboModuleProvider>;

  TurboModuleProvider(
      std::shared_ptr<facebook::react::CallInvoker> jsInvoker,
      TurboModuleFactory&& turboModuleFactory,
      std::shared_ptr<EventDispatcher> eventDispatcher,
      std::shared_ptr<MessageQueueThread> jsQueue,
      RNInstance::SafeWeak const& instance);

  std::shared_ptr<facebook::react::TurboModule> getTurboModule(
      std::string const& moduleName);
  void installJSBindings(facebook::react::RuntimeExecutor runtimeExecutor);
  void setScheduler(std::shared_ptr<facebook::react::Scheduler> scheduler) {
    this->m_scheduler = scheduler;
  }

 private:
  std::shared_ptr<facebook::react::CallInvoker> m_jsInvoker;
  RNInstance::SafeWeak m_instance;
  std::function<std::shared_ptr<facebook::react::TurboModule>(
      std::string const&,
      std::shared_ptr<facebook::react::CallInvoker>,
      std::shared_ptr<facebook::react::Scheduler>)>
      m_createTurboModule;
  std::mutex m_cacheMtx{};
  facebook::butter::
      map<std::string, std::shared_ptr<facebook::react::TurboModule>>
          m_cache;
  std::shared_ptr<facebook::react::Scheduler> m_scheduler;
};

} // namespace rnoh
