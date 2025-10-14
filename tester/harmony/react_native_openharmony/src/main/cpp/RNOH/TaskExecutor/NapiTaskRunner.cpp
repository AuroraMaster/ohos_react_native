/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include <glog/logging.h>
#include <napi/native_api.h>
#include <uv.h>
#include <atomic>

#include "NapiTaskRunner.h"
#include "RNOH/Assert.h"

namespace rnoh {

NapiTaskRunner::NapiTaskRunner(
    std::string name,
    napi_env env,
    ExceptionHandler exceptionHandler)
    : EventLoopTaskRunner(
          std::move(name),
          getLoop(env),
          std::move(exceptionHandler)),
      m_env(env) {
  // NOTE: let's hope the JS runtime doesn't move between system threads...
  m_threadId = std::this_thread::get_id();
}

NapiTaskRunner::~NapiTaskRunner() {
  DLOG(INFO) << "NapiTaskRunner(" << m_name << ")::~NapiTaskRunner()";
  cleanup();
}

bool NapiTaskRunner::isOnCurrentThread() const {
  return m_threadId == std::this_thread::get_id();
}

void NapiTaskRunner::executeTask() {
  RNOH_ASSERT(isOnCurrentThread());

  // https://nodejs.org/api/n-api.html#napi_handle_scope
  // "For any invocations of code outside the execution of a native method
  // (...) the module is required to create a scope before invoking any
  // functions that can result in the creation of JavaScript values"
  auto env = m_env;
  napi_handle_scope scope;
  auto result = napi_open_handle_scope(env, &scope);
  if (result != napi_ok) {
    LOG(ERROR) << "Failed to open handle scope";
    return;
  }

  EventLoopTaskRunner::executeTask();

  result = napi_close_handle_scope(env, scope);
  if (result != napi_ok) {
    LOG(ERROR) << "Failed to close handle scope";
    return;
  }
}

uv_loop_t* NapiTaskRunner::getLoop(napi_env env) const {
  uv_loop_t* loop = nullptr;
  napi_get_uv_event_loop(env, &loop);
  return loop;
}

} // namespace rnoh
