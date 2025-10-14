/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <unordered_map>

#include <react/renderer/mounting/ShadowView.h>

namespace rnoh {

class ShadowViewRegistry {
 public:
  using Shared = std::shared_ptr<ShadowViewRegistry>;

  void setShadowView(facebook::react::Tag, facebook::react::ShadowView const&);
  void clearShadowView(facebook::react::Tag);

  ~ShadowViewRegistry() {
    DLOG(INFO) << "~ShadowViewRegistry";
  }

  template <typename TEventEmitter>
  std::shared_ptr<const TEventEmitter> getEventEmitter(
      facebook::react::Tag tag) {
    auto it = m_shadowViewEntryByTag.find(tag);
    if (it != m_shadowViewEntryByTag.end()) {
      return std::dynamic_pointer_cast<const TEventEmitter>(
          it->second.eventEmitter.lock());
    }
    return nullptr;
  }

  template <typename TState>
  std::shared_ptr<TState const> getFabricState(facebook::react::Tag tag) {
    auto it = m_shadowViewEntryByTag.find(tag);
    if (it != m_shadowViewEntryByTag.end()) {
      return std::dynamic_pointer_cast<const TState>(it->second.state.lock());
    }
    return nullptr;
  }

  std::string getComponentName(facebook::react::Tag tag);

 private:
  using WeakEventEmitter = std::weak_ptr<facebook::react::EventEmitter const>;
  using WeakState = std::weak_ptr<facebook::react::State const>;

  struct ShadowViewEntry {
    WeakEventEmitter eventEmitter;
    WeakState state;
    std::string componentName;
  };

  std::unordered_map<facebook::react::Tag, ShadowViewEntry>
      m_shadowViewEntryByTag;
};

} // namespace rnoh
