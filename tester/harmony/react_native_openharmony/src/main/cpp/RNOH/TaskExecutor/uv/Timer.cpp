/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "Timer.h"

namespace rnoh::uv {

Timer::Timer(
    uv_loop_t* loop,
    Callback callback,
    uint64_t timeout,
    uint64_t repeat)
    : m_handle(new uv_timer_t), m_callback(std::move(callback)) {
  m_handle->data = this;
  uv_timer_init(loop, m_handle);
  uv_timer_start(
      m_handle,
      [](uv_timer_t* handle) {
        auto timerHandle = static_cast<Timer*>(handle->data);
        timerHandle->m_callback();
      },
      timeout,
      repeat);
}

Timer::~Timer() noexcept {
  if (m_handle == nullptr) {
    return;
  }
  uv_close(reinterpret_cast<uv_handle_t*>(m_handle), [](uv_handle_t* handle) {
    delete reinterpret_cast<uv_timer_t*>(handle);
  });
}

Timer::Timer(Timer&& other) noexcept {
  m_handle = other.m_handle;
  m_callback = std::move(other.m_callback);
  m_handle->data = this;
  other.m_handle = nullptr;
}

Timer& Timer::operator=(Timer&& other) noexcept {
  m_handle = other.m_handle;
  m_callback = std::move(other.m_callback);
  m_handle->data = this;
  other.m_handle = nullptr;
  return *this;
}

} // namespace rnoh::uv