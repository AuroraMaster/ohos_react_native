/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include "AbstractTaskRunner.h"
#include "DefaultExceptionHandler.h"
#include "uv/Async.h"
#include "uv/EventLoop.h"
#include "uv/Timer.h"

namespace rnoh {
class EventLoopTaskRunner : public AbstractTaskRunner {
 public:
  EventLoopTaskRunner(
      std::string name,
      uv_loop_t* loop,
      ExceptionHandler exceptionHandler = defaultExceptionHandler);
  ~EventLoopTaskRunner() override;

  EventLoopTaskRunner(const EventLoopTaskRunner&) = delete;
  EventLoopTaskRunner& operator=(const EventLoopTaskRunner&) = delete;

  void runAsyncTask(Task&& task) override;
  void runSyncTask(Task&& task) override;

  DelayedTaskId
  runDelayedTask(Task&& task, uint64_t delayMs, uint64_t repeatMs = 0) override;
  void cancelDelayedTask(DelayedTaskId taskId) override;

  void setExceptionHandler(ExceptionHandler handler) override;

 protected:
  virtual void executeTask();

  Task popNextTask();
  void waitForSyncTask(Task&& task);
  void cleanup();

  std::atomic<DelayedTaskId> m_nextTaskId = 0;
  std::string m_name;
  uv_loop_t* m_loop;
  std::atomic_bool m_running{true};
  std::queue<Task> m_asyncTaskQueue{};
  std::queue<Task> m_syncTaskQueue{};
  std::mutex m_mutex;
  std::unique_ptr<uv::Async> m_asyncHandle;
  std::unordered_map<DelayedTaskId, uv::Timer> m_timerByTaskId;
  ExceptionHandler m_exceptionHandler;
  bool cleanedUp = false;

  friend class ThreadTaskRunner;
};
} // namespace rnoh