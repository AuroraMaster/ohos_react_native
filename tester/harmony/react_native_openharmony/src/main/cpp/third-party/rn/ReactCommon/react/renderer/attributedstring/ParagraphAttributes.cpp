/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ParagraphAttributes.h"

#include <react/renderer/attributedstring/conversions.h>
#include <react/renderer/core/graphicsConversions.h>
#include <react/renderer/debug/debugStringConvertibleUtils.h>
#include <react/utils/FloatComparison.h>

namespace facebook::react {

bool ParagraphAttributes::operator==(const ParagraphAttributes &rhs) const {
  return std::tie(
             maximumNumberOfLines,
             ellipsizeMode,
             textBreakStrategy,
             adjustsFontSizeToFit,
             includeFontPadding,
             android_hyphenationFrequency,
             allowFontScaling,
             writingDirection) ==
      std::tie(
             rhs.maximumNumberOfLines,
             rhs.ellipsizeMode,
             rhs.textBreakStrategy,
             rhs.adjustsFontSizeToFit,
             rhs.includeFontPadding,
             rhs.android_hyphenationFrequency,
             rhs.allowFontScaling,
             rhs.writingDirection) &&
      floatEquality(minimumFontSize, rhs.minimumFontSize) &&
      floatEquality(maximumFontSize, rhs.maximumFontSize) &&
      floatEquality(minimumFontScale, rhs.minimumFontScale) &&
      floatEquality(maxFontSizeMultiplier, rhs.maxFontSizeMultiplier);
}

bool ParagraphAttributes::operator!=(const ParagraphAttributes &rhs) const {
  return !(*this == rhs);
}

#pragma mark - DebugStringConvertible

#if RN_DEBUG_STRING_CONVERTIBLE
SharedDebugStringConvertibleList ParagraphAttributes::getDebugProps() const {
  return {
      debugStringConvertibleItem("maximumNumberOfLines", maximumNumberOfLines),
      debugStringConvertibleItem("ellipsizeMode", ellipsizeMode),
      debugStringConvertibleItem("textBreakStrategy", textBreakStrategy),
      debugStringConvertibleItem("adjustsFontSizeToFit", adjustsFontSizeToFit),
      debugStringConvertibleItem("minimumFontSize", minimumFontSize),
      debugStringConvertibleItem("maximumFontSize", maximumFontSize),
      debugStringConvertibleItem("includeFontPadding", includeFontPadding),
      debugStringConvertibleItem(
          "android_hyphenationFrequency", android_hyphenationFrequency),
      debugStringConvertibleItem("minimumFontScale", minimumFontScale)};
}
#endif

} // namespace facebook::react
