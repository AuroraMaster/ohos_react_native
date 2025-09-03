/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <arkui/native_type.h>
#include <arkui/styled_string.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_text_typography.h>
#include <react/renderer/graphics/Size.h>
#include <react/renderer/textlayoutmanager/TextLayoutManager.h>
#include <memory>
#include "react/renderer/attributedstring/primitives.h"

namespace rnoh {

using UniqueTypographyStyle = std::unique_ptr<
    OH_Drawing_TypographyStyle,
    decltype(&OH_Drawing_DestroyTypographyStyle)>;
using SharedFontCollection = std::shared_ptr<
    OH_Drawing_FontCollection>;

class ArkUITypography final {
 public:
  facebook::react::TextMeasurement::Attachments getAttachments() const {
    facebook::react::TextMeasurement::Attachments result;
    result.reserve(m_attachmentCount);
    std::shared_ptr<OH_Drawing_TextBox> placeholderRects(
        OH_Drawing_TypographyGetRectsForPlaceholders(m_typography.get()), OH_Drawing_TypographyDestroyTextBox);
    // calculate attachment sizes and positions
    for (auto i = 0; i < m_attachmentCount; i++) {
      facebook::react::TextMeasurement::Attachment attachment;
      attachment.frame.origin.x =
          OH_Drawing_GetLeftFromTextBox(placeholderRects.get(), i);
      attachment.frame.origin.y =
          OH_Drawing_GetTopFromTextBox(placeholderRects.get(), i);
      attachment.frame.size.width =
          OH_Drawing_GetRightFromTextBox(placeholderRects.get(), i) -
          OH_Drawing_GetLeftFromTextBox(placeholderRects.get(), i);
      attachment.frame.size.height =
          OH_Drawing_GetBottomFromTextBox(placeholderRects.get(), i) -
          OH_Drawing_GetTopFromTextBox(placeholderRects.get(), i);
      attachment.frame.size.height /= m_scale;
      attachment.frame.size.width /= m_scale;
      attachment.frame.origin.x /= m_scale;
      attachment.frame.origin.y /= m_scale;
      result.push_back(std::move(attachment));
    }
    return result;
  }

  facebook::react::Float getHeight() const {
    return OH_Drawing_TypographyGetHeight(m_typography.get()) / m_scale;
  }

  facebook::react::Float getLongestLineWidth() const {
    return OH_Drawing_TypographyGetLongestLine(m_typography.get()) / m_scale;
  }

  bool getExceedMaxLines() const {
    return OH_Drawing_TypographyDidExceedMaxLines(m_typography.get());
  }

  using Rects = std::vector<facebook::react::Rect>;

  std::vector<Rects> getRectsForFragments(facebook::react::Point origin) const {
    std::vector<Rects> result;
    result.reserve(m_fragmentLengths.size());
    size_t fragmentBegin = 0;
    for (size_t i = 0; i < m_fragmentLengths.size(); i++) {
      auto fragmentEnd = fragmentBegin + m_fragmentLengths[i];
      auto textBoxes = OH_Drawing_TypographyGetRectsForRange(
          m_typography.get(),
          fragmentBegin,
          fragmentEnd,
          RECT_HEIGHT_STYLE_MAX,
          RECT_WIDTH_STYLE_MAX);
      auto textBoxCount = OH_Drawing_GetSizeOfTextBox(textBoxes);
      Rects rects;
      rects.reserve(textBoxCount);
      for (size_t j = 0; j < textBoxCount; j++) {
        facebook::react::Rect rect;
        rect.origin.x = (OH_Drawing_GetLeftFromTextBox(textBoxes, j)) / m_scale +
            m_offset.x / m_scale + origin.x;
        rect.origin.y = (OH_Drawing_GetTopFromTextBox(textBoxes, j)) / m_scale +
            m_offset.y / m_scale + origin.y;
        rect.size.width = (OH_Drawing_GetRightFromTextBox(textBoxes, j) -
            OH_Drawing_GetLeftFromTextBox(textBoxes, j)) / m_scale;
        rect.size.height = (OH_Drawing_GetBottomFromTextBox(textBoxes, j) -
            OH_Drawing_GetTopFromTextBox(textBoxes, j)) / m_scale;
        rects.emplace_back(std::move(rect));
      }
      result.emplace_back(std::move(rects));
      fragmentBegin = fragmentEnd;
    }
    return result;
  }
  
  void getLineMetrics(std::vector<OH_Drawing_LineMetrics>& data) const {
    auto count = OH_Drawing_TypographyGetLineCount(m_typography.get());
    for (int i = 0; i < count; i++) {
      OH_Drawing_LineMetrics metrics;
      OH_Drawing_TypographyGetLineMetricsAt(m_typography.get(), i, &metrics); // FIXME: lineMetrics out_of_range issue
      data.push_back(metrics);
    }
  }

 private:
  ArkUITypography(
      ArkUI_StyledString* typographyHandler,
      size_t attachmentCount,
      std::vector<size_t> fragmentLengths,
      facebook::react::Float maxWidth,
      float scale,
      std::optional<facebook::react::TextAlignment> textAlign)
      : m_typography(
            OH_ArkUI_StyledString_CreateTypography(typographyHandler),
            OH_Drawing_DestroyTypography),
        m_attachmentCount(attachmentCount),
        m_fragmentLengths(std::move(fragmentLengths)),
        m_scale(scale) {
            OH_Drawing_TypographyLayout(m_typography.get(), maxWidth);
            std::shared_ptr<OH_Drawing_LineMetrics> lineMetrics(
                OH_Drawing_TypographyGetLineMetrics(m_typography.get()),
                OH_Drawing_DestroyLineMetrics);
            if (lineMetrics) {
              m_offset.x = lineMetrics->x;
              m_offset.y = lineMetrics->y;
            }
            if (textAlign.has_value()) {
              auto longestWidth =
                  OH_Drawing_TypographyGetLongestLine(m_typography.get());
              switch (textAlign.value()) {
                case facebook::react::TextAlignment::Right:
                  m_offset.x = maxWidth - longestWidth;
                  break;
                case facebook::react::TextAlignment::Center:
                  m_offset.x = (maxWidth - longestWidth) / 2;
                  break;
                default:
                  break;
              }
            }
            // TextComponentInstance implements left margin by layoutConstraints
            // typography doesn't need to, also shouldn't, have left margin
            // do re-layout here using the longest line width as max width to
            // eliminate the left margin
            //
            // The return value of the OH_Drawing_TypographyGetLongestLine ()
            // interface needs to be rounded up; otherwise, abnormal text line
            // breaks may occur on some devices, such as mate 70 pro.
            if (!textAlign.has_value() || textAlign.value() != facebook::react::TextAlignment::Justified) {
              OH_Drawing_TypographyLayout(
                  m_typography.get(),
                  std::ceil(OH_Drawing_TypographyGetLongestLine(m_typography.get())));
            }
        }

  std::shared_ptr<OH_Drawing_Typography>
          m_typography;
  size_t m_attachmentCount;
  std::vector<size_t> m_fragmentLengths;

  float m_scale = 1.0;
  facebook::react::Point m_offset;
  friend class ArkUITypographyBuilder;
};

class ArkUITypographyBuilder final {
 public:
  ArkUITypographyBuilder(
      OH_Drawing_TypographyStyle* typographyStyle,
      std::shared_ptr<OH_Drawing_FontCollection> fontCollection,
      float scale,
      bool halfleading,
      std::string defaultFontFamilyName)
      : m_styledString(OH_ArkUI_StyledString_Create(typographyStyle, fontCollection.get()), OH_ArkUI_StyledString_Destroy),
        m_scale(scale),
        m_halfleading(halfleading),
        m_defaultFontFamilyName(defaultFontFamilyName),
        m_fontCollection(fontCollection) {}
    
    void setMaximumWidth(facebook::react::Float maximumWidth) {
        if (!isnan(maximumWidth) && maximumWidth > 0) {
            m_maximumWidth = maximumWidth;
        } else {
            m_maximumWidth = std::numeric_limits<facebook::react::Float>::max();
        }
    }

  void addFragment(
      const facebook::react::AttributedString::Fragment& fragment) {
    if (!m_isInitTextAlign) {
      m_textAlign = fragment.textAttributes.alignment;
      m_isInitTextAlign = true;
    }
    if (!fragment.isAttachment()) {
      addTextFragment(fragment);
    } else {
      addAttachment(fragment);
    }
  }

  ArkUITypography build() const {
    return ArkUITypography(
        m_styledString.get(),
        m_attachmentCount,
        m_fragmentLengths,
        m_maximumWidth,
        m_scale,
        m_textAlign);
  }

  ArkUI_StyledString* getTextStyleString() {
    return m_styledString.get();
  }

    float getScale()
    {
        return m_scale;
    }

 private:
  float m_scale;
  std::optional<facebook::react::TextAlignment> m_textAlign{};
  bool m_isInitTextAlign;
  bool m_halfleading;
  std::string m_defaultFontFamilyName;
  std::vector<OH_Drawing_PlaceholderSpan> m_placeholderSpan;
  
  size_t utf16Length(const std::string& str) {
  size_t len = 0;
  const unsigned char* currentByte =
      reinterpret_cast<const unsigned char*>(str.data());
  const unsigned char* endOfBytes = currentByte + str.size();
  //Judge the length of UTF-8 characters based on the number of bits higher than the first byte, take out the significant bits, and record the number of continuation bytes that need to be read
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
    //The remaining bytes are taken and the length is calculated
    while (continuationBytes-- && currentByte < endOfBytes &&
           ((*currentByte & 0xC0) == 0x80)) {
      codePoint =
          (codePoint << 6) | (*currentByte++ & 0x3F);
    }
    //The number of code elements is calculated according to the hexadecimal format
    if (codePoint < 0x10000) {
      len += 1;
    } else {
      len += 2;
    }
  }
  return len;
}
    void addTextFragment(
        const facebook::react::AttributedString::Fragment& fragment) {
        std::
        unique_ptr<OH_Drawing_TextStyle, decltype(&OH_Drawing_DestroyTextStyle)>
            textStyle(
                OH_Drawing_CreateTextStyle(), OH_Drawing_DestroyTextStyle);
        OH_Drawing_SetTextStyleHalfLeading(textStyle.get(), m_halfleading
        || !isnan(fragment.textAttributes.lineHeight));
    // fontSize
    auto fontSize = fragment.textAttributes.fontSize;
    if (fontSize <= 0) {
      // set fontSize to default for negative values(same as iOS)
      fontSize = 14;
    }
    OH_Drawing_SetTextStyleFontSize(textStyle.get(), fontSize * m_scale);

    // fontStyle
    if (fragment.textAttributes.fontStyle.has_value()) {
      OH_Drawing_SetTextStyleFontStyle(textStyle.get(), (int)fragment.textAttributes.fontStyle.value());
    }
    
    // fontColor
    if (fragment.textAttributes.foregroundColor) {
      OH_Drawing_SetTextStyleColor(textStyle.get(), (uint32_t)(*fragment.textAttributes.foregroundColor));
    }
    
    // textDecoration
    int32_t textDecorationType = TEXT_DECORATION_NONE;
    uint32_t textDecorationColor = 0xFF000000;
    int32_t textDecorationStyle = TEXT_DECORATION_STYLE_SOLID;
    if (fragment.textAttributes.textDecorationLineType.has_value()) {
      textDecorationType = (int32_t)fragment.textAttributes.textDecorationLineType.value();
      if (fragment.textAttributes.textDecorationColor) {
        textDecorationColor = (uint32_t)(*fragment.textAttributes.textDecorationColor);
      } else if (fragment.textAttributes.foregroundColor) {
        textDecorationColor = (uint32_t)(*fragment.textAttributes.foregroundColor);
      }
      if (textDecorationType ==
              (int32_t)facebook::react::TextDecorationLineType::Strikethrough ||
          textDecorationType ==
              (int32_t)facebook::react::TextDecorationLineType::
                  UnderlineStrikethrough) {
        textDecorationType = TEXT_DECORATION_LINE_THROUGH;
      }
    }
    if (fragment.textAttributes.textDecorationStyle.has_value()) {
      textDecorationStyle = (int32_t)fragment.textAttributes.textDecorationStyle.value();
    }
    OH_Drawing_SetTextStyleDecoration(textStyle.get(), textDecorationType);
    OH_Drawing_SetTextStyleDecorationColor(textStyle.get(), textDecorationColor);
    OH_Drawing_SetTextStyleDecorationStyle(textStyle.get(), textDecorationStyle);
    
    // backgroundColor
    if (fragment.textAttributes.backgroundColor) {
      std::unique_ptr<
          OH_Drawing_Brush,
          decltype(&OH_Drawing_BrushDestroy)>
          brush(OH_Drawing_BrushCreate(), OH_Drawing_BrushDestroy);
      OH_Drawing_BrushSetColor(brush.get(), (uint32_t)(*fragment.textAttributes.backgroundColor));
      OH_Drawing_SetTextStyleBackgroundBrush(textStyle.get(), brush.get());
    }
    
    // shadow
    std::unique_ptr<
        OH_Drawing_TextShadow,
        decltype(&OH_Drawing_DestroyTextShadow)>
        shadow(OH_Drawing_CreateTextShadow(), OH_Drawing_DestroyTextShadow);

    // new NDK for setting letterSpacing
    if (!isnan(fragment.textAttributes.letterSpacing)) {
      OH_Drawing_SetTextStyleLetterSpacing(textStyle.get(), m_scale * fragment.textAttributes.letterSpacing);
    }
    if (!isnan(fragment.textAttributes.lineHeight) &&
        fragment.textAttributes.lineHeight > 0) {
      // fontSize * fontHeight = lineHeight, no direct ndk for setting
      // lineHeight so do it in this weird way
      double fontHeight = fragment.textAttributes.lineHeight / fontSize;
      OH_Drawing_SetTextStyleFontHeight(textStyle.get(), fontHeight);
    }
    if (fragment.textAttributes.fontWeight.has_value()) {
      OH_Drawing_SetTextStyleFontWeight(
          textStyle.get(),
          mapValueToFontWeight(
              int(fragment.textAttributes.fontWeight.value())));
    }
    std::vector<const char*> fontFamilies;
    if (!m_defaultFontFamilyName.empty() && m_defaultFontFamilyName != "default") {
      fontFamilies.emplace_back(m_defaultFontFamilyName.c_str());
    }
    if (!fragment.textAttributes.fontFamily.empty()) {
      fontFamilies.emplace_back(fragment.textAttributes.fontFamily.c_str());
    }
    if (fontFamilies.size() > 0) {
      OH_Drawing_SetTextStyleFontFamilies(textStyle.get(), fontFamilies.size(), fontFamilies.data());
    }
    if (fragment.textAttributes.fontVariant.has_value()) {
      for (auto& [tag, value] : mapValueToFontVariant(int(fragment.textAttributes.fontVariant.value()))) {
        OH_Drawing_TextStyleAddFontFeature(textStyle.get(), tag.c_str(), value);
      }
    }
    // push text and corresponding textStyle to handler
    OH_ArkUI_StyledString_PushTextStyle(
        m_styledString.get(), textStyle.get());
    OH_ArkUI_StyledString_AddText(
        m_styledString.get(), fragment.string.c_str());
    m_fragmentLengths.emplace_back(utf16Length(fragment.string));
  }

  void addAttachment(
      const facebook::react::AttributedString::Fragment& fragment) {
    // using placeholder span for inline views
    OH_Drawing_PlaceholderSpan inlineView = {
        fragment.parentShadowView.layoutMetrics.frame.size.width * m_scale,
        fragment.parentShadowView.layoutMetrics.frame.size.height * m_scale,
        // align to the line's baseline
        ALIGNMENT_ABOVE_BASELINE,
        TEXT_BASELINE_ALPHABETIC,
        0,
    };
    // push placeholder to handler
    OH_ArkUI_StyledString_AddPlaceholder(
        m_styledString.get(), &inlineView);
    m_placeholderSpan.emplace_back(inlineView);
    m_attachmentCount++;
  }

  OH_Drawing_FontWeight mapValueToFontWeight(int value) {
    switch (value) {
      case 100:
        return OH_Drawing_FontWeight::FONT_WEIGHT_100;
      case 200:
        return OH_Drawing_FontWeight::FONT_WEIGHT_200;
      case 300:
        return OH_Drawing_FontWeight::FONT_WEIGHT_300;
      case 400:
        return OH_Drawing_FontWeight::FONT_WEIGHT_400;
      case 500:
        return OH_Drawing_FontWeight::FONT_WEIGHT_500;
      case 600:
        return OH_Drawing_FontWeight::FONT_WEIGHT_600;
      case 700:
        return OH_Drawing_FontWeight::FONT_WEIGHT_700;
      case 800:
        return OH_Drawing_FontWeight::FONT_WEIGHT_800;
      case 900:
        return OH_Drawing_FontWeight::FONT_WEIGHT_900;
      default:
        return OH_Drawing_FontWeight::FONT_WEIGHT_400;
    }
  }

std::map<std::string, int> mapValueToFontVariant(int value) {
    std::map<std::string, int> fontVariant{};
    if (value & (int)facebook::react::FontVariant::SmallCaps) {
      fontVariant.emplace("smcp", 1);
    }
    if (value & (int)facebook::react::FontVariant::OldstyleNums) {
      fontVariant.emplace("onum", 1);
    }
    if (value & (int)facebook::react::FontVariant::LiningNums) {
      fontVariant.emplace("lnum", 1);
    }
    if (value & (int)facebook::react::FontVariant::TabularNums) {
      fontVariant.emplace("tnum", 1);
    }
    if (value & (int)facebook::react::FontVariant::ProportionalNums) {
      fontVariant.emplace("pnum", 1);
    }
    return fontVariant;
  }

  std::unique_ptr<
    ArkUI_StyledString,
    decltype(&OH_ArkUI_StyledString_Destroy)>
    m_styledString;
  size_t m_attachmentCount = 0;
  std::vector<size_t> m_fragmentLengths{};
  facebook::react::Float m_maximumWidth =
      std::numeric_limits<facebook::react::Float>::max();
  SharedFontCollection m_fontCollection;
};

} // namespace rnoh