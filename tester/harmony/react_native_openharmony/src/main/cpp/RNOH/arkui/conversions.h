/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <arkui/native_type.h>
#include <bits/alltypes.h>
#include <glog/logging.h>
#include "react/renderer/attributedstring/primitives.h"
#include "react/renderer/components/textinput/primitives.h"
#include "react/renderer/components/view/primitives.h"

typedef enum {
  USER_NAME,
  PASSWORD,
  NEW_PASSWORD,
  FULL_STREET_ADDRESS,
  HOUSE_NUMBER,
  DISTRICT_ADDRESS,
  CITY_ADDRESS,
  PROVINCE_ADDRESS,
  COUNTRY_ADDRESS,
  PERSON_FULL_NAME,
  PERSON_LAST_NAME,
  PERSON_FIRST_NAME,
  PHONE_NUMBER,
  PHONE_COUNTRY_CODE,
  FULL_PHONE_NUMBER,
  EMAIL_ADDRESS,
  BANK_CARD_NUMBER,
  ID_CARD_NUMBER,
  NICKNAME,
  DETAIL_INFO_WITHOUT_STREET,
  FORMAT_ADDRESS,
  PASSPORT_NUMBER,
  VALIDITY,
  ISSUE_AT,
  ORGANIZATION,
  TAX_ID,
  ADDRESS_CITY_AND_STATE,
  FLIGHT_NUMBER,
  LICENSE_NUMBER,
  LICENSE_FILE_NUMBER,
  LICENSE_PLATE,
  ENGINE_NUMBER,
  LICENSE_CHASSIS_NUMBER,
  
} TextInput_TextContentType;

namespace rnoh {
inline ArkUI_BorderStyle convertReactBorderStyleToArk(
    facebook::react::BorderStyle rnBorder) {
  switch (rnBorder) {
    case facebook::react::BorderStyle::Solid:
      return ArkUI_BorderStyle::ARKUI_BORDER_STYLE_SOLID;
    case facebook::react::BorderStyle::Dotted:
      return ArkUI_BorderStyle::ARKUI_BORDER_STYLE_DOTTED;
    case facebook::react::BorderStyle::Dashed:
      return ArkUI_BorderStyle::ARKUI_BORDER_STYLE_DASHED;
    default:
      DLOG(WARNING) << "Invalid BorderStyle";
      return ArkUI_BorderStyle::ARKUI_BORDER_STYLE_SOLID;
  }
}

inline ArkUI_TextAreaType convertTextAreaInputType(
    facebook::react::KeyboardType keyboardType) {
  switch (keyboardType) {
    case facebook::react::KeyboardType::Numeric:
    case facebook::react::KeyboardType::NumberPad:
      return ARKUI_TEXTAREA_TYPE_NUMBER;
    case facebook::react::KeyboardType::PhonePad:
      return ARKUI_TEXTAREA_TYPE_PHONE_NUMBER;
    case facebook::react::KeyboardType::EmailAddress:
      return ARKUI_TEXTAREA_TYPE_EMAIL;
    default:
      return ARKUI_TEXTAREA_TYPE_NORMAL;
  }
}

inline uint32_t convertColorToTranslucentUnderline(
    facebook::react::SharedColor const& color) {
  auto colorComponents = colorComponentsFromColor(color);
  uint32_t red = static_cast<uint32_t>(colorComponents.red * 255) & 0xFF;
  uint32_t green = static_cast<uint32_t>(colorComponents.green * 255) & 0xFF;
  uint32_t blue = static_cast<uint32_t>(colorComponents.blue * 255) & 0xFF;
  uint32_t alpha = static_cast<uint32_t>(colorComponents.alpha * 255) & 0xFF;
  auto colorValue = (alpha << 24) | (red << 16) | (green << 8) | blue;
  return colorValue;
}

inline uint32_t convertColorToTranslucentSelection(
    facebook::react::SharedColor const& color) {
  auto colorComponents = colorComponentsFromColor(color);
  uint32_t red = static_cast<uint32_t>(colorComponents.red * 255) & 0xFF;
  uint32_t green = static_cast<uint32_t>(colorComponents.green * 255) & 0xFF;
  uint32_t blue = static_cast<uint32_t>(colorComponents.blue * 255) & 0xFF;
  uint32_t alpha = 0x55;
  auto colorValue = (alpha << 24) | (red << 16) | (green << 8) | blue;
  return colorValue;
}

inline ArkUI_TextInputType convertInputType(
    facebook::react::KeyboardType keyboardType) {
  switch (keyboardType) {
    case facebook::react::KeyboardType::Numeric:
    case facebook::react::KeyboardType::NumberPad:
      return ARKUI_TEXTINPUT_TYPE_NUMBER;
    case facebook::react::KeyboardType::DecimalPad:
      return ARKUI_TEXTINPUT_TYPE_NUMBER_DECIMAL;
    case facebook::react::KeyboardType::PhonePad:
      return ARKUI_TEXTINPUT_TYPE_PHONE_NUMBER;
    case facebook::react::KeyboardType::EmailAddress:
      return ARKUI_TEXTINPUT_TYPE_EMAIL;
    default:
      return ARKUI_TEXTINPUT_TYPE_NORMAL;
  }
}

inline ArkUI_EnterKeyType convertEnterKeyType(
    facebook::react::ReturnKeyType returnKeyType) {
  switch (returnKeyType) {
    case facebook::react::ReturnKeyType::Done:
      return ARKUI_ENTER_KEY_TYPE_DONE;
    case facebook::react::ReturnKeyType::Go:
      return ARKUI_ENTER_KEY_TYPE_GO;
    case facebook::react::ReturnKeyType::Next:
      return ARKUI_ENTER_KEY_TYPE_NEXT;
    case facebook::react::ReturnKeyType::Search:
      return ARKUI_ENTER_KEY_TYPE_SEARCH;
    case facebook::react::ReturnKeyType::Send:
      return ARKUI_ENTER_KEY_TYPE_SEND;
    default:
      return ARKUI_ENTER_KEY_TYPE_DONE;
  }
}

inline ArkUI_EnterKeyType convertTextAreaEnterKeyType(
    facebook::react::ReturnKeyType returnKeyType) {
  if (returnKeyType == facebook::react::ReturnKeyType::Default) {
    return ARKUI_ENTER_KEY_TYPE_NEW_LINE;
  }
  return convertEnterKeyType(returnKeyType);
}

inline ArkUI_EnterKeyType convertEnterKeyLabel(
    std::string returnKeyLabel) {
  if (!returnKeyLabel.compare("done")) {
    return ARKUI_ENTER_KEY_TYPE_DONE;
  } else if (!returnKeyLabel.compare("go")) {
    return ARKUI_ENTER_KEY_TYPE_GO;
  } else if (!returnKeyLabel.compare("next")) {
    return ARKUI_ENTER_KEY_TYPE_NEXT;
  } else if (!returnKeyLabel.compare("search")) {
    return ARKUI_ENTER_KEY_TYPE_SEARCH;
  } else if (!returnKeyLabel.compare("send")) {
    return ARKUI_ENTER_KEY_TYPE_SEND;
  } else {
    return ARKUI_ENTER_KEY_TYPE_DONE;
  }
}

inline ArkUI_EnterKeyType convertTextAreaEnterKeyLabel(
    std::string returnKeyLabel) {
  if (returnKeyLabel.empty()) {
    return ARKUI_ENTER_KEY_TYPE_NEW_LINE;
  }
  return convertEnterKeyLabel(returnKeyLabel);
}

inline ArkUI_TextAlignment convertTextAlign(
    facebook::react::TextAlignment alignment) {
  switch (alignment) {
    case facebook::react::TextAlignment::Natural:
    case facebook::react::TextAlignment::Left:
      return ARKUI_TEXT_ALIGNMENT_START;
    case facebook::react::TextAlignment::Right:
      return ARKUI_TEXT_ALIGNMENT_END;
    case facebook::react::TextAlignment::Center:
      return ARKUI_TEXT_ALIGNMENT_CENTER;
    case facebook::react::TextAlignment::Justified:
      return ARKUI_TEXT_ALIGNMENT_JUSTIFY;
    default:
      return ARKUI_TEXT_ALIGNMENT_START;
  }
}

inline ArkUI_FontWeight convertFontWeight(
    facebook::react::FontWeight fontWeight) {
  auto fontWeightInt = static_cast<int32_t>(fontWeight);
  if (fontWeightInt < 200) {
    return ARKUI_FONT_WEIGHT_W100;
  }
  if (fontWeightInt < 300) {
    return ARKUI_FONT_WEIGHT_W200;
  }
  if (fontWeightInt < 400) {
    return ARKUI_FONT_WEIGHT_W300;
  }
  if (fontWeightInt < 500) {
    return ARKUI_FONT_WEIGHT_W400;
  }
  if (fontWeightInt < 600) {
    return ARKUI_FONT_WEIGHT_W500;
  }
  if (fontWeightInt < 700) {
    return ARKUI_FONT_WEIGHT_W600;
  }
  if (fontWeightInt < 800) {
    return ARKUI_FONT_WEIGHT_W700;
  }
  if (fontWeightInt < 900) {
    return ARKUI_FONT_WEIGHT_W800;
  }
  return ARKUI_FONT_WEIGHT_W900;
}
inline ArkUI_NumberValue convertContentType(std::string const& textContentType){
if (textContentType == "addressCity") {
      return {.i32 = TextInput_TextContentType::CITY_ADDRESS};
  } else if (textContentType == "addressState") {
      return {.i32 = TextInput_TextContentType::PROVINCE_ADDRESS};
  } else if (textContentType == "countryName") {
      return {.i32 = TextInput_TextContentType::COUNTRY_ADDRESS};
  } else if (textContentType == "creditCardNumber") {
      return {.i32 = TextInput_TextContentType::BANK_CARD_NUMBER};
  } else if (textContentType == "fullStreetAddress") {
      return {.i32 = TextInput_TextContentType::FULL_STREET_ADDRESS};
  } else if (textContentType == "sublocality") {
      return {.i32 = TextInput_TextContentType::DISTRICT_ADDRESS};
  } else if (textContentType == "telephoneNumber") {
      return {.i32 = TextInput_TextContentType::PHONE_NUMBER};
  }else if (textContentType == "username") {
      return {.i32 = TextInput_TextContentType::USER_NAME};
  } else if (textContentType == "password") {
      return {.i32 = TextInput_TextContentType::PASSWORD};
  } else if (textContentType == "newPassword") {
      return {.i32 = TextInput_TextContentType::NEW_PASSWORD};
  } else if (textContentType == "houseNumber") {
      return {.i32 = TextInput_TextContentType::HOUSE_NUMBER};
  } else if (textContentType == "districtAddress") {
      return {.i32 = TextInput_TextContentType::DISTRICT_ADDRESS};
  } else if (textContentType == "cityAddress") {
      return {.i32 = TextInput_TextContentType::CITY_ADDRESS};
  } else if (textContentType == "provinceAddress") {
      return {.i32 = TextInput_TextContentType::PROVINCE_ADDRESS};
  } else if (textContentType == "countryAddress") {
      return {.i32 = TextInput_TextContentType::COUNTRY_ADDRESS};
  } else if (textContentType == "personFullName") {
      return {.i32 = TextInput_TextContentType::PERSON_FULL_NAME};
  } else if (textContentType == "personLastName") {
      return {.i32 = TextInput_TextContentType::PERSON_LAST_NAME};
  } else if (textContentType == "personFirstName") {
      return {.i32 = TextInput_TextContentType::PERSON_FIRST_NAME};
  } else if (textContentType == "phoneNumber") {
      return {.i32 = TextInput_TextContentType::PHONE_NUMBER};
  } else if (textContentType == "phoneCountryCode") {
      return {.i32 = TextInput_TextContentType::PHONE_COUNTRY_CODE};
  } else if (textContentType == "fullPhoneNumber") {
      return {.i32 = TextInput_TextContentType::FULL_PHONE_NUMBER};
  } else if (textContentType == "emailAddress") {
      return {.i32 = TextInput_TextContentType::EMAIL_ADDRESS};
  } else if (textContentType == "bankCardNumber") {
      return {.i32 = TextInput_TextContentType::BANK_CARD_NUMBER};
  } else if (textContentType == "idCardNumber") {
      return {.i32 = TextInput_TextContentType::ID_CARD_NUMBER};
  } else if (textContentType == "nickName") {
      return {.i32 = TextInput_TextContentType::NICKNAME};
  } else if (textContentType == "name") {
      return {.i32 = TextInput_TextContentType::PERSON_FULL_NAME};
  } else if (textContentType == "familyName") {
      return {.i32 = TextInput_TextContentType::PERSON_LAST_NAME};
  } else if (textContentType == "givenName") {
      return {.i32 = TextInput_TextContentType::PERSON_FIRST_NAME};
  } else if (textContentType == "detailInfoWithoutStreet") {
      return {.i32 = TextInput_TextContentType::DETAIL_INFO_WITHOUT_STREET};
  } else if (textContentType == "formatAddress") {
      return {.i32 = TextInput_TextContentType::FORMAT_ADDRESS};
  } else if (textContentType == "passportNumber") {
      return {.i32 = TextInput_TextContentType::PASSPORT_NUMBER};
  } else if (textContentType == "validity") {
      return {.i32 = TextInput_TextContentType::VALIDITY};
  } else if (textContentType == "issueAt") {
      return {.i32 = TextInput_TextContentType::ISSUE_AT};
  } else if (textContentType == "organization") {
      return {.i32 = TextInput_TextContentType::ORGANIZATION};
  } else if (textContentType == "taxId") {
      return {.i32 = TextInput_TextContentType::TAX_ID};
  } else if (textContentType == "addressCityAndState") {
      return {.i32 = TextInput_TextContentType::ADDRESS_CITY_AND_STATE};
  } else if (textContentType == "flightNumber") {
      return {.i32 = TextInput_TextContentType::FLIGHT_NUMBER};
  } else if (textContentType == "licenseNumber") {
      return {.i32 = TextInput_TextContentType::LICENSE_NUMBER};
  } else if (textContentType == "licenseFileNumber") {
      return {.i32 = TextInput_TextContentType::LICENSE_FILE_NUMBER};
  } else if (textContentType == "engineNumber") {
      return {.i32 = TextInput_TextContentType::ENGINE_NUMBER};
  } else if (textContentType == "licenseChassisNumber") {
      return {.i32 = TextInput_TextContentType::LICENSE_CHASSIS_NUMBER};
  } else if (textContentType == "licensePlate") {
      return {.i32 = TextInput_TextContentType::LICENSE_PLATE};
  } else {
      return {.i32 = -1};
  }
}

inline bool convertImportantForAutofill(
  std::string const& autoFill) {
  auto string = (std::string)autoFill;
  if (string == "no" || string == "noExcludeDescendants"
    || string == "auto") {
    return false;
  } else if (string == "yes" || string == "yesExcludeDescendants") {
    return true;
  } else {
    DLOG(WARNING) << "Invalid ImportantForAutofill";
    return true;
  }
}
} // namespace rnoh
