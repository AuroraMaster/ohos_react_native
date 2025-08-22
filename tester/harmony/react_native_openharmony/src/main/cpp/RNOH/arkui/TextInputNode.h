/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "TextInputNodeBase.h"
#include "react/renderer/components/textinput/TextInputProps.h"
#include "react/renderer/graphics/Color.h"

namespace rnoh {

class TextInputNodeDelegate {
 public:
  virtual ~TextInputNodeDelegate() = default;
  virtual void onChange(std::string text) {};
  virtual void onBlur() {};
  virtual void onFocus() {};
  virtual void onSubmit() {};
  virtual void onPasteOrCut() {};
  virtual void onContentScroll() {};
  virtual void onContentSizeChange(float width, float height, bool multiline) {};
  /**
   * @brief Callback that is called when the NODE_TEXT_INPUT_ON_WILL_DELETE
   * event is fired, eg. when text is about to be deleted.
   * @param node The text area's node.
   * @param position Index of the text to be deleted.
   * @param direction The direction for deleting the text, with 0 indicating
   * backward-delete and 1 indicating forward-delete.
   */
  virtual void onWillDelete(ArkUINode* node, int position, int direction) {}

  virtual void onTextSelectionChange(int32_t location, int32_t length){};
};

class TextInputNode : public TextInputNodeBase {
 private:
  uint32_t m_caretColorValue;
  bool m_autofocus{false};
  bool m_setTextContent{false};
  std::string m_textContent;
 protected:
  TextInputNodeDelegate* m_textInputNodeDelegate;

 public:
  explicit TextInputNode(const ArkUINode::Context::Shared& context = nullptr);
  ~TextInputNode() override;

  facebook::react::Point getTextInputOffset() const;

  facebook::react::Rect getTextContentRect() const override;

  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;

  void onNodeEvent(ArkUI_NodeEventType eventType, std::string_view eventString)
      override;

  void onNodeEvent(ArkUI_NodeEventType eventType, ArkUI_NodeEvent* event)
      override;

  void setTextInputNodeDelegate(TextInputNodeDelegate* textInputNodeDelegate);

  void setTextContent(std::string const& textContent);

  void setCaretHidden(bool hidden);

  void setInputType(ArkUI_TextInputType keyboardType);

  void setSelectedBackgroundColor(facebook::react::SharedColor const& color);

  void setPasswordIconVisibility(bool isVisible);

  void setEnterKeyType(ArkUI_EnterKeyType returnKeyType);

  void setCancelButtonMode(
      facebook::react::TextInputAccessoryVisibilityMode mode);

  void setFont(facebook::react::TextAttributes const& textAttributes) override;

  void setCaretColor(facebook::react::SharedColor const& color) override;

  void setMaxLength(int32_t maxLength) override;

  void setPlaceholder(std::string const& placeholder) override;

  void setPlaceholderColor(facebook::react::SharedColor const& color) override;

  void resetSelectedBackgroundColor();

  void setPasswordRules(std::string rules);
  
  void setUnderlineColorAndroid(facebook::react::SharedColor const& underlineColorAndroid);
  
  void SetContextMenuHidden(bool const& hidden);
  
  void setTextContentType(std::string const& textContentType);
  
  void setAutoFill(bool autoFill);
  
  void setBlurOnSubmit(bool blurOnSubmit);

  void setshowSoftInputOnFocus(int32_t enable);

  void setInputFilter(std::string const& inputFilter);

  void setAutoFocus(bool const &autoFocus);

  void setSelectAll(bool selectAll);

  /**
   * @Deprecated: use isFocused.
   */
  bool getTextFocusStatus();

  std::string getTextContent() override;
};

} // namespace rnoh