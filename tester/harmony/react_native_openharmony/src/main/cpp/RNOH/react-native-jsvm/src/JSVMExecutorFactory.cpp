/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "JSVMExecutorFactory.h"
#include <glog/logging.h>
#include "JSVMRuntime.h"

namespace rnjsvm {
std::unique_ptr<facebook::react::JSExecutor>
JSVMExecutorFactory::createJSExecutor(
    std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) {
  std::unique_ptr<facebook::jsi::Runtime> runtime =
      std::make_unique<JSVMRuntime>(jsQueue, m_initOptions);

  // Add js engine information to Error.prototype so in error reporting we
  // can send this information.
  auto errorPrototype = runtime->global()
                            .getPropertyAsObject(*runtime, "Error")
                            .getPropertyAsObject(*runtime, "prototype");
  errorPrototype.setProperty(*runtime, "jsEngine", "JSVM");

  return std::make_unique<JSVMExecutor>(
      std::move(runtime),
      delegate,
      jsQueue,
      timeoutInvoker_,
      runtimeInstaller_);
}

JSVMExecutor::JSVMExecutor(
    std::shared_ptr<facebook::jsi::Runtime> runtime,
    std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
    const facebook::react::JSIScopedTimeoutInvoker& timeoutInvoker,
    RuntimeInstaller runtimeInstaller)
    : facebook::react::JSIExecutor(runtime, delegate, timeoutInvoker, runtimeInstaller) {}

} // namespace rnjsvm