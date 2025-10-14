/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "TextLayoutManager.h"
#include <codecvt>

namespace facebook {
namespace react {

void *TextLayoutManager::getNativeTextLayoutManager() const {
    return (void *)m_textLayoutManagerDelegate.get();
}

TextMeasurement TextLayoutManager::measure(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
    auto &attributedString = attributedStringBox.getValue();
    return m_textLayoutManagerDelegate->measure(attributedString, paragraphAttributes, layoutConstraints);
}

TextMeasurement TextLayoutManager::measure(
    AttributedStringBox attributedStringBox,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints,
    std::shared_ptr<void> hostTextStorage) const {
    return this->measure(attributedStringBox, paragraphAttributes, layoutConstraints);
}

float  TextLayoutManager::getDPI() const{
    if( m_textLayoutManagerDelegate != nullptr){
        return m_textLayoutManagerDelegate->getDPI();
    }
}

float  TextLayoutManager::getScale() const
{
    if (m_textLayoutManagerDelegate != nullptr)
    {
        return m_textLayoutManagerDelegate->getScale();
    }
}

LinesMeasurements TextLayoutManager::measureLines(
    AttributedString attributedString,
    ParagraphAttributes paragraphAttributes,
    Size size) const {
    if (attributedString.getFragments().empty()) {
        return LinesMeasurements{};
    }    
    std::vector<OH_Drawing_LineMetrics> lineMetrics =
        m_textLayoutManagerDelegate->getLineMetrics(attributedString, paragraphAttributes, {size, size});
    VLOG(3) << "lineMetrics size=" << lineMetrics.size();
    
    std::string text = "";
    auto const& fragments = attributedString.getFragments();
    bool hasAttachmentCharacter = false;
    for (const auto& fragment : fragments) {
        if (fragment.isAttachment()) {
            hasAttachmentCharacter = true;
        }
        text += fragment.string.c_str();
    }
    std::u16string u16Text = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text);
        
    LinesMeasurements ret{};
    for (int i = 0; i < lineMetrics.size(); i++) {
        Rect frame;
        frame.origin.x = static_cast<Float>(lineMetrics[i].x);
        frame.origin.y = static_cast<Float>(lineMetrics[i].y);
        frame.size.width = static_cast<Float>(lineMetrics[i].width);
        frame.size.height = static_cast<Float>(lineMetrics[i].height);
        VLOG(3) << "lineMetrics index=" << i << ", frame.origin.x=" << frame.origin.x
            << ", frame.origin.y=" << frame.origin.y << ", frame.size.width=" << frame.size.width
            << ", frame.size.height=" << frame.size.height;
        
        Float descender = static_cast<Float>(lineMetrics[i].descender);
        Float ascender = static_cast<Float>(lineMetrics[i].ascender);
        Float capHeight = static_cast<Float>(lineMetrics[i].capHeight);
        Float xHeight = static_cast<Float>(lineMetrics[i].xHeight);
        std::u16string u16LineText;
        auto pos = lineMetrics[i].startIndex;
        auto len = lineMetrics[i].endIndex - lineMetrics[i].startIndex;
        if (pos > u16Text.length()) {
            LOG(ERROR) << "TextLayoutManager pos is out of range, text length = "
                << u16Text.length() << ", pos = " << pos;
            return ret;
        }
        if (pos != std::string::npos && pos + len > u16Text.length()) {
            LOG(ERROR) << "TextLayoutManager pos + len is out of range, text length = "
                << u16Text.length() << ", pos = " << pos << ", len = " << len;
            return ret;
        }
        u16LineText = u16Text.substr(pos, len);
        if (hasAttachmentCharacter) {
            // NOTE: Use std::remove and erase to remove the placeholder character `\uFFFC`
            // in the UTF-16 string (corresponding to `0xFFFC` in UTF-16)
            u16LineText.erase(std::remove(u16LineText.begin(), u16LineText.end(), 0xFFFC), u16LineText.end());
        }
        std::string lineText = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16LineText);
        
        LineMeasurement line(lineText, frame, descender, capHeight, ascender, xHeight);
        ret.push_back(line);
    }
    
    return ret;
}

std::shared_ptr<void> TextLayoutManager::getHostTextStorage(
    AttributedString attributedString,
    ParagraphAttributes paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
    return nullptr;
}

} // namespace react
} // namespace facebook
