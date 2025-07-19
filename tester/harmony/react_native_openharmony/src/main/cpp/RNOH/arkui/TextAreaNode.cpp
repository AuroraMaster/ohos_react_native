/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "TextAreaNode.h"
#include <bits/alltypes.h>
#include "NativeNodeApi.h"
#include "RNOH/arkui/conversions.h"

static constexpr std::array TEXT_AREA_NODE_EVENT_TYPES = {
    NODE_TEXT_AREA_ON_PASTE,
    NODE_TEXT_AREA_ON_CHANGE,
    NODE_TEXT_INPUT_ON_CUT,
    NODE_ON_FOCUS,
    NODE_ON_BLUR,
    NODE_TEXT_AREA_ON_TEXT_SELECTION_CHANGE,
    NODE_TEXT_AREA_ON_CONTENT_SCROLL,
    NODE_TEXT_AREA_ON_CONTENT_SIZE_CHANGE,
    NODE_TEXT_AREA_ON_WILL_DELETE,
    NODE_EVENT_ON_APPEAR,
    NODE_EVENT_ON_DISAPPEAR};

namespace rnoh {

TextAreaNode::TextAreaNode(Context context)
    : TextInputNodeBase(context, ArkUI_NodeType::ARKUI_NODE_TEXT_AREA),
      m_textAreaNodeDelegate(nullptr) {
  for (auto eventType : TEXT_AREA_NODE_EVENT_TYPES) {
    registerNodeEvent(eventType);
    // NODE_TEXT_AREA_ENABLE_KEYBOARD_ON_FOCUS missing in C-API
  }
}

TextAreaNode::~TextAreaNode() {
  for (auto eventType : TEXT_AREA_NODE_EVENT_TYPES) {
    unregisterNodeEvent(eventType);
  }
}

void TextAreaNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  ArkUINode::onNodeEvent(eventType, eventArgs);
  if (eventType == ArkUI_NodeEventType::NODE_ON_FOCUS) {
    if (m_textAreaNodeDelegate != nullptr) {
      m_textAreaNodeDelegate->onFocus();
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_ON_BLUR) {
    if (m_textAreaNodeDelegate != nullptr) {
      m_textAreaNodeDelegate->onBlur();
    }
  } else if (
      eventType ==
      ArkUI_NodeEventType::NODE_TEXT_AREA_ON_TEXT_SELECTION_CHANGE) {
    if (m_textAreaNodeDelegate != nullptr) {
      int32_t selectionLocation = eventArgs[0].i32;
      int32_t selectionLength = eventArgs[1].i32 - eventArgs[0].i32;
      m_textAreaNodeDelegate->onTextSelectionChange(
          selectionLocation, selectionLength);
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_TEXT_AREA_ON_CONTENT_SCROLL) {
    if (m_textAreaNodeDelegate != nullptr) {
      m_textAreaNodeDelegate->onContentScroll();
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_TEXT_AREA_ON_CONTENT_SIZE_CHANGE) {
    if (m_textAreaNodeDelegate != nullptr) {
      float width = eventArgs[0].f32;
      float height = eventArgs[1].f32;
      m_textAreaNodeDelegate->onContentSizeChange(width, height, true);
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_EVENT_ON_APPEAR) {
    if (m_autofocus == true){
      ArkUI_NumberValue value = {.i32 = static_cast<int32_t>(1)};
      ArkUI_AttributeItem item = {&value, 1};
      maybeThrow(m_context.nodeApi.setAttribute(
        m_nodeHandle, NODE_FOCUS_STATUS, &item));
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_EVENT_ON_DISAPPEAR) {
    if (getTextFocusStatus() == true){
      ArkUI_NumberValue value = {.i32 = static_cast<int32_t>(0)};
      ArkUI_AttributeItem item = {&value, 1};
      maybeThrow(m_context.nodeApi.setAttribute(
        m_nodeHandle, NODE_FOCUS_STATUS, &item));
    }
  }
}

void TextAreaNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    std::string_view eventString) {
  if (eventType == ArkUI_NodeEventType::NODE_TEXT_AREA_ON_CHANGE) {
    if (m_textAreaNodeDelegate != nullptr) {
      std::string text(eventString);
      if (m_setTextContent == true && text==m_textContent){ //it does not trigger onChange when using setTextContent
        m_setTextContent = false;
      } else{
        m_setTextContent = false;
        m_textAreaNodeDelegate->onChange(std::move(text));
      }
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_TEXT_AREA_ON_PASTE) {
    if (m_textAreaNodeDelegate != nullptr) {
      m_textAreaNodeDelegate->onPasteOrCut();
    }
  } else if (eventType == ArkUI_NodeEventType::NODE_TEXT_INPUT_ON_CUT) {
    if (m_textAreaNodeDelegate != nullptr) {
      m_textAreaNodeDelegate->onPasteOrCut();
    }
  }
  // NODE_TEXT_AREA_ON_CUT missing
}

void TextAreaNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    ArkUI_NodeEvent* event) {
  if (eventType == ArkUI_NodeEventType::NODE_TEXT_AREA_ON_WILL_DELETE) {
    ArkUI_NumberValue arkUiValues[2];
    maybeThrow(OH_ArkUI_NodeEvent_GetNumberValue(event, 0, arkUiValues));
    maybeThrow(OH_ArkUI_NodeEvent_GetNumberValue(event, 1, arkUiValues));

    ArkUI_NumberValue returnValue[] = {
        ArkUI_NumberValue{1},
    };
    // OH_ArkUI_NodeEvent_SetReturnNumberValue copies the value, so passing this
    // as a pointer to a variable on the stack should be safe.  (check the
    // arkui_ace_engine repository).
    // We need to set this or else the TextArea won't delete the character.
    maybeThrow(OH_ArkUI_NodeEvent_SetReturnNumberValue(event, returnValue, 1));

    // Contrary to its documentation, and no matter the actual type of data
    // OH_ArkUI_NodeEvent_GetNumberValue copies the numbers at indexes lower or
    // equal than 1 to .f32. Because both float and i32 is 4 byte we can still
    // read arkUiValues[1] as i32. arkUIValues[1] is originally set (when the
    // event is created) as int32_t, so we get the correct value here.
    // In c++ using unions for type punning like this is undefined behaviour,
    // but OH_ArkUI_NodeEvent_GetNumberValue is already doing it, so we either
    // can use OH_ArkUI_NodeEvent_GetNumberValue according to the docs and just
    // read the i32, or read the f32 and convert it to i32 using memcpy, but
    // that would break the contract set up by the documentation of
    // OH_ArkUI_NodeEvent_GetNumberValue.
    m_textAreaNodeDelegate->onWillDelete(
        this, static_cast<int>(round(arkUiValues[0].f32)), arkUiValues[1].i32);
  }
}

void TextAreaNode::setTextAreaNodeDelegate(
    TextAreaNodeDelegate* textAreaNodeDelegate) {
  m_textAreaNodeDelegate = textAreaNodeDelegate;
}

facebook::react::Point TextAreaNode::getTextAreaOffset() const {
  auto value = NativeNodeApi::getInstance()
                   ->getAttribute(m_nodeHandle, NODE_OFFSET)
                   ->value;
  facebook::react::Float x = value[0].i32;
  facebook::react::Float y = value[1].i32;

  return facebook::react::Point{x, y};
}

facebook::react::Rect TextAreaNode::getTextContentRect() const {
  auto value = getAttribute(NODE_TEXT_AREA_CONTENT_RECT).value;
  facebook::react::Float x = value[0].f32;
  facebook::react::Float y = value[1].f32;
  facebook::react::Float width = value[2].f32;
  facebook::react::Float height = value[3].f32;
  return {x, y, width, height};
}

void TextAreaNode::setTextContent(std::string const& textContent) {
  ArkUI_AttributeItem item = {.string = textContent.c_str()};
  m_setTextContent = true;
  m_textContent = textContent;
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_TEXT, &item));
}

void TextAreaNode::setInputType(ArkUI_TextAreaType  keyboardType) {
  ArkUI_NumberValue value = {.i32 = keyboardType};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_TYPE, &item));
}

void TextAreaNode::setFont(
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
    const int32_t arraySize = 3;
    std::array<ArkUI_NumberValue, arraySize> value = {
        {{.f32 = fontSize},
        {.i32 = static_cast<int32_t>(fontStyle)},
        {.i32 = static_cast<int32_t>(fontWeight)}}};
    ArkUI_AttributeItem item = {value.data(), value.size(), fontFamily.c_str()};
    maybeThrow(m_context.nodeApi.setAttribute(
        m_nodeHandle, NODE_TEXT_AREA_PLACEHOLDER_FONT, &item));
}

void TextAreaNode::setCaretColor(facebook::react::SharedColor const& color) {
  uint32_t colorValue = *color;
  ArkUI_NumberValue value = {.u32 = colorValue};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_CARET_COLOR, &item));
}

void TextAreaNode::setMaxLength(int32_t maxLength) {
  ArkUI_NumberValue value = {.i32 = maxLength};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_MAX_LENGTH, &item));
}

void TextAreaNode::setPlaceholder(std::string const& placeholder) {
  ArkUI_AttributeItem item = {.string = placeholder.c_str()};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_PLACEHOLDER, &item));
}

void TextAreaNode::setPlaceholderColor(
    facebook::react::SharedColor const& color) {
  uint32_t colorValue = *color;
  ArkUI_NumberValue value = {.u32 = colorValue};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_PLACEHOLDER_COLOR, &item));
}

void TextAreaNode::defaultSetPadding() {
  ArkUI_NumberValue value = {.f32 = 0.f};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_PADDING, &item));
}

void TextAreaNode::setEnterKeyType(
    ArkUI_EnterKeyType returnKeyType) {
  ArkUI_NumberValue value = {.i32 = returnKeyType};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_ENTER_KEY_TYPE, &item));
}
std::string TextAreaNode::getTextContent() {
  auto item = NativeNodeApi::getInstance()->getAttribute(
      m_nodeHandle, NODE_TEXT_AREA_TEXT);
  return item->string;
}

void TextAreaNode::SetContextMenuHidden(bool const& hidden) {
  ArkUI_NumberValue value = {.i32 = hidden};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
    maybeThrow(m_context.nodeApi.setAttribute(
        m_nodeHandle, NODE_TEXT_AREA_SELECTION_MENU_HIDDEN, &item));
}

void TextAreaNode::setTextContentType(std::string const& textContentType){
   ArkUI_NumberValue type = rnoh::convertContentType(textContentType); 
  if (type.i32 == -1) { 
    this->setAutoFill(false); // The purpose is to fix the issue where an autofill bubble still pops up when textContentType is dynamically changed to none.
  } else {
    this->setAutoFill(true);
  }
  std::array<ArkUI_NumberValue, 1> value = {type}; 
  ArkUI_AttributeItem item = {value.data(), value.size()};
  maybeThrow(m_context.nodeApi.setAttribute(
    m_nodeHandle, NODE_TEXT_INPUT_CONTENT_TYPE, &item));
}

void TextAreaNode::setUnderlineColorAndroid(
    facebook::react::SharedColor const& underlineColorAndroid) {
  if (*underlineColorAndroid >> 24 == 0) {
      return;
  }
  ArkUI_NumberValue showValue = {.i32 = 1};
  ArkUI_AttributeItem showItem = {&showValue, sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_SHOW_UNDERLINE, &showItem));
  ArkUI_NumberValue value[] = {
      {.u32 = rnoh::convertColorToTranslucentUnderline(underlineColorAndroid)},
      {.u32 = rnoh::convertColorToTranslucentUnderline(underlineColorAndroid)},
      {.u32 = rnoh::convertColorToTranslucentUnderline(underlineColorAndroid)},
      {.u32 = rnoh::convertColorToTranslucentUnderline(underlineColorAndroid)}};

  ArkUI_AttributeItem item = {
      .value = value, .size = sizeof(value) / sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_UNDERLINE_COLOR, &item));
}

void TextAreaNode::setAutoFill(bool autoFill) {
  uint32_t isAutoFill = static_cast<uint32_t>(autoFill);
  ArkUI_NumberValue value = {.u32 = isAutoFill};
  ArkUI_AttributeItem item = {&value, sizeof(ArkUI_NumberValue)};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_ENABLE_AUTO_FILL, &item));
}

void TextAreaNode::setBlurOnSubmit(bool blurOnSubmit) {
  ArkUI_NumberValue value = {.i32 = int32_t(blurOnSubmit)};
  ArkUI_AttributeItem item = {&value, 1};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_BLUR_ON_SUBMIT, &item));
}
void TextAreaNode::setshowSoftInputOnFocus(int32_t enable){
  ArkUI_NumberValue value = {.i32 = enable};
  ArkUI_AttributeItem item = {&value, 1};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_SHOW_KEYBOARD_ON_FOCUS, &item));
}

void TextAreaNode::setInputFilter(const std::string &inputFilter) {
  ArkUI_AttributeItem item = {.string = inputFilter.c_str()};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_AREA_INPUT_FILTER, &item));
}

void TextAreaNode::setSelectAll(bool selectAll) {
  ArkUI_NumberValue value = {.i32 = int32_t(selectAll)};
  ArkUI_AttributeItem item = {&value, 1};
  maybeThrow(m_context.nodeApi.setAttribute(
      m_nodeHandle, NODE_TEXT_INPUT_SELECT_ALL, &item));
}

void TextAreaNode::setAutoFocus(bool const &autoFocus){
    m_autofocus = autoFocus;
}
bool TextAreaNode::getTextFocusStatus(){
    return NativeNodeApi::getInstance()->getAttribute(
      m_nodeHandle, NODE_FOCUS_STATUS)->value[0].i32;
}
} // namespace rnoh
