/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "ArkUINode.h"

namespace rnoh {
class StackNodeDelegate {
 public:
  virtual ~StackNodeDelegate() = default;
  virtual void onClick() {};
  virtual void onHoverIn() {};
  virtual void onHoverOut() {};
};

class StackNode : public ArkUINode {
 protected:
  StackNodeDelegate* m_stackNodeDelegate;

 public:
  explicit StackNode(Context context = {});
  ~StackNode() override;

  void insertChild(ArkUINode& child, std::size_t index);
  void addChild(ArkUINode &child);
  void removeChild(ArkUINode& child);
  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;
  void onClick();
  void setStackNodeDelegate(StackNodeDelegate* stackNodeDelegate);
  StackNode& setAlign(int32_t align);
};

} // namespace rnoh