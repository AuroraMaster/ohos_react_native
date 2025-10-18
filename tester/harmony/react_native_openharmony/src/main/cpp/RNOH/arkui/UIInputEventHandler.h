/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <arkui/native_node.h>
#include <arkui/ui_input_event.h>
#include "RNOH/arkui/ArkUINode.h"
#include "RNOH/TouchTarget.h"

namespace rnoh {

/**
 * @api: RN_LIBRARY_DEVELOPER
 *
 * Allows the given ArkUINode to handle touch events.
 * This is intended to be used by ArkUINodes that are "Roots" of the React
 * Native component trees.
 * Examples include the "RootView" and "ModalHostingView" components.
 */
class UIInputEventHandler {
 public:
  /**
   * Constructor.
   * NOTE: The node must outlive the UIInputEventHandler.
   */
  UIInputEventHandler(ArkUINode& node);
  virtual ~UIInputEventHandler() noexcept(false);
  virtual void onTouchEvent(ArkUI_UIInputEvent* event) = 0;
    
 private:
  ArkUINode& m_node;
};

} // namespace rnoh