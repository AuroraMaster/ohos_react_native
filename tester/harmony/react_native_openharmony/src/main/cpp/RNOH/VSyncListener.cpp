/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "VSyncListener.h"

namespace rnoh {

VSyncListener::VSyncListener(char const* name) : m_vsyncHandle(name) {}

void VSyncListener::scheduleNextVsync() {
  auto alreadyScheduled = m_scheduled.exchange(true);
  if (alreadyScheduled) {
    return;
  }
  m_vsyncHandle.requestFrame(
      [](long long timestamp, void* data) {
        auto weakSelf = static_cast<std::weak_ptr<VSyncListener>*>(data);
        if (auto self = weakSelf->lock()) {
          self->m_scheduled.store(false);
          VSyncCallback callback;
          {
            std::lock_guard<std::mutex> lock(self->m_callbackMutex);
            callback = std::move(self->m_callback);
          }
          if (callback) {
            callback(timestamp);
          }
        }
        delete weakSelf;
      },
      new std::weak_ptr<VSyncListener>(weak_from_this()));
}

void VSyncListener::requestFrame(VSyncCallback callback) {
  {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    m_callback = std::move(callback);
  }
  scheduleNextVsync();
}

bool VSyncListener::isScheduled() const {
  return m_scheduled.load();
}

} // namespace rnoh