/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <array>
#include <memory>
#include <optional>
#include "AbstractTaskRunner.h"
#include "qos/qos.h"

namespace rnoh {

enum TaskThread {
  MAIN = 0, // main thread running the eTS event loop
  JS, // React Native's JS runtime thread
  BACKGROUND, // background tasks queue
  WORKER, // used by some turbo modules
};

class TaskExecutor {
 public:
  using Task = AbstractTaskRunner::Task;
  using ExceptionHandler = AbstractTaskRunner::ExceptionHandler;
  using Shared = std::shared_ptr<TaskExecutor>;
  using Weak = std::weak_ptr<TaskExecutor>;

  struct DelayedTask {
   public:
    // copy and move constructors
    DelayedTask(const DelayedTask&) = default;
    DelayedTask& operator=(const DelayedTask&) = default;
    DelayedTask(DelayedTask&&) = default;
    DelayedTask& operator=(DelayedTask&&) = default;

   private:
    DelayedTask(AbstractTaskRunner::DelayedTaskId id, TaskThread thread)
        : taskId(id), thread(thread) {}

    AbstractTaskRunner::DelayedTaskId taskId;
    TaskThread thread;

    friend class TaskExecutor;
  };

  TaskExecutor(
      napi_env mainEnv,
      std::unique_ptr<AbstractTaskRunner> workerTaskRunner,
      bool shouldEnableBackground = false);
  ~TaskExecutor() noexcept;

  void runTask(TaskThread thread, Task&& task);
  void runSyncTask(TaskThread thread, Task&& task);
  DelayedTask runDelayedTask(
      TaskThread thread,
      Task&& task,
      uint64_t delayMs,
      uint64_t repeatMs = 0);
  void cancelDelayedTask(DelayedTask taskId);

  bool isOnTaskThread(TaskThread thread) const;

  std::optional<TaskThread> getCurrentTaskThread() const;

  void setExceptionHandler(ExceptionHandler handler);

 private:
  AbstractTaskRunner::Shared getTaskRunner(TaskThread taskThread) const;

  void setTaskThreadPriority(QoS_Level);
  std::array<std::shared_ptr<AbstractTaskRunner>, TaskThread::WORKER + 1>
      m_taskRunners;
  std::array<std::optional<TaskThread>, TaskThread::WORKER + 1> m_waitsOnThread;
};

} // namespace rnoh
