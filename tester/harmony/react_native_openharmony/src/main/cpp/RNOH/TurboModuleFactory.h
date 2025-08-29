/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <ReactCommon/TurboModule.h>
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/FeatureFlagRegistry.h"
#include "RNOH/UIManagerModule.h"
#include "napi/native_api.h"
namespace rnoh {

class TurboModuleProvider;

class TurboModuleFactoryDelegate {
 public:
  using Context = ArkTSTurboModule::Context;
  using SharedTurboModule = std::shared_ptr<facebook::react::TurboModule>;

  virtual ~TurboModuleFactoryDelegate(){};
  virtual SharedTurboModule createTurboModule(
      Context ctx,
      const std::string& name) const = 0;
};

class TurboModuleFactory {
 public:
  struct ArkTSTurboModuleEnvironment {
    napi_env napiEnv;
    NapiRef arkTSTurboModuleProviderRef;
  };

  using Context = ArkTSTurboModule::Context;
  using SharedTurboModule = std::shared_ptr<facebook::react::TurboModule>;

  TurboModuleFactory(
      std::unordered_map<TaskThread, ArkTSTurboModuleEnvironment>
          arkTSTurboModuleEnvironmentByTaskThread,
      FeatureFlagRegistry::Shared featureFlagRegistry,
      const ComponentJSIBinderByString&&,
      TaskExecutor::Shared,
      std::vector<std::shared_ptr<TurboModuleFactoryDelegate>>,
      std::shared_ptr<ArkTSMessageHub> arkTSMessageHub);

  ~TurboModuleFactory() noexcept;

  virtual SharedTurboModule create(
      std::shared_ptr<facebook::react::CallInvoker> jsInvoker,
      const std::string& name,
      std::shared_ptr<EventDispatcher> eventDispatcher,
      std::shared_ptr<MessageQueueThread> jsQueue,
      std::shared_ptr<facebook::react::Scheduler> scheduler,
      RNInstance::SafeWeak instance) const;

 protected:
  std::optional<TaskThread> findArkTSTurboModuleThread(
      const std::string& turboModuleName) const;

  bool hasArkTSTurboModule(
      const std::string& turboModuleName,
      TaskThread thread,
      napi_env env,
      NapiRef const& turboModuleProviderRef) const;

  SharedTurboModule delegateCreatingTurboModule(
      Context ctx,
      const std::string& name) const;

  NapiRef maybeGetArkTsTurboModuleInstanceRef(
      const std::string& name,
      TaskThread thread,
      ArkTSTurboModuleEnvironment arkTSTurboModuleEnv) const;

  ArkTSTurboModuleEnvironment getArkTSTurboModuleEnvironmentByTaskThread(
      TaskThread taskThread) const;

  virtual SharedTurboModule handleUnregisteredModuleRequest(
      Context ctx,
      const std::string& name) const;

  const ComponentJSIBinderByString m_componentBinderByString;
  std::unordered_map<TaskThread, ArkTSTurboModuleEnvironment>
      m_arkTSTurboModuleEnvironmentByTaskThread;
  TaskExecutor::Shared m_taskExecutor;
  std::vector<std::shared_ptr<TurboModuleFactoryDelegate>> m_delegates;
  std::shared_ptr<ArkTSMessageHub> m_arkTSMessageHub;
  FeatureFlagRegistry::Shared m_featureFlagRegistry;
};

} // namespace rnoh
