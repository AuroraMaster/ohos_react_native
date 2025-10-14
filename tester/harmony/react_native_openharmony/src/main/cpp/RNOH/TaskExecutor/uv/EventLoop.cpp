/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "EventLoop.h"
#include <glog/logging.h>

namespace rnoh::uv {

EventLoop::EventLoop() : m_loop(new uv_loop_t) {
  uv_loop_init(m_loop);
}

EventLoop::~EventLoop() {
  auto status = uv_loop_close(m_loop);
  if (status == UV_EBUSY) {
    LOG(ERROR)
        << "Tried to destroy a busy event loop. The loop will continue running, and the memory will be leaked.";
    return;
  }
  delete m_loop;
}

uv_loop_t* EventLoop::handle() const {
  return m_loop;
}

void EventLoop::run() const {
  uv_run(m_loop, UV_RUN_DEFAULT);
}

} // namespace rnoh::uv