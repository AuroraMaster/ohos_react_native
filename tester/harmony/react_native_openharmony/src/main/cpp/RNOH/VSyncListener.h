/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include "NativeVsyncHandle.h"

namespace rnoh {
class VSyncListener : public std::enable_shared_from_this<VSyncListener> {
 public:
  using VSyncCallback = std::function<void(long long timestamp)>;

  VSyncListener(char const* name = "VSyncListener");

  void requestFrame(VSyncCallback callback);

  bool isScheduled() const;

 private:
  void scheduleNextVsync();

  NativeVsyncHandle m_vsyncHandle;
  std::mutex m_callbackMutex;
  VSyncCallback m_callback;
  std::atomic_bool m_scheduled{false};
};
} // namespace rnoh