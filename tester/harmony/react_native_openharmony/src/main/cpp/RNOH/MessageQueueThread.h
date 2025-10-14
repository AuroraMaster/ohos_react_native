/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "RNOH/TaskExecutor/TaskExecutor.h"

namespace rnoh {

class MessageQueueThread : public facebook::react::MessageQueueThread {
 public:
  MessageQueueThread(std::shared_ptr<TaskExecutor> const& taskExecutor);
  virtual ~MessageQueueThread();

  void runOnQueue(std::function<void()>&& func) override;

  void runOnQueueSync(std::function<void()>&& func) override;

  void quitSynchronous() override;

 private:
  std::shared_ptr<TaskExecutor> taskExecutor;
};

} // namespace rnoh