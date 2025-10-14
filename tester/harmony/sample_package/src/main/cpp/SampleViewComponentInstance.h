/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/CustomNode.h"
#include "RNOH/arkui/TextNode.h"
#include "SampleViewComponentDescriptor.h"

namespace rnoh {
class SampleViewComponentInstance
    : public CppComponentInstance<facebook::react::SampleViewShadowNode>,
      public CustomNodeDelegate {
 public:
  SampleViewComponentInstance(Context context);

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;
  void onPropsChanged(SharedConcreteProps const& props) override;
  void onCommandReceived(
      std::string const& commandName,
      folly::dynamic const& args) override;

  ArkUINode& getLocalRootArkUINode() override;

  // StackNodeDelegate
  void onClick() override;

 private:
  CustomNode m_customNode{};
  TextNode m_textNode{};
  int32_t m_fontSize = 12;
};
} // namespace rnoh