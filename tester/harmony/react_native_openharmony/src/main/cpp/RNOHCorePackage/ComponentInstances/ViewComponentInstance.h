/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/view/ViewShadowNode.h>
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/StackNode.h"

namespace rnoh {
class ViewComponentInstance
    : public CppComponentInstance<facebook::react::ViewShadowNode>,
      public StackNodeDelegate {
 private:
  StackNode m_stackNode;

 public:
  ViewComponentInstance(Context context);

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;
  void onLayoutChanged(
      const facebook::react::LayoutMetrics& layoutMetrics) override;

  void onClick() override;
  void onHoverIn() override;
  void onHoverOut() override;
  StackNode& getLocalRootArkUINode() override;
};
} // namespace rnoh