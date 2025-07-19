/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "ArkUINode.h"

namespace rnoh {

class ToggleNodeDelegate {
 public:
  virtual ~ToggleNodeDelegate() = default;
  virtual void onValueChange(int32_t& value) = 0;
};

class ToggleNode : public ArkUINode {
 protected:
  ArkUI_NodeHandle m_childArkUINodeHandle;
  ToggleNodeDelegate* m_toggleNodeDelegate;

 public:
  explicit ToggleNode(Context context = {});
  ~ToggleNode();

  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs);
  void setToggleNodeDelegate(ToggleNodeDelegate* ToggleNodeDelegate);

  ToggleNode& setSelectedColor(facebook::react::SharedColor const& color);
  ToggleNode& setUnselectedColor(facebook::react::SharedColor const& color);
  ToggleNode& setThumbColor(facebook::react::SharedColor const& color);
  ToggleNode& setFocusable(bool focusable);

  bool getValue();
  ToggleNode& setValue(bool value);
};

} // namespace rnoh