/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/mounting/ShadowViewMutation.h>
#include "RNOH/ComponentInstance.h"

namespace rnoh {

/**
 * @Thread: MAIN
 *
 * MountingManager is responsible for creating, updating and destroying
 * ComponentInstances
 *
 */
class MountingManager {
 protected:
  using Mutation = facebook::react::ShadowViewMutation;
  using MutationList = facebook::react::ShadowViewMutationList;

 public:
  using Shared = std::shared_ptr<MountingManager>;
  class Weak {
   public:
    Weak() = default;
    explicit Weak(
        std::weak_ptr<MountingManager> weakPtr,
        std::weak_ptr<std::atomic<bool>> willBeDestroyed)
        : m_weakPtr(weakPtr), m_willBeDestroyed(willBeDestroyed) {}

    std::shared_ptr<MountingManager> lock() const noexcept {
      auto willBeDestroyed = m_willBeDestroyed.lock();
      if (!willBeDestroyed || willBeDestroyed->load()) {
        return nullptr;
      }
      return m_weakPtr.lock();
    }
   private:
    std::weak_ptr<MountingManager> m_weakPtr;
    std::weak_ptr<std::atomic<bool>> m_willBeDestroyed;
  };

  virtual ~MountingManager() noexcept = default;

  virtual void willMount(MutationList const& mutations) = 0;

  virtual void doMount(MutationList const& mutations) = 0;

  virtual void didMount(MutationList const& mutations) = 0;
    
  virtual void handleMutation(Mutation const& mutation) = 0;

  virtual void finalizeMutationUpdates(MutationList const& mutations) = 0;
    
  virtual void dispatchCommand(
      const facebook::react::ShadowView& shadowView,
      const std::string& commandName,
      folly::dynamic const& args) = 0;

  virtual void setIsJsResponder(
      const facebook::react::ShadowView& shadowView,
      bool isJsResponder,
      bool blockNativeResponder) = 0;

     virtual void schedulerDidSendAccessibilityEvent(
      const facebook::react::ShadowView& shadowView,
      std::string const& eventType) {}

  virtual void updateView(
      facebook::react::Tag tag,
      folly::dynamic props,
      facebook::react::ComponentDescriptor const& componentDescriptor) = 0;

  virtual void clearPreallocatedViews() = 0;
  
  virtual void clearPreallocatedViews(
      facebook::react::ShadowViewMutationList mutations) = 0;
  virtual void clearPreallocationRequestQueue() = 0;
};
} // namespace rnoh
