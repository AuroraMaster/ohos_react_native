/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "TextInputNode.h"
#include "NativeNodeApi.h"
#include "RNOH/arkui/conversions.h"
#include "conversions.h"
#include <deviceinfo.h>
#include "RNOH/arkui/DynamicArkUILoader.h"
#include "RNOH/ApiVersionCheck.h"

static constexpr std::array TEXT_INPUT_NODE_EVENT_TYPES = {
    NODE_TEXT_INPUT_ON_PASTE,
    NODE_TEXT_INPUT_ON_CUT,
    NODE_TEXT_INPUT_ON_CHANGE,
    NODE_TEXT_INPUT_ON_SUBMIT,
    NODE_ON_FOCUS,
    NODE_ON_BLUR,
    NODE_TEXT_INPUT_ON_TEXT_SELECTION_CHANGE,
    NODE_TEXT_INPUT_ON_CONTENT_SCROLL,
    NODE_TEXT_INPUT_ON_CONTENT_SIZE_CHANGE,
    NODE_TEXT_INPUT_ON_WILL_DELETE,
    NODE_EVENT_ON_APPEAR};

namespace rnoh {

// ARKUI_TEXTAREA_TYPE_ONE_TIME_CODE only support 
// API version >= 20, using const to avoid possible compiling issue. 
const int32_t TEXT_INPUT_TYPE_ONE_TIME_CODE = 14;

TextInputNode::TextInputNode(const ArkUINode::Context::Shared &context)
    : TextInputNodeBase(context, ArkUI_NodeType::ARKUI_NODE_TEXT_INPUT), m_textInputNodeDelegate(nullptr) {
    if (IsAtLeastApi15()) {
        registerNodeEvent(NODE_TEXT_INPUT_ON_CHANGE_WITH_PREVIEW_TEXT);
    }
    for (auto eventType : TEXT_INPUT_NODE_EVENT_TYPES) {
        registerNodeEvent(eventType);
    }

    ArkUI_NumberValue value = {.i32 = 1};
    ArkUI_AttributeItem item = {&value, 1};
    m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_ENABLE_KEYBOARD_ON_FOCUS, &item);
}

TextInputNode::~TextInputNode() {
    if (IsAtLeastApi15()) {
        unregisterNodeEvent(NODE_TEXT_INPUT_ON_CHANGE_WITH_PREVIEW_TEXT);
    }
    for (auto eventType : TEXT_INPUT_NODE_EVENT_TYPES) {
        unregisterNodeEvent(eventType);
    }
}

void TextInputNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  ArkUINode::onNodeEvent(eventType, eventArgs);
  if (eventType == ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_SUBMIT) {
    if (m_textInputNodeDelegate != nullptr) {
      m_textInputNodeDelegate->onSubmit();
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_ON_FOCUS) {
    if (m_textInputNodeDelegate != nullptr) {
      m_textInputNodeDelegate->onFocus();
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_ON_BLUR) {
    if (m_textInputNodeDelegate != nullptr) {
      m_textInputNodeDelegate->onBlur();
    }
  } else if (
      eventType ==
      ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_TEXT_SELECTION_CHANGE) {
    if (m_textInputNodeDelegate != nullptr) {
      int32_t selectionLocation = eventArgs[0].i32;
      int32_t selectionLength = eventArgs[1].i32 - eventArgs[0].i32;
      m_textInputNodeDelegate->onTextSelectionChange(
          selectionLocation, selectionLength);
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_CONTENT_SCROLL) {
    if (m_textInputNodeDelegate != nullptr) {
      m_textInputNodeDelegate->onContentScroll();
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_CONTENT_SIZE_CHANGE) {
    if (m_textInputNodeDelegate != nullptr) {
      float width = eventArgs[0].f32;
      float height = eventArgs[1].f32;
      m_textInputNodeDelegate->onContentSizeChange(width, height, false);
    }
 } else if (eventType == ArkUI_NodeEventType::NODE_EVENT_ON_APPEAR) {
    if (m_autofocus == true) {
      setFocusStatus(1);
    }
  }
}

void TextInputNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    std::string_view eventString) {
  if (eventType == ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_CHANGE) {
    if (m_textInputNodeDelegate != nullptr) {
      std::string text(eventString);
      if (m_setTextContent == true && text == m_textContent){ //it does not trigger onChange when using setTextContent
        m_setTextContent = false;
      } else {
        m_setTextContent = false;
        m_textInputNodeDelegate->onChange(std::move(text));
      }
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_PASTE) {
    if (m_textInputNodeDelegate != nullptr) {
      m_textInputNodeDelegate->onPasteOrCut();
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_CUT) {
    if (m_textInputNodeDelegate != nullptr) {
      m_textInputNodeDelegate->onPasteOrCut();
    }
  }
}

void TextInputNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    ArkUI_NodeEvent* event) {
  if (eventType == ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_WILL_DELETE) {
    ArkUI_NumberValue arkUiValues[2];
    maybeThrow(OH_ArkUI_NodeEvent_GetNumberValue(event, 0, arkUiValues));
    maybeThrow(OH_ArkUI_NodeEvent_GetNumberValue(event, 1, arkUiValues));

    ArkUI_NumberValue ret[] = {
        ArkUI_NumberValue{1},
    };
    // OH_ArkUI_NodeEvent_SetReturnNumberValue copies the value, so passing this
    // as a pointer to a variable on the stack should be safe.  (check the
    // arkui_ace_engine repository).
    // We need to set this or else the TextInput won't delete the character.
    maybeThrow(OH_ArkUI_NodeEvent_SetReturnNumberValue(event, ret, 1));
    m_textInputNodeDelegate->onWillDelete(
        this, static_cast<int>(round(arkUiValues[0].f32)), arkUiValues[1].i32);
  }
}

void TextInputNode::setTextInputNodeDelegate(
    TextInputNodeDelegate* textInputNodeDelegate) {
  m_textInputNodeDelegate = textInputNodeDelegate;
}

facebook::react::Point TextInputNode::getTextInputOffset() const {
  auto value = NativeNodeApi::getInstance()
                   ->getAttribute(m_nodeHandle, NODE_OFFSET)
                   ->value;
  facebook::react::Float x = value->i32;
  value++;
  facebook::react::Float y = value->i32;
  return facebook::react::Point{x, y};
}

facebook::react::Rect TextInputNode::getTextContentRect() const {
  auto value = getAttribute(NODE_TEXT_INPUT_CONTENT_RECT).value;
  facebook::react::Float x = value[0].f32;
  facebook::react::Float y = value[1].f32;
  facebook::react::Float width = value[2].f32;
  facebook::react::Float height = value[3].f32;
  return {x, y, width, height};
}

void TextInputNode::setTextContent(std::string const& textContent) {
  ArkUI_AttributeItem item = {.string = textContent.c_str()};
  m_setTextContent = true;
  m_textContent = textContent;
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_TEXT, &item);
}

void TextInputNode::setSelectedBackgroundColor(
    facebook::react::SharedColor const& color) {
  ArkUI_NumberValue selectedBackgroundColor = {
      .u32 = rnoh::convertColorToTranslucentSelection(color)};
  ArkUI_AttributeItem colorItem = {
      &selectedBackgroundColor, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_SELECTED_BACKGROUND_COLOR, &colorItem);
}

void TextInputNode::setCaretHidden(bool hidden) {
  if (hidden) {
    ArkUI_NumberValue value = {.f32 = 0};
    ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
    m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_CARET_STYLE, &item);

    value = {.u32 = 0};
    item = {&value, sizeof(ArkUI_NumberValue)};
    m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_CARET_COLOR, &item);
  } else {
    ArkUI_NumberValue value = {.f32 = 2};  //The default width of the cursor in ArkUI is 2 vp
    ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
    m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_CARET_STYLE, &item);

    value = {.u32 = m_caretColorValue};
    item = {&value, sizeof(ArkUI_NumberValue)};
    m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_CARET_COLOR, &item);
  }
}

void TextInputNode::setInputType(ArkUI_TextInputType keyboardType) {
  ArkUI_NumberValue value = {.i32 = keyboardType};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_TYPE, &item);
}

void TextInputNode::setPasswordIconVisibility(bool isVisible) {
  ArkUI_NumberValue value = {.i32 = isVisible ? 1 : 0};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_SHOW_PASSWORD_ICON, &item);
}

void TextInputNode::setEnterKeyType(
    ArkUI_EnterKeyType returnKeyType) {
  ArkUI_NumberValue value = {.i32 = returnKeyType};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_ENTER_KEY_TYPE, &item);
}

void TextInputNode::setCancelButtonMode(
    facebook::react::TextInputAccessoryVisibilityMode mode) {
  int32_t cancelButtonStyle =
      static_cast<int32_t>(ARKUI_CANCELBUTTON_STYLE_INVISIBLE);
  if (mode == facebook::react::TextInputAccessoryVisibilityMode::Always) {
    cancelButtonStyle = static_cast<int32_t>(ARKUI_CANCELBUTTON_STYLE_CONSTANT);
  } else if (
      mode == facebook::react::TextInputAccessoryVisibilityMode::WhileEditing) {
    cancelButtonStyle = static_cast<int32_t>(ARKUI_CANCELBUTTON_STYLE_INPUT);
  }

  ArkUI_NumberValue value[] = {{.i32 = cancelButtonStyle}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};

  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_CANCEL_BUTTON, &item);
}

void TextInputNode::setFont(
    facebook::react::TextAttributes const& textAttributes) {
  TextInputNodeBase::setCommonFontAttributes(textAttributes);

  std::string fontFamily = "HarmonyOS Sans";
  if (!textAttributes.fontFamily.empty()) {
    fontFamily = textAttributes.fontFamily;
  }

  auto fontWeight = ARKUI_FONT_WEIGHT_NORMAL;
  if (textAttributes.fontWeight.has_value()) {
    fontWeight = rnoh::convertFontWeight(textAttributes.fontWeight.value());
  }

  auto fontStyle = ARKUI_FONT_STYLE_NORMAL;
  if (textAttributes.fontStyle.has_value() &&
      textAttributes.fontStyle.value() == facebook::react::FontStyle::Italic) {
    fontStyle = ARKUI_FONT_STYLE_ITALIC;
  }

    float fontSize = defaultFontSize;
    if (!std::isnan(textAttributes.fontSize)) {
        fontSize = static_cast<float>(textAttributes.fontSize);
    }
    bool allowFontScaling = true;
    if (textAttributes.allowFontScaling.has_value()) {
        allowFontScaling = textAttributes.allowFontScaling.value();
    }
    if (!allowFontScaling) {
        maybeThrow(NativeNodeApi::getInstance()->setLengthMetricUnit(
            m_nodeHandle, ArkUI_LengthMetricUnit::ARKUI_LENGTH_METRIC_UNIT_VP));
    } else {
        maybeThrow(NativeNodeApi::getInstance()->setLengthMetricUnit(
            m_nodeHandle, ArkUI_LengthMetricUnit::ARKUI_LENGTH_METRIC_UNIT_FP));
    }

  std::array<ArkUI_NumberValue, 3> value = {
      {{.f32 = fontSize},
       {.i32 = static_cast<int32_t>(fontStyle)},
       {.i32 = static_cast<int32_t>(fontWeight)}}};
  ArkUI_AttributeItem item = {value.data(), value.size(), fontFamily.c_str()};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_PLACEHOLDER_FONT, &item);
}

void TextInputNode::setCaretColor(facebook::react::SharedColor const& color) {
  uint32_t colorValue = *color;
  m_caretColorValue = *color;
  ArkUI_NumberValue value = {.u32 = colorValue};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_CARET_COLOR, &item);
}

void TextInputNode::setMaxLength(int32_t maxLength) {
  ArkUI_NumberValue value = {.i32 = maxLength};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_MAX_LENGTH, &item);
}

void TextInputNode::setPlaceholder(std::string const& placeholder) {
  ArkUI_AttributeItem item = {.string = placeholder.c_str()};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_PLACEHOLDER, &item);
}

void TextInputNode::setPlaceholderColor(
    facebook::react::SharedColor const& color) {
  uint32_t colorValue = *color;
  ArkUI_NumberValue value = {.u32 = colorValue};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_PLACEHOLDER_COLOR, &item);
}

void TextInputNode::resetSelectedBackgroundColor() {
  m_nodeApi->resetAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_SELECTED_BACKGROUND_COLOR);
}

void TextInputNode::setPasswordRules(std::string rules) {
  ArkUI_AttributeItem item = {.string = rules.c_str()};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_PASSWORD_RULES, &item);
}

void TextInputNode::setUnderlineColorAndroid(
    facebook::react::SharedColor const& underlineColorAndroid) {
  if (*underlineColorAndroid >> 24 == 0) {
    return;
  }
  ArkUI_NumberValue showValue = {.i32 = 1};
  ArkUI_AttributeItem showItem = {&showValue, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_SHOW_UNDERLINE, &showItem);
  ArkUI_NumberValue value[] = {
      {.u32 = rnoh::convertColorToTranslucentUnderline(underlineColorAndroid)},
      {.u32 = rnoh::convertColorToTranslucentUnderline(underlineColorAndroid)},
      {.u32 = rnoh::convertColorToTranslucentUnderline(underlineColorAndroid)},
      {.u32 = rnoh::convertColorToTranslucentUnderline(underlineColorAndroid)}};

  ArkUI_AttributeItem item = {
      .value = value, .size = sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_UNDERLINE_COLOR, &item);
}

void TextInputNode::SetContextMenuHidden(bool const& hidden) {
  ArkUI_NumberValue value = {.i32 = hidden};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_SELECTION_MENU_HIDDEN, &item);
}

void TextInputNode::setTextContentType(std::string const& textContentType) {
  if (textContentType == "oneTimeCode") {
    ArkUI_NumberValue value = {.i32 = TEXT_INPUT_TYPE_ONE_TIME_CODE};
    ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
    m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_AREA_TYPE, &item);
    return;
  }
  ArkUI_NumberValue type = rnoh::convertContentType(textContentType);
  if (type.i32 == -1) {
    this->setAutoFill(false); // The purpose is to fix the issue where an
                              // autofill bubble still pops up when
                              // textContentType is dynamically changed to none.
  } else {
    this->setAutoFill(true);
  }
  std::array<ArkUI_NumberValue, 1> value = {type};
  ArkUI_AttributeItem item = {value.data(), value.size()};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_CONTENT_TYPE, &item);
}

void TextInputNode::setAutoFill(bool autoFill) {
  uint32_t isAutoFill = static_cast<uint32_t>(autoFill);
  ArkUI_NumberValue value = {.u32 = isAutoFill};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_ENABLE_AUTO_FILL, &item);
}

void TextInputNode::setBlurOnSubmit(bool blurOnSubmit) {
  ArkUI_NumberValue value = {.i32 = int32_t(blurOnSubmit)};
  ArkUI_AttributeItem item = {&value, 1};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_BLUR_ON_SUBMIT, &item);
}

void TextInputNode::setshowSoftInputOnFocus(int32_t enable) {
  ArkUI_NumberValue value = {.i32 = enable};
  ArkUI_AttributeItem item = {&value, 1};
  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_SHOW_KEYBOARD_ON_FOCUS, &item);
}

void TextInputNode::setInputFilter(const std::string& inputFilter) {
  ArkUI_AttributeItem item = {.string = inputFilter.c_str()};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_INPUT_FILTER, &item);
}

std::string TextInputNode::getTextContent() {
  auto item = NativeNodeApi::getInstance()->getAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_TEXT);
  return item->string;
}

void TextInputNode::setSelectAll(bool selectAll) {
  ArkUI_NumberValue value = {.i32 = int32_t(selectAll)};
  ArkUI_AttributeItem item = {&value, 1};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_TEXT_INPUT_SELECT_ALL, &item);
}

void TextInputNode::setAutoFocus(bool const &autoFocus){
    m_autofocus = autoFocus;
}

bool TextInputNode::getTextFocusStatus() {
  return isFocused();
}

} // namespace rnoh
