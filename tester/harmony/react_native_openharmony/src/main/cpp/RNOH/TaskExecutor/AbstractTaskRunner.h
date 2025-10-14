/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <folly/Function.h>
#include <exception>
#include "RNOH/Assert.h"

class AbstractTaskRunner {
 public:
  using Shared = std::shared_ptr<AbstractTaskRunner>;
  using Task = folly::Function<void()>;
  using DelayedTaskId = uint64_t;
  using ExceptionHandler = std::function<void(std::exception_ptr const)>;

  virtual void runAsyncTask(Task&& task) = 0;
  virtual void runSyncTask(Task&& task) = 0;
  virtual DelayedTaskId
  runDelayedTask(Task&& task, uint64_t delayMs, uint64_t repeatMs = 0) = 0;
  virtual void cancelDelayedTask(DelayedTaskId taskId) = 0;

  virtual bool isOnCurrentThread() const {
    RNOH_ASSERT("isOnCurrentThread() is not implemented");
    return false;
  };

  virtual void setExceptionHandler(ExceptionHandler handler) = 0;

  virtual ~AbstractTaskRunner() = default;
};
