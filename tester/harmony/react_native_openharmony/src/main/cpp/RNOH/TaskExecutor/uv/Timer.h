/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <folly/Function.h>
#include <uv.h>

namespace rnoh {
namespace uv {
class Timer final {
 public:
  using Callback = folly::Function<void()>;

  Timer(uv_loop_t* loop, Callback callback, uint64_t timeout, uint64_t repeat);
  ~Timer() noexcept;

  // rule of five (no copy, custom move)
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;
  Timer(Timer&&) noexcept;
  Timer& operator=(Timer&&) noexcept;

 private:
  uv_timer_t* m_handle;
  Callback m_callback;
};
} // namespace uv
} // namespace rnoh