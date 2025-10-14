/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <glog/logging.h>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <unordered_set>
#include "NativeVsyncHandle.h"

namespace rnoh {
/**
 * @internal
 */
class UITicker {
 public:
  using Timestamp = std::chrono::
      time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;

  static void onTick(long long _timestamp, void* data) {
    auto self = static_cast<UITicker*>(data);
    self->tick(std::chrono::steady_clock::now());
  }

  UITicker() : m_vsyncHandle("UITicker") {}

  using Shared = std::shared_ptr<UITicker>;

  std::function<void()> subscribe(std::function<void(Timestamp)>&& listener) {
    std::lock_guard lock(listenersMutex);
    auto id = m_nextListenerId;
    m_nextListenerId++;
    auto listenersCount = m_listenerById.size();
    m_listenerById.insert_or_assign(id, std::move(listener));
    if (listenersCount == 0) {
      this->requestNextTick();
    }
    return [id, this]() {
      std::lock_guard lock(listenersMutex);
      this->m_listenerById.erase(id);
    };
  }

 private:
  std::unordered_map<int, std::function<void(Timestamp)>> m_listenerById;
  std::mutex listenersMutex;
  NativeVsyncHandle m_vsyncHandle;
  int m_nextListenerId = 0;

  void requestNextTick() {
    m_vsyncHandle.requestFrame(onTick, this);
  }

  void tick(Timestamp timestamp) {
    std::lock_guard lock(listenersMutex);
    if (m_listenerById.empty()) {
      return;
    }
    for (const auto& idAndListener : m_listenerById) {
      idAndListener.second(timestamp);
    }
    this->requestNextTick();
  }
};

} // namespace rnoh