/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <thread>
#include "AbstractTaskRunner.h"
#include "DefaultExceptionHandler.h"

namespace rnoh {

class EventLoopTaskRunner;

class ThreadTaskRunner : public AbstractTaskRunner {
 public:
  ThreadTaskRunner(
      std::string name,
      ExceptionHandler exceptionHandler = defaultExceptionHandler);
  ~ThreadTaskRunner() override;

  void runAsyncTask(Task&& task) override;
  void runSyncTask(Task&& task) override;
  DelayedTaskId
  runDelayedTask(Task&& task, uint64_t delayMs, uint64_t repeatMs = 0) override;
  void cancelDelayedTask(DelayedTaskId taskId) override;

  bool isOnCurrentThread() const override;
  void setExceptionHandler(ExceptionHandler handler) override;

 protected:
  std::thread m_thread;
  std::unique_ptr<EventLoopTaskRunner> m_wrappedTaskRunner{};
};
} // namespace rnoh