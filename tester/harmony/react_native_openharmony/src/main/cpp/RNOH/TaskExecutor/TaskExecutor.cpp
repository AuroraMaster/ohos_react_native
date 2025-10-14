/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "TaskExecutor.h"
#include <glog/logging.h>
#include <react/renderer/debug/SystraceSection.h>
#include "NapiTaskRunner.h"
#include "RNOH/Assert.h"
#include "RNOH/Performance/HarmonyReactMarker.h"
#include "RNOH/RNOHError.h"
#include "ThreadTaskRunner.h"

namespace rnoh {

TaskExecutor::TaskExecutor(
    napi_env mainEnv,
    std::unique_ptr<AbstractTaskRunner> workerTaskRunner,
    bool shouldEnableBackground) {
  auto mainTaskRunner = std::make_shared<NapiTaskRunner>("RNOH_MAIN", mainEnv);
  auto jsTaskRunner = std::make_shared<ThreadTaskRunner>("RNOH_JS");
  auto backgroundExecutor = shouldEnableBackground
      ? std::make_shared<ThreadTaskRunner>("RNOH_BACKGROUND")
      : nullptr;
  m_taskRunners = {
      mainTaskRunner,
      jsTaskRunner,
      backgroundExecutor,
      std::move(workerTaskRunner)};
  this->runTask(TaskThread::JS, [this]() {
    this->setTaskThreadPriority(QoS_Level::QOS_USER_INTERACTIVE);
  });
  if (shouldEnableBackground) {
    this->runTask(TaskThread::BACKGROUND, [this]() {
      this->setTaskThreadPriority(QoS_Level::QOS_USER_INTERACTIVE);
    });
  }
}

TaskExecutor::~TaskExecutor() noexcept {
  DLOG(INFO) << "TaskExecutor::~TaskExecutor()";
  std::thread cleanupThread(
      [](std::array<AbstractTaskRunner::Shared, 4> taskRunners) {},
      std::move(m_taskRunners));
  cleanupThread.detach();
}

void TaskExecutor::setTaskThreadPriority(QoS_Level level) {
#ifdef C_API_ARCH
  int ret = OH_QoS_SetThreadQoS(level);
  std::array<char, 16> buffer = {0};
  pthread_getname_np(pthread_self(), buffer.data(), buffer.size());
  DLOG(INFO) << "TaskExecutor::setTaskThreadPriority " << buffer.data()
             << (ret == 0 ? " SUCCESSFUL" : " FAILED");
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::CHANGE_THREAD_PRIORITY,
      buffer.data());
#else
  DLOG(WARNING)
      << "TaskExecutor::setTaskThreadPriority available only with C-API";
#endif
}

void TaskExecutor::runTask(TaskThread thread, Task&& task) {
  facebook::react::SystraceSection s("#RNOH::TaskExecutor::runTask");
  auto taskRunner = this->getTaskRunner(thread);
  taskRunner->runAsyncTask(std::move(task));
}

void TaskExecutor::runSyncTask(TaskThread thread, Task&& task) {
  auto waitsOnThread = m_waitsOnThread[thread];
  if (waitsOnThread.has_value() && isOnTaskThread(waitsOnThread.value())) {
    throw RNOHError("Deadlock detected");
  }
  auto currentThread = getCurrentTaskThread();
  if (currentThread.has_value() && currentThread != thread) {
    m_waitsOnThread[currentThread.value()] = thread;
  }
  std::exception_ptr thrownError;
  auto taskRunner = this->getTaskRunner(thread);
  taskRunner->runSyncTask([task = std::move(task), &thrownError]() mutable {
    try {
      task();
    } catch (const std::exception& e) {
      thrownError = std::current_exception();
    }
  });
  if (thrownError) {
    std::rethrow_exception(thrownError);
  }
  if (currentThread.has_value()) {
    m_waitsOnThread[currentThread.value()] = std::nullopt;
  }
}

TaskExecutor::DelayedTask TaskExecutor::runDelayedTask(
    TaskThread thread,
    Task&& task,
    uint64_t delayMs,
    uint64_t repeatMs) {
  auto runner = this->getTaskRunner(thread);
  auto id = runner->runDelayedTask(std::move(task), delayMs, repeatMs);
  return {id, thread};
}

void TaskExecutor::cancelDelayedTask(DelayedTask taskId) {
  auto runner = this->getTaskRunner(taskId.thread);
  runner->cancelDelayedTask(taskId.taskId);
}

bool TaskExecutor::isOnTaskThread(TaskThread thread) const {
  auto runner = m_taskRunners[thread];
  return runner && runner->isOnCurrentThread();
}

std::optional<TaskThread> TaskExecutor::getCurrentTaskThread() const {
  if (isOnTaskThread(TaskThread::MAIN)) {
    return TaskThread::MAIN;
  } else if (isOnTaskThread(TaskThread::JS)) {
    return TaskThread::JS;
  } else if (isOnTaskThread(TaskThread::BACKGROUND)) {
    return TaskThread::BACKGROUND;
  } else if (isOnTaskThread(TaskThread::WORKER)) {
    return TaskThread::WORKER;
  } else {
    return std::nullopt;
  }
}

void TaskExecutor::setExceptionHandler(ExceptionHandler handler) {
    for (auto& taskRunner : m_taskRunners) {
    if (taskRunner) {
      taskRunner->setExceptionHandler(handler);
    }
  }
}

AbstractTaskRunner::Shared TaskExecutor::getTaskRunner(
    TaskThread taskThread) const {
  auto runner = m_taskRunners[taskThread];
  RNOH_ASSERT(runner != nullptr);
  return runner;
}

} // namespace rnoh
