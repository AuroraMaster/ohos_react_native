/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ComponentInstancePreallocationRequestQueue.h"
#include <optional>
#include "RNOH/RNOHError.h"
#include "ffrt/cpp/pattern/job_partner.h"
#include "RNOH/ParallelComponent.h"
#include "RNOH/ApiVersionCheck.h"
#include "RNOH/FFRTConfig.h"

namespace rnoh {

void ComponentInstancePreallocationRequestQueue::push(const Request& request) {
  if (!IsAtLeastApi21()) {
    auto lock = std::lock_guard(m_mtx);
    m_queue.push(std::move(request));
  }
  auto delegate = m_weakDelegate.lock();
  if (delegate != nullptr) {
    if (IsAtLeastApi21() &&
        (parallelComponentHandles.find(request.componentHandle) !=
             parallelComponentHandles.end() ||
         ComponentNameManager::getInstance().hasComponentName(
             request.componentName))) {
      auto job_partner = 
            ffrt::job_partner<ScenarioID::PRECREATED_COMPONENTS_PARALLELIZATION>::get_main_partner(
            ffrt::job_partner_attr().max_parallelism(MAX_THREAD_NUM_RECREATED_COMPONENTS).qos(THREAD_PRIORITY_LEVEL_5));
      job_partner->submit<true>([delegate, request = std::move(request)] {
        delegate->processPreallocationRequest(request);
      });
    } else {
      delegate->onPushPreallocationRequest();
    }
  }
}

bool ComponentInstancePreallocationRequestQueue::isEmpty() {
  auto lock = std::lock_guard(m_mtx);
  return m_queue.empty();
}

auto ComponentInstancePreallocationRequestQueue::pop()
    -> std::optional<Request> {
  auto lock = std::lock_guard(m_mtx);
  if (!m_queue.empty()) {
    auto request = m_queue.front();
    m_queue.pop();
    return request;
  }
  return std::nullopt;
}

void ComponentInstancePreallocationRequestQueue::setDelegate(
    Delegate::Weak weakDelegate) {
  m_weakDelegate = weakDelegate;
}

void ComponentInstancePreallocationRequestQueue::clear() {
  auto lock = std::lock_guard(m_mtx);
  m_queue = {};
}

} // namespace rnoh
