/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "TextInputComponentInstance.h"
#include "RNOH/arkui/conversions.h"
#include "conversions.h"

#include <folly/dynamic.h>
#include <glog/logging.h>
#include <react/renderer/components/textinput/TextInputProps.h>
#include <react/renderer/components/textinput/TextInputState.h>
#include <react/renderer/core/ConcreteState.h>
#include <sstream>
#include <utility>
#include <algorithm>
#include <boost/locale.hpp>
#include "ScrollViewComponentInstance.h"

namespace rnoh {

TextInputComponentInstance::TextInputComponentInstance(Context context)
    : CppComponentInstance(std::move(context)),
      ArkTSMessageHub::Observer(m_deps->arkTSMessageHub),
      m_textInputNode(m_arkUINodeCtx),
      m_textAreaNode(m_arkUINodeCtx) {}

void TextInputComponentInstance::onContentSizeChange(float width, float height, bool multiline) {
  if (multiline == m_multiline){
    m_contentSizeWidth = width;
    m_contentSizeHeight = height;
    m_eventEmitter->onContentSizeChange(getOnContentSizeChangeMetrics());
  }
}

void TextInputComponentInstance::onContentScroll() {
  m_eventEmitter->onScroll(getTextInputMetrics());
}

std::string TextInputComponentInstance::getTextContentFromState(SharedConcreteState const& state) {
  std::ostringstream contentStream;
  for (auto const& fragment :
      state->getData().attributedStringBox.getValue().getFragments()) {
    contentStream << fragment.string;
  }
  return contentStream.str();
}

void TextInputComponentInstance::onChange(std::string text) {
  m_nativeEventCount++;
  m_content = std::move(text);
  m_eventEmitter->onChange(getOnChangeMetrics());
  m_valueChanged = true;
}

void TextInputComponentInstance::onSubmit() {
  m_eventEmitter->onSubmitEditing(getTextInputMetrics());
}

void TextInputComponentInstance::onBlur() {
  this->m_focused = false;
  if (m_isControlledTextInput) {
    m_caretPositionForControlledInput = m_selectionStart;
  }
  if (m_props->traits.clearButtonMode ==
      facebook::react::TextInputAccessoryVisibilityMode::WhileEditing) {
    m_textInputNode.setCancelButtonMode(
        facebook::react::TextInputAccessoryVisibilityMode::Never);
  } else if (
      m_props->traits.clearButtonMode ==
      facebook::react::TextInputAccessoryVisibilityMode::UnlessEditing) {
    m_textInputNode.setCancelButtonMode(
        facebook::react::TextInputAccessoryVisibilityMode::Always);
  }
  m_eventEmitter->onBlur(getTextInputMetrics());
  m_eventEmitter->onEndEditing(getTextInputMetrics());
}

void TextInputComponentInstance::onFocus() {
  this->m_focused = true;
  if (this->m_clearTextOnFocus) {
    m_textAreaNode.setTextContent("");
    m_textInputNode.setTextContent("");
  }
  if (m_props->traits.clearButtonMode ==
      facebook::react::TextInputAccessoryVisibilityMode::WhileEditing) {
    m_textInputNode.setCancelButtonMode(m_props->traits.clearButtonMode);
  } else if (
      m_props->traits.clearButtonMode ==
      facebook::react::TextInputAccessoryVisibilityMode::UnlessEditing) {
    m_textInputNode.setCancelButtonMode(
        facebook::react::TextInputAccessoryVisibilityMode::Never);
  }
  m_eventEmitter->onFocus(getTextInputMetrics());
}

void TextInputComponentInstance::onPasteOrCut() {
  m_textWasPastedOrCut = true;
}

void TextInputComponentInstance::onWillDelete(
    ArkUINode* node,
    int position,
    int direction) {
  // onTextSelectionChange doesn't get triggered when backspace is pressed and
  // the TextInput is empty, so we cover that case here.
  if (m_eventEmitter && position == 0 && direction == 0) {
    auto keyPressMetrics =
        facebook::react::KeyPressMetrics();
    m_nativeEventCount++;
    keyPressMetrics.text = "";
    keyPressMetrics.eventCount = m_nativeEventCount;
    m_eventEmitter->onKeyPress(keyPressMetrics);
    // Use the onChange event to synchronize eventCount to ensure normal
    // rendering during long-press deletion.
    m_eventEmitter->onChange(getOnChangeMetrics());
  }
}

void TextInputComponentInstance::onTextSelectionChange(
    int32_t location,
    int32_t length) {
  if (m_isControlledTextInput &&
      m_hasLatestControlledValueChangeBeenProcessed) {
    m_caretPositionForControlledInput = m_selectionStart;
    /**
     * `m_valueChanged` is used here because "setTextAndSelection" command is
     * emitted only if value has changed.
     */
    if (m_valueChanged) {
      m_hasLatestControlledValueChangeBeenProcessed = false;
    }
  }
  if (m_textWasPastedOrCut) {
    m_textWasPastedOrCut = false;
  } else if (m_valueChanged) {
    std::u16string key;
    bool noPreviousSelection = m_selectionLength == 0;
    bool cursorDidNotMove = location == m_selectionLocation;
    bool cursorMovedBackwardsOrAtBeginningOfInput =
        (location < m_selectionLocation) || location <= 0;
    if (!cursorMovedBackwardsOrAtBeginningOfInput &&
        (noPreviousSelection || !cursorDidNotMove)) {
      auto utfContent = boost::locale::conv::utf_to_utf<char16_t>(m_content);
      if (location > 0 && location <= utfContent.size()) {
        int length = std::max(location - m_selectionLocation, 1);
        length = std::min(length, location);
        key = utfContent.substr(location - length, length);
      }
    }
    auto keyPressMetrics = facebook::react::KeyPressMetrics();
    keyPressMetrics.text = boost::locale::conv::utf_to_utf<char>(key);
    keyPressMetrics.eventCount = m_nativeEventCount;
    m_eventEmitter->onKeyPress(keyPressMetrics);
  }
  if (m_valueChanged) {
    m_valueChanged = false;
  }

  m_selectionLocation = location;
  m_selectionLength = length;
  m_selectionStart = location;
  m_selectionEnd = location + length;
  if (m_eventEmitter != NULL) {
    m_eventEmitter->onSelectionChange(getTextInputMetrics());
  }
}

facebook::react::TextInputMetrics
TextInputComponentInstance::getTextInputMetrics() {
  auto textInputMetrics = facebook::react::TextInputMetrics();

  auto contentOffset = m_multiline
      ? m_textAreaNode.getTextContentRect().origin
      : m_textInputNode.getTextContentRect().origin;
  float pointScaleFactor = m_layoutMetrics.pointScaleFactor;
  auto padding = m_layoutMetrics.contentInsets - m_layoutMetrics.borderWidth;
  contentOffset.x = contentOffset.x / pointScaleFactor - padding.left;
  contentOffset.y = contentOffset.y / pointScaleFactor - padding.top;
  contentOffset.x = std::max<float>(-contentOffset.x, 0.0f);
  contentOffset.y = std::max<float>(-contentOffset.y, 0.0f);
  textInputMetrics.contentOffset = contentOffset;

  textInputMetrics.containerSize = m_layoutMetrics.frame.size;

  textInputMetrics.eventCount = this->m_nativeEventCount;
  textInputMetrics.selectionRange.location = this->m_selectionLocation;
  textInputMetrics.selectionRange.length = this->m_selectionLength;
  textInputMetrics.contentSize.width = this->m_contentSizeWidth;
  textInputMetrics.contentSize.height = this->m_contentSizeHeight;
  textInputMetrics.zoomScale = 1;
  textInputMetrics.text = m_multiline
      ? m_textAreaNode.getTextContent()
      : m_textInputNode.getTextContent();
  return textInputMetrics;
}

facebook::react::OnChangeMetrics
TextInputComponentInstance::getOnChangeMetrics() {
  auto OnChangeMetrics = facebook::react::OnChangeMetrics();
  OnChangeMetrics.eventCount = this->m_nativeEventCount;
  OnChangeMetrics.text = m_multiline
    ? m_textAreaNode.getTextContent()
    : m_textInputNode.getTextContent();
  return OnChangeMetrics;
}

facebook::react::Size
TextInputComponentInstance::getOnContentSizeChangeMetrics() {
  auto OnContentSizeChangeMetrics = facebook::react::Size();
  OnContentSizeChangeMetrics.width= this->m_contentSizeWidth;
  OnContentSizeChangeMetrics.height = this->m_contentSizeHeight;
  return OnContentSizeChangeMetrics;
}

void TextInputComponentInstance::onPropsChanged(
    SharedConcreteProps const& props) {
  m_multiline = props->traits.multiline;
  m_isControlledTextInput = !props->text.empty();
  if (m_multiline) {
    m_textInputNode.setTextInputNodeDelegate(nullptr);
    m_textAreaNode.setTextAreaNodeDelegate(this);
  } else {
    m_textInputNode.setTextInputNodeDelegate(this);
    m_textAreaNode.setTextAreaNodeDelegate(nullptr);
  }
  CppComponentInstance::onPropsChanged(props);
  m_clearTextOnFocus = props->traits.clearTextOnFocus;
  if (!m_props) {
    m_textInputNode.setInputFilter(".*");
    m_textAreaNode.setInputFilter(".*");
    /* Initialize m_content to accurately calculate the cursor position */
    m_content = props->text;
  }
  if (!m_props ||
      props->traits.showSoftInputOnFocus != m_props->traits.showSoftInputOnFocus){
    m_textAreaNode.setshowSoftInputOnFocus((int32_t)props->traits.showSoftInputOnFocus);
    m_textInputNode.setshowSoftInputOnFocus((int32_t)props->traits.showSoftInputOnFocus);
  }
  if (!m_props || props->importantForAutofill != m_props->importantForAutofill) {
    m_textAreaNode.setAutoFill(convertImportantForAutofill(props->importantForAutofill));
    m_textInputNode.setAutoFill(convertImportantForAutofill(props->importantForAutofill));
  }
  if (!m_props ||
      props->traits.textContentType != m_props->traits.textContentType) {
    m_textInputNode.setTextContentType(props->traits.textContentType);
    m_textAreaNode.setTextContentType(props->traits.textContentType);
  }
  if (!m_props || props->traits.contextMenuHidden != m_props->traits.contextMenuHidden){
    m_textInputNode.SetContextMenuHidden(props->traits.contextMenuHidden);
    m_textAreaNode.SetContextMenuHidden(props->traits.contextMenuHidden);
  }
  if (!m_props || *(props->underlineColorAndroid) != *(m_props->underlineColorAndroid)) {
    m_textInputNode.setUnderlineColorAndroid(props->underlineColorAndroid);
    m_textAreaNode.setUnderlineColorAndroid(props->underlineColorAndroid);
  }
  if (!m_props ||
        props->traits.passwordRules != 
          m_props->traits.passwordRules) {
      m_textInputNode.setPasswordRules(props->traits.passwordRules);
  }
  if (!m_props ||
      *(props->textAttributes.foregroundColor) !=
          *(m_props->textAttributes.foregroundColor)) {
    if (props->textAttributes.foregroundColor) {
      m_textAreaNode.setFontColor(props->textAttributes.foregroundColor);
      m_textInputNode.setFontColor(props->textAttributes.foregroundColor);
    } else {
      m_textAreaNode.setFontColor(facebook::react::blackColor());
      m_textInputNode.setFontColor(facebook::react::blackColor());
    }
  }
  if (!m_props || props->textAttributes != m_props->textAttributes) {
    m_textAreaNode.setFont(props->textAttributes);
    m_textInputNode.setFont(props->textAttributes);
  }
  if (!m_props ||
    props->textAttributes.lineHeight != m_props->textAttributes.lineHeight) {
    if (props->textAttributes.lineHeight) {
      m_textAreaNode.setTextInputLineHeight(props->textAttributes);
      m_textInputNode.setTextInputLineHeight(props->textAttributes);
    }
  }  
  if (!m_props || *(props->backgroundColor) != *(m_props->backgroundColor)) {
    if (props->backgroundColor) {
      m_textAreaNode.setBackgroundColor(props->backgroundColor);
      m_textInputNode.setBackgroundColor(props->backgroundColor);
    } else {
      m_textAreaNode.setBackgroundColor(facebook::react::clearColor());
      m_textInputNode.setBackgroundColor(facebook::react::clearColor());
    }
  }
  if (props->rawProps.count("textAlignVertical") != 0){
    std::string rawAlignment = props->rawProps["textAlignVertical"].asString();
    ArkUI_Alignment aligment;
    if (rawAlignment == "top"){
      aligment = ARKUI_ALIGNMENT_TOP;
    } else if(rawAlignment == "center"){
      aligment = ARKUI_ALIGNMENT_CENTER;
    } else if(rawAlignment == "bottom"){
      aligment = ARKUI_ALIGNMENT_BOTTOM;
    } else if(rawAlignment == "auto"){
      aligment = ARKUI_ALIGNMENT_CENTER;
    }
    m_textInputNode.setAlignment(aligment);
    m_textAreaNode.setAlignment(aligment);
  }
  if (props->textAttributes.alignment) {
    if (!m_props ||
        *(props->textAttributes.alignment) !=
            *(m_props->textAttributes.alignment)) {
      m_textAreaNode.setTextAlign(props->textAttributes.alignment);
      m_textInputNode.setTextAlign(props->textAttributes.alignment);
    }
  }
  if (!m_props || *(props->cursorColor) != *(m_props->cursorColor)) {
    if (props->cursorColor) {
      m_textAreaNode.setCaretColor(props->cursorColor);
      m_textInputNode.setCaretColor(props->cursorColor);
    } else {
      m_textAreaNode.setCaretColor(facebook::react::blackColor());
      m_textInputNode.setCaretColor(facebook::react::blackColor());
    }
  }
  if (!m_props || props->traits.keyboardType != m_props->traits.keyboardType) {
    if(m_multiline){
      if(props->traits.keyboardType == facebook::react::KeyboardType::DecimalPad){
        m_textAreaNode.setInputType(rnoh::convertTextAreaInputType(facebook::react::KeyboardType::Numeric));
      }else{
        m_textAreaNode.setInputType(rnoh::convertTextAreaInputType(props->traits.keyboardType));
      }
    }else{
      m_textInputNode.setInputType(
        props->traits.secureTextEntry
            ? ARKUI_TEXTINPUT_TYPE_PASSWORD
            : rnoh::convertInputType(props->traits.keyboardType));
      m_textInputNode.setPasswordIconVisibility(false);      
    }
  }
  if (!m_props || props->maxLength != m_props->maxLength) {
    if (!props->maxLength) {
      m_textAreaNode.resetMaxLength();
      m_textInputNode.resetMaxLength();
    } else {
      m_textAreaNode.setMaxLength(props->maxLength);
      m_textInputNode.setMaxLength(props->maxLength);
    }
  }
  if (!m_props || props->placeholder != m_props->placeholder) {
    m_textAreaNode.setPlaceholder(props->placeholder);
    m_textInputNode.setPlaceholder(props->placeholder);
  }
  if (props->placeholderTextColor) {
    if (!m_props ||
        *(props->placeholderTextColor) != *(m_props->placeholderTextColor)) {
      m_textAreaNode.setPlaceholderColor(props->placeholderTextColor);
      m_textInputNode.setPlaceholderColor(props->placeholderTextColor);
    }
  }
  if (props->rawProps.count("focusable") > 0) {
    if (!m_props ||
        props->rawProps["focusable"].asBool() !=
            m_props->rawProps["focusable"].asBool()) {
      m_textAreaNode.setFocusable(props->rawProps["focusable"].asBool());
      m_textInputNode.setFocusable(props->rawProps["focusable"].asBool());
    }
  }
  m_textAreaNode.setId(getIdFromProps(props));
  m_textInputNode.setId(getIdFromProps(props));

  if (!m_props || props->autoFocus != m_props->autoFocus) {
    m_autoFocus = props->autoFocus;
    if (m_multiline == true) {
        m_textAreaNode.setAutoFocus(props->autoFocus);
    } else if (m_multiline == false) {
        m_textInputNode.setAutoFocus(props->autoFocus);
    }
  }
  if (!m_props || props->selection->start != m_props->selection->start ||
    props->selection->end != m_props->selection->end) {
    if (m_multiline == true) {
        m_textAreaNode.setTextSelection(props->selection->start, props->selection->end);
    } else {
        m_textInputNode.setTextSelection(props->selection->start, props->selection->end);
    }
  }
  if (!m_props || *(props->selectionColor) != *(m_props->selectionColor)) {
    if (props->selectionColor) {
      m_textInputNode.setSelectedBackgroundColor(props->selectionColor);
      if (!props->cursorColor) {
        m_textInputNode.setCaretColor(props->selectionColor);
        m_textAreaNode.setCaretColor(props->selectionColor);
      }
    } else {
      m_textInputNode.resetSelectedBackgroundColor();
    }
  }
  if (!m_props ||
      props->traits.secureTextEntry != m_props->traits.secureTextEntry ||
      props->traits.keyboardType != m_props->traits.keyboardType) {
    m_textInputNode.setInputType(
        props->traits.secureTextEntry
            ? ARKUI_TEXTINPUT_TYPE_PASSWORD
            : rnoh::convertInputType(props->traits.keyboardType));
    m_textInputNode.setPasswordIconVisibility(false);        
  }
  if (!m_props || props->traits.caretHidden != m_props->traits.caretHidden) {
    m_textInputNode.setCaretHidden(props->traits.caretHidden);
  }
  if (!m_props ||
      props->traits.returnKeyType != m_props->traits.returnKeyType ||
      props->traits.returnKeyLabel != m_props->traits.returnKeyLabel) {
    auto returnKeyType = rnoh::convertEnterKeyType(props->traits.returnKeyType);
    auto textAreaReturnKeyType =
        rnoh::convertTextAreaEnterKeyType(props->traits.returnKeyType);
    if (props->traits.returnKeyType ==
        facebook::react::ReturnKeyType::Default) {
      returnKeyType = rnoh::convertEnterKeyLabel(props->traits.returnKeyLabel);
      textAreaReturnKeyType =
          rnoh::convertTextAreaEnterKeyLabel(props->traits.returnKeyLabel);
    }
    m_textInputNode.setEnterKeyType(returnKeyType);
    m_textAreaNode.setEnterKeyType(textAreaReturnKeyType);
  }
  if (!m_props ||
      props->traits.clearButtonMode != m_props->traits.clearButtonMode) {
    if (m_focused) {
      if (props->traits.clearButtonMode ==
          facebook::react::TextInputAccessoryVisibilityMode::WhileEditing) {
        m_textInputNode.setCancelButtonMode(props->traits.clearButtonMode);
      } else if (
          props->traits.clearButtonMode ==
          facebook::react::TextInputAccessoryVisibilityMode::UnlessEditing) {
        m_textInputNode.setCancelButtonMode(
            facebook::react::TextInputAccessoryVisibilityMode::Never);
      }
    } else {
      if (props->traits.clearButtonMode ==
          facebook::react::TextInputAccessoryVisibilityMode::WhileEditing) {
        m_textInputNode.setCancelButtonMode(
            facebook::react::TextInputAccessoryVisibilityMode::Never);
      } else if (
          props->traits.clearButtonMode ==
          facebook::react::TextInputAccessoryVisibilityMode::UnlessEditing) {
        m_textInputNode.setCancelButtonMode(
            facebook::react::TextInputAccessoryVisibilityMode::Always);
      }
    }

    if (props->traits.clearButtonMode ==
            facebook::react::TextInputAccessoryVisibilityMode::Always ||
        props->traits.clearButtonMode ==
            facebook::react::TextInputAccessoryVisibilityMode::Never) {
      m_textInputNode.setCancelButtonMode(props->traits.clearButtonMode);
    }
  }
  if (!m_props ||
      !(props->yogaStyle.padding() == m_props->yogaStyle.padding())) {
    m_textInputNode.setPadding(resolveEdges(props->yogaStyle.padding()));
    m_textAreaNode.setPadding(resolveEdges(props->yogaStyle.padding()));
  }
  if (!m_props || props->blurOnSubmit != m_props->blurOnSubmit) {
    m_textInputNode.setBlurOnSubmit(props->blurOnSubmit);
    m_textAreaNode.setBlurOnSubmit(props->blurOnSubmit);
  }
  if (!m_props || props->traits.editable != m_props->traits.editable) {
    m_textAreaNode.setEnabled(props->traits.editable);
  m_textInputNode.setEnabled(props->traits.editable);
  }
  if (!m_props || props->traits.selectTextOnFocus != m_props->traits.selectTextOnFocus) {
    m_textInputNode.setSelectAll(props->traits.selectTextOnFocus);
    m_textAreaNode.setSelectAll(props->traits.selectTextOnFocus);
  }
}

void TextInputComponentInstance::onLayoutChanged(
    facebook::react::LayoutMetrics const& layoutMetrics) {
  CppComponentInstance::onLayoutChanged(layoutMetrics);
  if (m_multiline) {
     m_textInputNode.setLayoutRect(
        layoutMetrics.frame.origin,
        layoutMetrics.frame.size,
        layoutMetrics.pointScaleFactor);
  } else {
    m_textAreaNode.setLayoutRect(
        layoutMetrics.frame.origin,
        layoutMetrics.frame.size,
        layoutMetrics.pointScaleFactor);
  }
}

int32_t TextInputComponentInstance::countUtf16Characters(const std::string& content) {
  int32_t len = 0;
  const unsigned char* currentByte = reinterpret_cast<const unsigned char*>(content.data());
  const unsigned char* endOfBytes = currentByte + content.size();
  // Judge the length of UTF-8 characters based on the number of bits higher
  // than the first byte, take out the significant bits, and record the number
  // of continuation bytes that need to be read
  while (currentByte < endOfBytes) {
    uint32_t codePoint = 0;
    int continuationBytes = 0;
    if (*currentByte < 0x80) {
      codePoint = *currentByte++;
    } else if ((*currentByte >> 5) == 0x6) {
      codePoint = *currentByte & 0x1F;
      continuationBytes = 1;
      ++currentByte;
    } else if ((*currentByte >> 4) == 0xE) {
      codePoint = *currentByte & 0x0F;
      continuationBytes = 2;
      ++currentByte;
    } else if ((*currentByte >> 3) == 0x1E) {
      codePoint = *currentByte & 0x07;
      continuationBytes = 3;
      ++currentByte;
    } else {
      ++currentByte;
      continue;
    }
    // The remaining bytes are taken and the length is calculated
    while (continuationBytes-- && currentByte < endOfBytes &&
           ((*currentByte & 0xC0) == 0x80)) {
      codePoint = (codePoint << 6) | (*currentByte++ & 0x3F);
    }
    // The number of code elements is calculated according to the hexadecimal
    // format
    if (codePoint < 0x10000) {
      len += 1;
    } else {
      len += 2;
    }
  }
  return len;
}
void TextInputComponentInstance::setTextContentAndSelection(std::string const &content, size_t selectionStart, size_t selectionEnd) {
   if (selectionStart > selectionEnd) {
      // swap to match behavior on Android
      std::swap(selectionStart, selectionEnd);
    }
    m_textInputNode.setTextContent(content);      
    m_textAreaNode.setTextContent(content);
    m_textInputNode.setTextSelection(selectionStart, selectionEnd);
    m_textAreaNode.setTextSelection(selectionStart, selectionEnd);
    m_selectionStart = selectionStart;
    m_selectionEnd = selectionEnd;
}

void TextInputComponentInstance::setTextContent(std::string const& content) {
  // NOTE: if selection isn't set explicitly by JS side, we want it to stay
  // roughly in the same place, rather than have it move to the end of the
  // input (which is the ArkUI default behaviour)
  //auto selectionFromEnd = m_content.size() - m_selectionLocation;
  if (m_selectionLocation == 0 && !content.empty() && m_autoFocus) {
    m_selectionLocation = countUtf16Characters(content);
  }
  int32_t contentLength = countUtf16Characters(content);
  int32_t selectionFromEnd = countUtf16Characters(m_content) - m_selectionLocation;
  int32_t selectionStart = contentLength - selectionFromEnd;
  int32_t selectionEnd = selectionStart + m_selectionLength;
  selectionStart = std::max(0, std::min(selectionStart, contentLength));
  selectionEnd = std::max(selectionStart, std::min(selectionEnd, contentLength));
  if (m_isControlledTextInput) {
    m_caretPositionForControlledInput = selectionStart;
  }
  setTextContentAndSelection(content, selectionStart, selectionEnd);
}

void TextInputComponentInstance::onCommandReceived(
    std::string const& commandName,
    folly::dynamic const& args) {
  if (commandName == "focus") {
    // When showSoftInputOnFocus is set to false, the keyboard opened by the
    // previous TextInput will be dismissed.
    if (m_props->traits.showSoftInputOnFocus == false) {
      blur();
    }
    focus();
    if (m_selectionStart != -1 && m_selectionEnd != -1 && !m_props->traits.selectTextOnFocus) {
      m_textInputNode.setTextSelection(m_selectionStart, m_selectionEnd);
      m_textAreaNode.setTextSelection(m_selectionStart, m_selectionEnd);
    }
    if (m_isControlledTextInput) {
      m_caretPositionForControlledInput = m_selectionStart;
    }
  } else if (commandName == "blur") {
    blur();
  } else if (
      commandName == "setTextAndSelection" && args.isArray() &&
      args.size() == 4 && args[0].asInt() >= m_nativeEventCount) {
    m_hasLatestControlledValueChangeBeenProcessed = true;
    auto textContent = args[1].asString();
    auto selectionStart = args[2].asInt();
    auto selectionEnd = args[3].asInt();
    if (selectionStart < 0) {
      if (m_isControlledTextInput) {
        setTextContentAndSelection(
            textContent,
            m_caretPositionForControlledInput,
            m_caretPositionForControlledInput);
      } else {
        setTextContent(textContent);
      }
    } else {
      setTextContentAndSelection(textContent, selectionStart, selectionEnd);
    }
  }
}

void TextInputComponentInstance::onStateChanged(
    SharedConcreteState const& state) {
  CppComponentInstance::onStateChanged(state);
  if (state->getData().mostRecentEventCount < this->m_nativeEventCount) {
    return;
  }

  auto content = getTextContentFromState(state);
  setTextContent(content);
}

ArkUINode& TextInputComponentInstance::getLocalRootArkUINode() {
  if (m_multiline) {
    return m_textAreaNode;
  }
  return m_textInputNode;
}

void TextInputComponentInstance::focus() {
  getLocalRootArkUINode().setFocusStatus(1);
}

void TextInputComponentInstance::blur() {
  getLocalRootArkUINode().setFocusStatus(0);
}

void TextInputComponentInstance::onMessageReceived(
    const ArkTSMessage& message) {
  if (message.name == "KEYBOARD_VISIBLE" && this->m_focused) {
    auto parent = this->m_parent.lock();
    std::shared_ptr<ScrollViewComponentInstance> scrollView = nullptr;
    while (parent != nullptr) {
      scrollView =
          std::dynamic_pointer_cast<ScrollViewComponentInstance>(parent);
      if (scrollView != nullptr) {
        scrollView->setKeyboardAvoider(shared_from_this());
        break;
      }
      parent = parent->getParent().lock();
    }
  }
}

// KeyboardAvoider
facebook::react::Float
TextInputComponentInstance::getBottomEdgeOffsetRelativeToScrollView(
    std::shared_ptr<ScrollViewComponentInstance> scrollView) {
  auto relativePos = m_layoutMetrics.frame.origin;
  auto parent = m_parent.lock();
  while (parent != nullptr && parent->getTag() != scrollView->getTag()) {
    relativePos += parent->getLayoutMetrics().frame.origin;
    parent = parent->getParent().lock();
  }
  /**
   * It looks like 24 is used by the platform when KeyboardAvoider hack isn't
   * needed.
   */
  auto GAP_BETWEEN_KEYBOARD_AND_TEXT_INPUT = 24;
  return relativePos.y + m_layoutMetrics.frame.size.height +
      GAP_BETWEEN_KEYBOARD_AND_TEXT_INPUT;
}

} // namespace rnoh
