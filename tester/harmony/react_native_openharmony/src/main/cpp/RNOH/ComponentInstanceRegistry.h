/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * Used only in C-API based Architecture.
 */
#pragma once
#include <memory>
#include <thread>

#include <react/renderer/core/ReactPrimitives.h>
#include "RNOH/Assert.h"
#include "RNOH/ComponentInstance.h"

namespace rnoh {
/**
 * @Thread: MAIN
 *
 * ComponentInstanceRegistry stores the ComponentInstance objects and allows
 * retrieving them by tag or id.
 */
class ComponentInstanceRegistry : public ComponentInstance::Registry {
 public:
  using Shared = std::shared_ptr<ComponentInstanceRegistry>;

  ~ComponentInstanceRegistry() {
    DLOG(INFO) << "~ComponentInstanceRegistry";
  }

  ComponentInstance::Shared findByTag(facebook::react::Tag tag) const {
    std::lock_guard<std::mutex> lock(m_componentInstanceByTagMtx);
    auto it = m_componentInstanceByTag.find(tag);
    if (it != m_componentInstanceByTag.end()) {
      return it->second;
    }
    return nullptr;
  }

  std::optional<facebook::react::Tag> findTagById(const std::string& id) const {
    std::lock_guard<std::mutex> lock(m_tagByIdMtx);
    auto it = m_tagById.find(id);
    if (it != m_tagById.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  ComponentInstance::Shared findById(const std::string& id) const override {
    auto maybeTag = this->findTagById(id);
    if (!maybeTag.has_value()) {
      return nullptr;
    }
    return this->findByTag(maybeTag.value());
  }

  void insert(ComponentInstance::Shared componentInstance) {
    auto tag = componentInstance->getTag();
    std::lock_guard<std::mutex> lock(m_componentInstanceByTagMtx);
    m_componentInstanceByTag.emplace(tag, std::move(componentInstance));
  }

  void updateTagById(
      facebook::react::Tag tag,
      const std::string& id,
      const std::string prevId) {
    std::lock_guard<std::mutex> lock(m_tagByIdMtx);
    if (!prevId.empty() && m_tagById.find(prevId) != m_tagById.end()) {
      m_tagById.erase(prevId);
    }
    if (!id.empty()) {
      m_tagById.emplace(id, tag);
    }
  }

  void deleteByTag(facebook::react::Tag tag) {
    std::unordered_map<facebook::react::Tag, ComponentInstance::Shared>::
        iterator tagAndComponentInstance;
    {
      std::lock_guard<std::mutex> lock(m_componentInstanceByTagMtx);
      tagAndComponentInstance = m_componentInstanceByTag.find(tag);
      if (tagAndComponentInstance == m_componentInstanceByTag.end()) {
        return;
      }
    }
    auto componentInstance = tagAndComponentInstance->second;
    auto componentInstanceId = componentInstance->getId();
    if (!componentInstanceId.empty()) {
      std::lock_guard<std::mutex> lock(m_tagByIdMtx);
      m_tagById.erase(componentInstanceId);
    }

    std::lock_guard<std::mutex> lock(m_componentInstanceByTagMtx);
    m_componentInstanceByTag.erase(tag);
  }

  private:
  std::thread::id m_mainThreadId = std::this_thread::get_id();
  std::unordered_map<facebook::react::Tag, ComponentInstance::Shared>
      m_componentInstanceByTag = {};
  std::unordered_map<std::string, facebook::react::Tag> m_tagById = {};
  mutable std::mutex m_componentInstanceByTagMtx;
  mutable std::mutex m_tagByIdMtx;

  void assertMainThread() const {
    RNOH_ASSERT_MSG(
        m_mainThreadId == std::this_thread::get_id(),
        "ComponentInstanceRegistry must only be accessed on the main thread");
  }
};
} // namespace rnoh