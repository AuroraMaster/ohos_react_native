/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "EventLoopTaskRunner.h"
#include <glog/logging.h>
#include <react/renderer/debug/SystraceSection.h>
#include "RNOH/Assert.h"
#include "RNOH/RNOHError.h"

namespace rnoh {
EventLoopTaskRunner::EventLoopTaskRunner(
    std::string name,
    uv_loop_t* loop,
    ExceptionHandler exceptionHandler)
    : m_name(name),
      m_loop(loop),
      m_asyncHandle(
          std::make_unique<uv::Async>(m_loop, [this] { this->executeTask(); })),
      m_exceptionHandler(std::move(exceptionHandler)) {}

EventLoopTaskRunner::~EventLoopTaskRunner() {
  DLOG(INFO) << "EventLoopTaskRunner::~EventLoopTaskRunner()";
  RNOH_ASSERT_MSG(
      cleanedUp,
      "EventLoopTaskRunner::cleanup must be called in the derived class before it is destructed to properly clean up resources");
}

void EventLoopTaskRunner::runAsyncTask(Task&& task) {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_asyncTaskQueue.push(std::move(task));
    m_asyncHandle->send();
  }
}

void EventLoopTaskRunner::runSyncTask(Task&& task) {
  if (isOnCurrentThread()) {
    task();
    return;
  }
  waitForSyncTask(std::move(task));
}

EventLoopTaskRunner::DelayedTaskId EventLoopTaskRunner::runDelayedTask(
    Task&& task,
    uint64_t delayMs,
    uint64_t repeatMs) {
  auto id = m_nextTaskId++;

  auto timerCallback = [this, id, repeatMs, task = std::move(task)]() mutable {
    task();
    if (repeatMs == 0) {
      m_timerByTaskId.erase(id);
    }
  };

  auto createTimer = [this,
                      id,
                      delayMs,
                      repeatMs,
                      timerCallback = std::move(timerCallback)]() mutable {
    auto [it, inserted] = m_timerByTaskId.emplace(
        id, uv::Timer(m_loop, std::move(timerCallback), delayMs, repeatMs));
  };

  if (isOnCurrentThread()) {
    createTimer();
  } else {
    runAsyncTask(std::move(createTimer));
  }
  return id;
}

void EventLoopTaskRunner::cancelDelayedTask(DelayedTaskId taskId) {
  runAsyncTask([this, taskId] {
    auto it = m_timerByTaskId.find(taskId);
    if (it != m_timerByTaskId.end()) {
      m_timerByTaskId.erase(it);
    }
  });
}

void EventLoopTaskRunner::setExceptionHandler(ExceptionHandler handler) {
  m_exceptionHandler = std::move(handler);
}

void EventLoopTaskRunner::executeTask() {
  Task task = popNextTask();
  if (task) {
    try {
      facebook::react::SystraceSection s("#RNOH::TaskRunner::task");
      task();
      // ensure the resources captured by the task are cleaned up
      task = nullptr;
    } catch (...) {
      m_exceptionHandler(std::current_exception());
    }
  }
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_syncTaskQueue.empty() || !m_asyncTaskQueue.empty()) {
    m_asyncHandle->send();
  }
}

auto EventLoopTaskRunner::popNextTask() -> Task {
  std::lock_guard<std::mutex> queueLock(m_mutex);
  Task task{};
  if (!m_running) {
    return {};
  }
  if (!m_syncTaskQueue.empty()) {
    task = std::move(m_syncTaskQueue.front());
    m_syncTaskQueue.pop();
  } else if (!m_asyncTaskQueue.empty()) {
    task = std::move(m_asyncTaskQueue.front());
    m_asyncTaskQueue.pop();
  }
  return task;
}

void EventLoopTaskRunner::waitForSyncTask(Task&& task) {
  std::mutex mtx;
  std::condition_variable cv;
  std::atomic_bool done{false};

  // Wrap the task to notify the waiting thread when it's done
  auto wrappedTask = [task = std::move(task), &done, &cv, &mtx]() mutable {
    auto doneLock = std::unique_lock(mtx);
    try {
      task();
    } catch (...) {
      done = true;
      cv.notify_one();
      throw;
    }
    done = true;
    cv.notify_one();
  };

  {
    std::unique_lock<std::mutex> queueLock(m_mutex);
    m_syncTaskQueue.push(std::move(wrappedTask));
    m_asyncHandle->send();
  }
  auto doneLock = std::unique_lock(mtx);
  cv.wait(doneLock, [this, &done] { return done.load(); });
}

void EventLoopTaskRunner::cleanup() {
  if (cleanedUp) {
    return;
  }
  runSyncTask([this] {
    std::lock_guard<std::mutex> queueLock(m_mutex);
    m_running = false;
    m_asyncHandle.reset();
    RNOH_ASSERT_MSG(
        m_syncTaskQueue.empty(),
        "Task runner was destroyed while there were pending sync tasks");
    m_asyncTaskQueue = {};
    m_timerByTaskId.clear();
  });
  cleanedUp = true;
}

} // namespace rnoh