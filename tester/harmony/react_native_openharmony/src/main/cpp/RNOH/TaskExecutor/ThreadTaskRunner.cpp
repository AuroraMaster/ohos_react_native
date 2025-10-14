/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ThreadTaskRunner.h"
#include "EventLoopTaskRunner.h"
#include "RNOH/Assert.h"
#include "uv/EventLoop.h"
namespace rnoh {
ThreadTaskRunner::ThreadTaskRunner(
    std::string name,
    ExceptionHandler exceptionHandler) {
  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock lock(mtx);
  m_thread = std::thread{[&] {
    uv::EventLoop eventLoop;
    {
      std::unique_lock lock(mtx);
      this->m_wrappedTaskRunner = std::make_unique<EventLoopTaskRunner>(
          name, eventLoop.handle(), exceptionHandler);
      cv.notify_one();
    }
    eventLoop.run();
  }};
  cv.wait(lock, [&] { return this->m_wrappedTaskRunner != nullptr; });
  auto handle = m_thread.native_handle();
  pthread_setname_np(handle, name.c_str());
}

ThreadTaskRunner::~ThreadTaskRunner() {
  RNOH_ASSERT(!isOnCurrentThread());
  DLOG(INFO) << "ThreadTaskRunner::~ThreadTaskRunner()::start";
  // drop the wrapped task runner to stop the event loop
  m_wrappedTaskRunner->cleanup();
  m_wrappedTaskRunner.reset();
  m_thread.join();
  DLOG(INFO) << "ThreadTaskRunner::~ThreadTaskRunner()::stop";
}

void ThreadTaskRunner::runAsyncTask(Task&& task) {
  m_wrappedTaskRunner->runAsyncTask(std::move(task));
}

void ThreadTaskRunner::runSyncTask(Task&& task) {
  m_wrappedTaskRunner->runSyncTask(std::move(task));
}

auto ThreadTaskRunner::runDelayedTask(
    Task&& task,
    uint64_t delayMs,
    uint64_t repeatMs) -> DelayedTaskId {
  return m_wrappedTaskRunner->runDelayedTask(
      std::move(task), delayMs, repeatMs);
}

void ThreadTaskRunner::cancelDelayedTask(DelayedTaskId taskId) {
  m_wrappedTaskRunner->cancelDelayedTask(taskId);
}

bool ThreadTaskRunner::isOnCurrentThread() const {
  return m_thread.get_id() == std::this_thread::get_id();
}

void ThreadTaskRunner::setExceptionHandler(ExceptionHandler handler) {
  m_wrappedTaskRunner->setExceptionHandler(handler);
}

} // namespace rnoh