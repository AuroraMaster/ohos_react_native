/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/view/ViewShadowNode.h>
#include "ComponentInstance.h"
#include "RNArkTSComponentDelegate.h"
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/ArkUINode.h"
#include "RNOH/arkui/TouchEventDispatcher.h"
#include "arkui/NodeContentHandle.h"
#include "arkui/StackNode.h"

namespace rnoh {

using ArkTSComponentDelegateGetter = std::function<std::optional<std::shared_ptr<RNArkTSComponentDelegate>>()>;
/**
 * This ComponentInstance is created when no other ComponentInstance was found.
 * It is used for backward compatibility reasons with ArkTS-based architecture.
 */
class FallbackComponentInstance
    : public CppComponentInstance<facebook::react::ViewShadowNode>,
      public ArkTSMessageHub::Observer {
 private:
  // NOTE: the order matters. `m_arkUINode` must be deleted before
  // `m_arkUIBuilderNodeDeleter`
  folly::Function<void()> m_arkUIBuilderNodeDeleter;
  ArkTSComponentDelegateGetter m_arkTSComponentDelegateGetter;
  std::unique_ptr<ArkUINode> m_arkUINode;
  StackNode m_stackNode;
  NodeContentHandle m_contentHandle;
  TouchEventDispatcher m_touchEventDispatcher;
  bool m_isRootTouchTarget = false;

 public:
  FallbackComponentInstance(
      Context ctx,
      std::unique_ptr<ArkUINode> arkUINode,
      NodeContentHandle contentHandle,
      folly::Function<void()> arkUIBuilderNodeDeleter,
      ArkTSComponentDelegateGetter arkTSComponentDelegateGetter)
      : CppComponentInstance(ctx),
        m_arkUIBuilderNodeDeleter(std::move(arkUIBuilderNodeDeleter)),
        m_arkTSComponentDelegateGetter(std::move(arkTSComponentDelegateGetter)),
        m_arkUINode(std::move(arkUINode)),
        m_contentHandle(std::move(contentHandle)),
        ArkTSMessageHub::Observer(m_deps->arkTSMessageHub),
        m_stackNode(ctx.arkUINodeContext) {
    m_arkUINode->setArkUINodeDelegate(this);
    m_stackNode.insertChild(*m_arkUINode, 0);
  };

  ArkUINode& getLocalRootArkUINode() override {
    return m_stackNode;
  };

  void onArkUINodeDestroy(ArkUINode* /*node*/) override {
    m_arkUIBuilderNodeDeleter();
  }

  void onLayoutChanged(
      facebook::react::LayoutMetrics const& layoutMetrics) override {
    m_stackNode.setLayoutRect(
      layoutMetrics.frame.origin, layoutMetrics.frame.size, layoutMetrics.pointScaleFactor);
  }

  facebook::react::Point getCurrentOffset() const override {
    auto delegate = m_arkTSComponentDelegateGetter();
    if (delegate.has_value()) {
      auto position = delegate->get()->callSync("getCurrentOffset", {});
      if (position.isObject()) {
        auto x = position["x"].asDouble();
        auto y = position["y"].asDouble();
        return {x, y};
      }
    }
    return {0, 0};
  }

 protected:
  void onPropsChanged(const SharedConcreteProps& /*props*/) override {
    // NOOP: Props are set on ArkTS side.
  }

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override {
    ComponentInstance::onChildInserted(childComponentInstance, index);
    m_contentHandle.insertNode(
        childComponentInstance->getLocalRootArkUINode(), index);
  }

  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override {
    ComponentInstance::onChildRemoved(childComponentInstance);
    m_contentHandle.removeNode(childComponentInstance->getLocalRootArkUINode());
  }
  
  bool isRootTouchTarget() const override {
    return m_isRootTouchTarget;
  }

  void onMessageReceived(ArkTSMessage const& message) override {
    if (message.name == "RNOH::TOUCH_EVENT" &&
        message.payload["tag"].asInt() == this->getTag()) {
      m_isRootTouchTarget = true;
      TouchEvent touchEvent(message.payload["touchEvent"]);
      m_touchEventDispatcher.dispatchTouchEvent(touchEvent, shared_from_this());
    }
  }
};
} // namespace rnoh
