/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
 
#ifndef JSVM_EXECUTOR_FACTORY_H
#define JSVM_EXECUTOR_FACTORY_H

#include <jsireact/JSIExecutor.h>

namespace rnjsvm {
class JSVMExecutorFactory : public facebook::react::JSExecutorFactory {
 public:
  explicit JSVMExecutorFactory(
      facebook::react::JSIExecutor::RuntimeInstaller runtimeInstaller,
      folly::dynamic initOptions)
      : runtimeInstaller_(runtimeInstaller),
        timeoutInvoker_(facebook::react::JSIExecutor::defaultTimeoutInvoker),
        m_initOptions(initOptions) {}

  std::unique_ptr<facebook::react::JSExecutor> createJSExecutor(
      std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) override;

 private:
  facebook::react::JSIExecutor::RuntimeInstaller runtimeInstaller_;
  facebook::react::JSIScopedTimeoutInvoker timeoutInvoker_;
  folly::dynamic m_initOptions;
};

class JSVMExecutor : public facebook::react::JSIExecutor {
 public:
  JSVMExecutor(
      std::shared_ptr<facebook::jsi::Runtime> runtime,
      std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      const facebook::react::JSIScopedTimeoutInvoker& timeoutInvoker,
      RuntimeInstaller runtimeInstaller);

 private:
  facebook::react::JSIScopedTimeoutInvoker timeoutInvoker_;
};

} // namespace rnjsvm

#endif