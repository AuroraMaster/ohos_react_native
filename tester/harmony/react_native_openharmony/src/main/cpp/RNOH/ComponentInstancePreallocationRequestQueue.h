/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */


#pragma once

#include <mutex>
#include <queue>
#include "react/renderer/mounting/ShadowView.h"

namespace rnoh {

/**
 * @internal
 * @threadSafe
 */
class ComponentInstancePreallocationRequestQueue {
 public:
  class Delegate {
    friend ComponentInstancePreallocationRequestQueue;

   public:
    using Weak = std::weak_ptr<Delegate>;

   protected:
    virtual void onPushPreallocationRequest() = 0;
  };

  using Shared = std::shared_ptr<ComponentInstancePreallocationRequestQueue>;
  using Weak = std::weak_ptr<ComponentInstancePreallocationRequestQueue>;
  using Request = struct {
    facebook::react::Tag tag;
    facebook::react::ComponentHandle componentHandle;
    facebook::react::ComponentName componentName;
  };

 private:
  std::queue<Request> m_queue;
  std::mutex m_mtx;
  Delegate::Weak m_weakDelegate;

 public:
  void setDelegate(
      ComponentInstancePreallocationRequestQueue::Delegate::Weak weakDelegate);

  void push(const Request& request);

  std::optional<Request> pop();

  void clear();

  bool isEmpty();
};
} // namespace rnoh
