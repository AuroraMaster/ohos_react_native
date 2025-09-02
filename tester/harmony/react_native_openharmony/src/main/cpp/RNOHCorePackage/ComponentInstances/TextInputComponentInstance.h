/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/TextAreaNode.h"
#include "RNOH/arkui/TextInputNode.h"
#include "ScrollViewComponentInstance.h"
#include "react/renderer/components/textinput/TextInputEventEmitter.h"
#include "react/renderer/components/textinput/TextInputShadowNode.h"

namespace rnoh {
class TextInputComponentInstance
    : public CppComponentInstance<facebook::react::TextInputShadowNode>,
      public TextInputNodeDelegate,
      public TextAreaNodeDelegate,
      public ArkTSMessageHub::Observer,
      public KeyboardAvoider {
 private:
  TextInputNode m_textInputNode;
  TextAreaNode m_textAreaNode;
  bool m_multiline{false};

  int32_t m_nativeEventCount = 0;

  static std::string getTextContentFromState(SharedConcreteState const& state);
  facebook::react::TextInputMetrics getTextInputMetrics();
  facebook::react::TextInputMetrics getTextInputMetrics(std::string text);
  facebook::react::OnChangeMetrics getOnChangeMetrics();
  facebook::react::Size getOnContentSizeChangeMetrics();
  int32_t getTextSize(const std::string &content);

  bool m_secureInput{false};

  bool m_clearTextOnFocus{false};

  std::string m_content;

  int32_t m_selectionLocation = 0;

  int32_t m_selectionLength = 0;

  float m_contentSizeWidth = 0;
  float m_contentSizeHeight = 0;

  bool m_textWasPastedOrCut = false;
  bool m_valueChanged = false;

  bool m_focused{false};
  bool m_shouldIgnoreNextChangeEvent = false;

  int32_t m_selectionStart = -1;
  int32_t m_selectionEnd = -1;

  /**
   * we want to record the focusing caret position when default value is set,
   * so that when user input something we could restore the caret
   * to the right position.
   */
  bool m_isControlledTextInput = false;
  /**
   * A desired caret position for controlled TextInput.
   * When TextInput is controlled, and props->value wasn't updated on JS side
   * after a key was pressed, ArkUI moves caret forward but caret position
   * shouldn't change.
   */
  int32_t m_caretPositionForControlledInput = 0;
  /**
   * When a user types two keys quickly (with two fingers), and TextInput value
   * is hardcoded on JS side, then a cursor can move forward. This property
   * prevents updating `m_caretPositionForControlledInput` too quickly.
   */
  bool m_hasLatestControlledValueChangeBeenProcessed = true;

  void focus();
  void blur();

  void setTextContentAndSelection(
      std::string const& content,
      size_t selectionStart,
      size_t selectionEnd);
  void setTextContent(std::string const& content);

 public:
  TextInputComponentInstance(Context context);

  void onPropsChanged(SharedConcreteProps const& props) override;

  void onStateChanged(SharedConcreteState const& state) override;

  void onLayoutChanged(
      facebook::react::LayoutMetrics const& layoutMetrics) override;

  void onCommandReceived(
      std::string const& commandName,
      folly::dynamic const& args) override;

  void onChange(std::string text) override;

  void onBlur() override;

  void onSubmit() override;

  void onFocus() override;

  void onPasteOrCut() override;

  void onTextSelectionChange(int32_t location, int32_t length) override;
  
  void onContentScroll() override;

  void onContentSizeChange(float width, float height, bool multiline) override;

  void onWillDelete(ArkUINode* node, int position, int direction) override;

  ArkUINode& getLocalRootArkUINode() override;

  void onMessageReceived(ArkTSMessage const& message) override;

  // KeyboardAvoider
  facebook::react::Float getBottomEdgeOffsetRelativeToScrollView(
      std::shared_ptr<ScrollViewComponentInstance> scrollView) override;
};
} // namespace rnoh