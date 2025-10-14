/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <thread>
#include "RNOH/Assert.h"

namespace rnoh {

/**
 * Utility class for asserting that an object is called and destroyed on the
 * same thread that created the object.
 */
class ThreadGuard final {
 public:
  void assertThread() const {
    RNOH_ASSERT_MSG(
        m_threadId == std::this_thread::get_id(),
        "This method must be called on the same thread that created the object.");
  }

  ThreadGuard() = default;
  ~ThreadGuard() {
    assertThread();
  }

  // rule-of-five constructors and assignment operators
  ThreadGuard(ThreadGuard const&) = default;
  ThreadGuard& operator=(ThreadGuard const&) = default;

  ThreadGuard(ThreadGuard&&) = default;
  ThreadGuard& operator=(ThreadGuard&&) = default;

 private:
  std::thread::id m_threadId = std::this_thread::get_id();
};
} // namespace rnoh