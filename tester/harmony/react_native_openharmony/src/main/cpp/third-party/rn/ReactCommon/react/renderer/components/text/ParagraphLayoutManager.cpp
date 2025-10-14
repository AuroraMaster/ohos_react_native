/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ParagraphLayoutManager.h"
#include <folly/Hash.h>
#include <react/renderer/core/CoreFeatures.h>

namespace facebook::react {

TextMeasurement ParagraphLayoutManager::measure(
    AttributedString const &attributedString,
    ParagraphAttributes const &paragraphAttributes,
    LayoutConstraints layoutConstraints) const {
  bool cacheLastTextMeasurement = CoreFeatures::cacheLastTextMeasurement;
  if (cacheLastTextMeasurement &&
    // Value 0.5f is too small to judge, so 0.8f is a better number.
      ((std::abs(layoutConstraints.maximumSize.width - availableWidth_) <= 0.8f) ||
       (std::abs(layoutConstraints.maximumSize.width -
           cachedTextMeasurement_.size.width) <= 0.8f))) {
    /* Yoga has requested measurement for this size before. Let's use cached
     * value. `TextLayoutManager` might not have cached this because it could be
     * using different width to generate cache key. This happens because Yoga
     * switches between available width and exact width but since we already
     * know exact width, it is wasteful to calculate it again.
     */
    isCacheChanged = false;
  } else {
    isCacheChanged = true;
  }
  if (CoreFeatures::cacheNSTextStorage) {
    size_t newHash = folly::hash::hash_combine(
        0,
        textAttributedStringHashLayoutWise(attributedString),
        paragraphAttributes);
    if (hash_ == 0 || newHash != hash_) {
      hash_ = newHash;
    } else {
      if (isCacheChanged == false) {
        return cachedTextMeasurement_;
      }
    }
  }

  if (cacheLastTextMeasurement) {
    cachedTextMeasurement_ = textLayoutManager_->measure(
        AttributedStringBox(attributedString),
        paragraphAttributes,
        layoutConstraints,
        hostTextStorage_);

    availableWidth_ = layoutConstraints.maximumSize.width;

    return cachedTextMeasurement_;
  } else {
    return textLayoutManager_->measure(
        AttributedStringBox(attributedString),
        paragraphAttributes,
        layoutConstraints,
        hostTextStorage_);
  }
}

LinesMeasurements ParagraphLayoutManager::measureLines(
    AttributedString const &attributedString,
    ParagraphAttributes const &paragraphAttributes,
    Size size) const {
  return textLayoutManager_->measureLines(
      attributedString, paragraphAttributes, size);
}

void ParagraphLayoutManager::setTextLayoutManager(
    std::shared_ptr<TextLayoutManager const> textLayoutManager) const {
  textLayoutManager_ = std::move(textLayoutManager);
}

void ParagraphLayoutManager::resetCache() const {
  // Reset compare parameters
  hash_ = 0;
  availableWidth_ = 0.0f;
}

std::shared_ptr<TextLayoutManager const>
ParagraphLayoutManager::getTextLayoutManager() const {
  return textLayoutManager_;
}

std::shared_ptr<void> ParagraphLayoutManager::getHostTextStorage() const {
  return hostTextStorage_;
}
} // namespace facebook::react
