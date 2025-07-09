/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/modal/ModalHostViewShadowNode.h>
#include "RNOH/ArkTSBridge.h"
#include "RNOH/ArkTSMessageHub.h"
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/ArkUIDialogHandler.h"
#include "RNOH/arkui/CustomNode.h"
#include "RNOH/arkui/StackNode.h"
#include "RNOH/arkui/UIInputEventHandler.h"

namespace rnoh {
class ModalHostViewComponentInstance
    : public CppComponentInstance<facebook::react::ModalHostViewShadowNode>,
      public ArkUIDialogDelegate,
      public ArkTSMessageHub::Observer {
 private:
  enum FoldStatus : uint32_t { FOLD_STATUS_UNKNOWN, FOLD_STATUS_EXPANDED, FOLD_STATUS_FOLDED, FOLD_STATUS_HALF_FOLDED };
  using ScreenOrientation = facebook::react::ModalHostViewEventEmitter::
      OnOrientationChangeOrientation;
  CustomNode m_virtualNode;
  StackNode m_rootStackNode;
  CustomNode m_rootCustomNode;
  ArkUIDialogHandler m_dialogHandler;
  std::shared_ptr<UIInputEventHandler> m_touchHandler;
  std::shared_ptr<ArkTSMessageHandler> m_displaySizeChangeHandler;
  std::optional<ScreenOrientation> m_screenOrientation;

 private:
  void updateDisplaySize(
      DisplayMetrics const& displayMetrics,
      SharedConcreteState const& state);
    
    void resetModalPosition(
      DisplayMetrics const& displayMetrics,
      SharedConcreteState const& state);
  void updateSlideTransition(DisplayMetrics const& displayMetrics);

 public:
  ModalHostViewComponentInstance(Context context);
    
  void onPropsChanged(SharedConcreteProps const& props) override;
  void onStateChanged(SharedConcreteState const& state) override;
  void setLayout(facebook::react::LayoutMetrics layoutMetrics) override;

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;

  void onFinalizeUpdates() override;

  void showDialog();

  void onShow() override;

  void onRequestClose() override;

  CustomNode& getLocalRootArkUINode() override;

  // ArkTSMessageHub::Observer
  void onMessageReceived(ArkTSMessage const& message) override;

  friend class ModalHostTouchHandler;
};
} // namespace rnoh