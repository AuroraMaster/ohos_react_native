/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/text/ParagraphProps.h>
#include <react/renderer/components/text/ParagraphState.h>
#include <react/renderer/components/textinput/TextInputProps.h>
#include <react/renderer/core/ConcreteState.h>
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"

namespace rnoh {

class TextInputComponentNapiBinder : public ViewComponentNapiBinder {
 public:
  napi_value createProps(
      napi_env env,
      facebook::react::ShadowView const shadowView) override {
    napi_value napiViewProps =
        ViewComponentNapiBinder::createProps(env, shadowView);
    if (auto props =
            std::dynamic_pointer_cast<const facebook::react::TextInputProps>(
                shadowView.props)) {
      auto textAlign = props->textAttributes.alignment;
      std::string alignment = "";
      if (textAlign.has_value()) {
        alignment = textAlignmentToString(textAlign.value());
      }
      auto propsObjBuilder =
          ArkJS(env)
              .getObjectBuilder(napiViewProps)
              .addProperty("text", props->text)
              .addProperty("fontColor", props->textAttributes.foregroundColor)
              .addProperty("fontFamily", props->textAttributes.fontFamily)
              .addProperty("lineHeight", props->textAttributes.lineHeight)
              .addProperty("fontSize", props->textAttributes.fontSize)
              .addProperty("multiline", props->traits.multiline)
              .addProperty("placeholder", props->placeholder)
              .addProperty("placeholderTextColor", props->placeholderTextColor)
              .addProperty("editable", props->traits.editable)
              .addProperty("caretHidden", props->traits.caretHidden)
              .addProperty("secureTextEntry", props->traits.secureTextEntry)
              .addProperty("selectionColor", props->selectionColor)
              .addProperty("blurOnSubmit", props->blurOnSubmit)
              .addProperty(
                  "returnKeyType",
                  returnKeyTypeToString(props->traits.returnKeyType))
              .addProperty(
                  "returnKeyLabel",
                  props->traits.returnKeyLabel)
              .addProperty("textAlign", alignment)
              .addProperty("autoFocus", props->autoFocus)
              .addProperty("importantForAutofill", props->importantForAutofill)
              .addProperty(
                  "keyboardType",
                  keyboardTypeToString(props->traits.keyboardType))
              .addProperty(
                  "fontStyle",
                  props->textAttributes.fontStyle ==
                          facebook::react::FontStyle::Italic
                      ? "italic"
                      : "normal");

      if (props->textAttributes.fontWeight.has_value()) {
        propsObjBuilder.addProperty(
            "fontWeight",
            static_cast<int>(props->textAttributes.fontWeight.value()));
      }
      return propsObjBuilder.build();
    }
    return napiViewProps;
  };

 private:
  std::string returnKeyTypeToString(
      facebook::react::ReturnKeyType returnKeyType) {
    switch (returnKeyType) {
      case facebook::react::ReturnKeyType::Done:
        return "done";
      case facebook::react::ReturnKeyType::Go:
        return "go";
      case facebook::react::ReturnKeyType::Next:
        return "next";
      case facebook::react::ReturnKeyType::Search:
        return "search";
      case facebook::react::ReturnKeyType::Send:
        return "send";
      default:
        return "default";
    }
  };

  std::string textAlignmentToString(facebook::react::TextAlignment alignment) {
    switch (alignment) {
      case facebook::react::TextAlignment::Natural:
        return "natural";
      case facebook::react::TextAlignment::Left:
        return "left";
      case facebook::react::TextAlignment::Right:
        return "right";
      case facebook::react::TextAlignment::Center:
        return "center";
      case facebook::react::TextAlignment::Justified:
        return "justified";
    }
  };

  std::string keyboardTypeToString(facebook::react::KeyboardType keyboardType) {
    switch (keyboardType) {
      case facebook::react::KeyboardType::EmailAddress:
        return "emailAddress";
      case facebook::react::KeyboardType::Numeric:
        return "numeric";
      case facebook::react::KeyboardType::PhonePad:
        return "phonePad";
      case facebook::react::KeyboardType::NumberPad:
        return "numberPad";
      case facebook::react::KeyboardType::DecimalPad:
        return "decimalPad";
      default:
        return "default";
    }
  }
};

} // namespace rnoh
