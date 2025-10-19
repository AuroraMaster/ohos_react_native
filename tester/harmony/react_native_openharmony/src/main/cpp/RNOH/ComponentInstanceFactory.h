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
#include <glog/logging.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <memory>
#include <vector>
#include "RNOH/ComponentInstance.h"
#include "RNOH/CustomComponentArkUINodeHandleFactory.h"
#include "RNOH/FallbackComponentInstance.h"
#include "RNOH/ThreadGuard.h"

namespace rnoh {
class ComponentInstanceFactoryDelegate {
 public:
  ComponentInstanceFactoryDelegate() = default;

  using Shared = std::shared_ptr<ComponentInstanceFactoryDelegate>;

  virtual ComponentInstance::Shared create(ComponentInstance::Context ctx) = 0;
};

/**
 * @thread: MAIN
 * Used by the ComponentRegistry to create ComponentInstances.
 */
class ComponentInstanceFactory {
 public:
  using Shared = std::shared_ptr<ComponentInstanceFactory>;

  ComponentInstanceFactory(
      std::vector<ComponentInstanceFactoryDelegate::Shared> delegates,
      ComponentInstance::Dependencies::Shared dependencies,
      CustomComponentArkUINodeHandleFactory::Shared
          customComponentArkUINodeHandleFactory)
      : m_delegates(std::move(delegates)),
        m_dependencies(dependencies),
        m_customComponentArkUINodeHandleFactory(
            customComponentArkUINodeHandleFactory) {}

  ComponentInstance::Shared createArkTSComponent(
      facebook::react::Tag tag,
      facebook::react::ComponentHandle componentHandle,
      std::string componentName) {
    m_threadGuard.assertThread();
    ComponentInstance::Context ctx = {
        .tag = tag,
        .componentHandle = componentHandle,
        .componentName = componentName,
        .dependencies = m_dependencies,
        .arkUINodeContext = m_arkUINodeContext};

    auto maybeCustomComponentWrapper =
        m_customComponentArkUINodeHandleFactory->create(tag, componentName);
    if (maybeCustomComponentWrapper.has_value()) {
      auto [nodeHandle, nodeContent, deleter] =
          std::move(maybeCustomComponentWrapper.value());
      auto arkUINode = std::make_unique<ArkUINode>(nodeHandle);
      auto arkUIComponentInstance = std::make_shared<FallbackComponentInstance>(
          ctx,
          std::move(arkUINode),
          std::move(nodeContent),
          std::move(deleter));
      return arkUIComponentInstance;
    }
    return nullptr;
  }

  ComponentInstance::Shared create(
      facebook::react::Tag tag,
      facebook::react::ComponentHandle componentHandle,
      std::string componentName) {
    ComponentInstance::Context ctx = {
        .tag = tag,
        .componentHandle = componentHandle,
        .componentName = componentName,
        .dependencies = m_dependencies,
        .arkUINodeContext = m_arkUINodeContext};
    for (auto& delegate : m_delegates) {
      auto componentInstance = delegate->create(ctx);
      if (componentInstance != nullptr) {
        componentInstance->onCreate();
        return componentInstance;
      }
    }
    return nullptr;
  }

  void setArkUINodeContext(const ArkUINode::Context::Shared& arkUINodeContext) {
    m_threadGuard.assertThread();
    m_arkUINodeContext = arkUINodeContext;
  }

  private:
    std::vector<ComponentInstanceFactoryDelegate::Shared> m_delegates;
    ComponentInstance::Dependencies::Shared m_dependencies;
    CustomComponentArkUINodeHandleFactory::Shared
        m_customComponentArkUINodeHandleFactory;
    ThreadGuard m_threadGuard{};
    ArkUINode::Context::Shared m_arkUINodeContext;
};
} // namespace rnoh