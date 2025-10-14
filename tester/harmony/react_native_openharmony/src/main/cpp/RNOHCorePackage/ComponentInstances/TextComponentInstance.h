/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/text/ParagraphShadowNode.h>
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/StackNode.h"
#include "RNOH/arkui/TextNode.h"
#include "../../RNOH/TextMeasureRegistry.h"

namespace rnoh {
class TextComponentInstance
    : public CppComponentInstance<facebook::react::ParagraphShadowNode>,
      public TextNodeDelegate {
 private:
  using FragmentTouchTargetByTag =
      std::unordered_map<facebook::react::Tag, std::shared_ptr<TouchTarget>>;

  TextNode m_textNode{};
  StackNode* m_stackNodePtr = nullptr;
  std::vector<std::shared_ptr<ArkUINode>> m_childNodes{};
  FragmentTouchTargetByTag m_fragmentTouchTargetByTag{};
  bool m_touchTargetChildrenNeedUpdate = false;
  bool m_hasCheckNesting = false;
  std::string m_key;
  std::string m_textContent = "";

 public:
  TextComponentInstance(Context context);
  ~TextComponentInstance() override;
  ArkUINode& getLocalRootArkUINode() override;
  std::vector<TouchTarget::Shared> getTouchTargetChildren() override;
  bool checkUpdateBaseNode() override;
  void setShadowView(facebook::react::ShadowView const& shadowView) override;
 protected:
  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;
  void onPropsChanged(SharedConcreteProps const& props) override;
  void onStateChanged(SharedConcreteState const& textState) override;
  const std::string& getAccessibilityLabel() const override;
  void onFinalizeUpdates() override;
  
 private:
  void setTextAttributes(const facebook::react::TextAttributes& textAttributes);
  void updateFragmentTouchTargets(
      facebook::react::ParagraphState const& newState);
};
} // namespace rnoh