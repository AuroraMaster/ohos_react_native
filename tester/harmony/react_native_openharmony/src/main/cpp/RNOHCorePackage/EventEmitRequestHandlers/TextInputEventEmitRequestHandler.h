/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include <react/renderer/components/textinput/TextInputEventEmitter.h>
#include "RNOH/ArkJS.h"
#include "RNOH/EventEmitRequestHandler.h"

namespace rnoh {

facebook::react::TextInputMetrics convertTextInputEvent(
    ArkJS& arkJs,
    napi_value eventObject) {
  auto text = arkJs.getString(arkJs.getObjectProperty(eventObject, "text"));
  auto eventCount =
      arkJs.getInteger(arkJs.getObjectProperty(eventObject, "eventCount"));
  auto selectionRangeNapi =
      arkJs.getObject(arkJs.getObjectProperty(eventObject, "selectionRange"));
  auto selectionRangeLocation =
      arkJs.getInteger(selectionRangeNapi.getProperty("location"));
  auto selectionRangeLength =
      arkJs.getInteger(selectionRangeNapi.getProperty("length"));
  facebook::react::TextInputMetrics textInputMetrics{
      .text = text,
      .selectionRange =
          {.location = selectionRangeLocation, .length = selectionRangeLength},
      .eventCount = eventCount};
  return textInputMetrics;
}

facebook::react::KeyPressMetrics convertKeyPressEvent(
    ArkJS& arkJs,
    napi_value eventObject) {
  auto key = arkJs.getString(arkJs.getObjectProperty(eventObject, "key"));
  auto eventCount =
      arkJs.getInteger(arkJs.getObjectProperty(eventObject, "eventCount"));
  facebook::react::KeyPressMetrics keyPressMetrics{
      .text = key, .eventCount = eventCount};
  return keyPressMetrics;
}

facebook::react::OnChangeMetrics convertOnChangeEvent(
    ArkJS& arkJs,
    napi_value eventObject) {
  auto text = arkJs.getString(arkJs.getObjectProperty(eventObject, "text"));
  auto eventCount =
      arkJs.getInteger(arkJs.getObjectProperty(eventObject, "eventCount"));
  facebook::react::OnChangeMetrics onChangeMetrics{
      .text = text, .eventCount = eventCount};
  return onChangeMetrics;
}

enum TextInputEventType {
  TEXT_INPUT_UNSUPPORTED,
  TEXT_INPUT_ON_CHANGE,
  TEXT_INPUT_ON_SUBMIT_EDITING,
  TEXT_INPUT_ON_END_EDITING,
  TEXT_INPUT_ON_FOCUS,
  TEXT_INPUT_ON_BLUR,
  TEXT_INPUT_ON_KEY_PRESS,
  TEXT_INPUT_ON_SELECTION_CHANGE,
};

TextInputEventType getTextInputEventType(std::string const& eventName) {
  if (eventName == "TextInputChange") {
    return TextInputEventType::TEXT_INPUT_ON_CHANGE;
  } else if (eventName == "onSubmitEditing") {
    return TextInputEventType::TEXT_INPUT_ON_SUBMIT_EDITING;
  } else if (eventName == "onEndEditing") {
    return TextInputEventType::TEXT_INPUT_ON_END_EDITING;
  } else if (eventName == "onFocus") {
    return TextInputEventType::TEXT_INPUT_ON_FOCUS;
  } else if (eventName == "onBlur") {
    return TextInputEventType::TEXT_INPUT_ON_BLUR;
  } else if (eventName == "onKeyPress") {
    return TextInputEventType::TEXT_INPUT_ON_KEY_PRESS;
  } else if (eventName == "onSelectionChange") {
    return TextInputEventType::TEXT_INPUT_ON_SELECTION_CHANGE;
  } else {
    return TextInputEventType::TEXT_INPUT_UNSUPPORTED;
  }
}

class TextInputEventEmitRequestHandler : public EventEmitRequestHandler {
  void handleEvent(EventEmitRequestHandler::Context const& ctx) override {
    auto eventType = getTextInputEventType(ctx.eventName);
    if (eventType == TextInputEventType::TEXT_INPUT_UNSUPPORTED) {
      return;
    }

    auto eventEmitter =
        ctx.shadowViewRegistry
            ->getEventEmitter<facebook::react::TextInputEventEmitter>(ctx.tag);
    if (eventEmitter == nullptr) {
      return;
    }

    ArkJS arkJs(ctx.env);
    switch (eventType) {
      case TextInputEventType::TEXT_INPUT_ON_CHANGE:
        eventEmitter->onChange(convertOnChangeEvent(arkJs, ctx.payload));
        break;
      case TextInputEventType::TEXT_INPUT_ON_SUBMIT_EDITING:
        eventEmitter->onSubmitEditing(
            convertTextInputEvent(arkJs, ctx.payload));
        break;
      case TextInputEventType::TEXT_INPUT_ON_END_EDITING:
        eventEmitter->onEndEditing(convertTextInputEvent(arkJs, ctx.payload));
        break;
      case TextInputEventType::TEXT_INPUT_ON_FOCUS:
        eventEmitter->onFocus(convertTextInputEvent(arkJs, ctx.payload));
        break;
      case TextInputEventType::TEXT_INPUT_ON_BLUR:
        eventEmitter->onBlur(convertTextInputEvent(arkJs, ctx.payload));
        break;
      case TextInputEventType::TEXT_INPUT_ON_KEY_PRESS:
        eventEmitter->onKeyPress(convertKeyPressEvent(arkJs, ctx.payload));
        break;
      case TextInputEventType::TEXT_INPUT_ON_SELECTION_CHANGE:
        eventEmitter->onSelectionChange(
            convertTextInputEvent(arkJs, ctx.payload));
        break;
      default:
        break;
    }
  }
};

} // namespace rnoh