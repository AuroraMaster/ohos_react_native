/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "ArkUINode.h"

namespace rnoh {
class CustomNodeDelegate {
 public:
  virtual ~CustomNodeDelegate() noexcept(false) = default;
  virtual void onClick(){};
  virtual void onHoverIn(){};
  virtual void onHoverOut(){};
};

struct UserCallback {
  std::function<void(ArkUI_NodeCustomEvent* event)> callback;
};

class CustomNode : public ArkUINode {
 protected:
  CustomNodeDelegate* m_customNodeDelegate;

 public:
  explicit CustomNode(const ArkUINode::Context::Shared& context = nullptr);
  ~CustomNode() override;

  void insertChild(ArkUINode& child, std::size_t index);
  void addChild(ArkUINode &child);
  void removeChild(ArkUINode& child);
  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;
  void onClick();
  void setCustomNodeDelegate(CustomNodeDelegate* customNodeDelegate);
  CustomNode& setAlign(int32_t align);
  CustomNode& setFocusable(bool focusable);
  void onMeasure(ArkUI_NodeCustomEventType eventType);
  void onLayout();
 private:
  UserCallback *userCallback_ = nullptr;
  void (*eventReceiver)(ArkUI_NodeCustomEvent* event);
};

} // namespace rnoh