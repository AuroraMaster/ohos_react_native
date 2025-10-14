/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "Async.h"

namespace rnoh::uv {

Async::Async(uv_loop_t* loop, Callback callback)
    : m_handle(new uv_async_t), m_callback(std::move(callback)) {
  m_handle->data = this;
  uv_async_init(loop, m_handle, [](uv_async_t* handle) {
    auto asyncHandle = static_cast<Async*>(handle->data);
    asyncHandle->m_callback();
  });
}

Async::~Async() noexcept {
  uv_close(reinterpret_cast<uv_handle_t*>(m_handle), [](uv_handle_t* handle) {
    delete reinterpret_cast<uv_async_t*>(handle);
  });
}

void Async::send() {
  uv_async_send(m_handle);
}

} // namespace rnoh::uv