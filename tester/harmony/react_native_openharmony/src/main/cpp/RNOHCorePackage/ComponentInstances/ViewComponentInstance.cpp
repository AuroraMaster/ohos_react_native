/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ViewComponentInstance.h"
#include <RNOH/arkui/NativeNodeApi.h>

namespace rnoh {
ViewComponentInstance::ViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)),
      m_stackNode(context.arkUINodeContext) {
  getLocalRootArkUINode().setStackNodeDelegate(this);
}

void ViewComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance, std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    getLocalRootArkUINode().insertChild(
        childComponentInstance->getLocalRootArkUINode(), index);
}

void ViewComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance)
{
  CppComponentInstance::onChildRemoved(childComponentInstance);
  getLocalRootArkUINode().removeChild(
      childComponentInstance->getLocalRootArkUINode());
}

void ViewComponentInstance::onHoverIn() {
  if(m_eventEmitter != nullptr){
        m_eventEmitter->dispatchEvent(
          "pointerEnter", [=](facebook::jsi::Runtime& runtime) {
            auto payload = facebook::jsi::Object(runtime);
            return payload;
          });
      }
}

void ViewComponentInstance::onHoverOut() {
    if(m_eventEmitter != nullptr){
        m_eventEmitter->dispatchEvent(
          "pointerLeave", [=](facebook::jsi::Runtime& runtime) {
            auto payload = facebook::jsi::Object(runtime);
            return payload;
          });
    }
}

void ViewComponentInstance::onClick() {
  if (m_eventEmitter != nullptr) {
    m_eventEmitter->dispatchEvent(
        "click", [=](facebook::jsi::Runtime& runtime) {
          auto payload = facebook::jsi::Object(runtime);
          return payload;
        });
  }
}

StackNode& ViewComponentInstance::getLocalRootArkUINode()
{
    return m_stackNode;
}

void ViewComponentInstance::onLayoutChanged(
    const facebook::react::LayoutMetrics& layoutMetrics) {
  CppComponentInstance::onLayoutChanged(layoutMetrics);
  folly::dynamic payload = folly::dynamic::object("surfaceId", this->getTag())(
      "width", layoutMetrics.frame.size.width)(
      "height", layoutMetrics.frame.size.height);
  m_deps->arkTSChannel->postMessage(
      "RNOH::ROOT_COMPONENT_DIMENSIONS_CHANGED", payload);
}
} // namespace rnoh
