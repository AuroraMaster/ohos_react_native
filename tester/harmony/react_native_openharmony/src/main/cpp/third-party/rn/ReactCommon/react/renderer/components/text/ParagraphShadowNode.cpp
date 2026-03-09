/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ParagraphShadowNode.h"

#include <cmath>

#include <react/debug/react_native_assert.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/components/view/ViewShadowNode.h>
#include <react/renderer/components/view/conversions.h>
#include <react/renderer/core/TraitCast.h>
#include <react/renderer/graphics/rounding.h>
#include <react/renderer/telemetry/TransactionTelemetry.h>
#include "RNOH/modalshrink/GuideLayout.h"

#include "ParagraphState.h"
#include <yoga/Yoga.h>
#include "RNOH/TextMeasurer.h"

namespace facebook::react {

using Content = ParagraphShadowNode::Content;

char const ParagraphComponentName[] = "Paragraph";

Content const& ParagraphShadowNode::getContent(
    LayoutContext const& layoutContext) const {
  // Check if content cache refresh is needed (for recalculating font size
  // during Modal scaling)
  auto& guideLayout = GuideLayout::getInstance();
  if (guideLayout.needsContentRefresh(getTag())) {
    content_.reset();
    guideLayout.markContentRefreshed(getTag());
  }

  if (content_.has_value()) {
    return content_.value();
  }

  ensureUnsealed();

  auto textAttributes = TextAttributes::defaultTextAttributes();
  textAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
  textAttributes.apply(getConcreteProps().textAttributes);

  // Modal content font scaling handling
  if (guideLayout.isModalContentShrinkEnabled() &&
      guideLayout.isInModalSubtree(getTag())) {
    float scaleFactor = guideLayout.getScaleFactor();
    // Scale font size
    if (!std::isnan(textAttributes.fontSize) && textAttributes.fontSize > 0) {
      textAttributes.fontSize *= scaleFactor;
    }
    // Scale line height
    if (!std::isnan(textAttributes.lineHeight) &&
        textAttributes.lineHeight > 0) {
      textAttributes.lineHeight *= scaleFactor;
    }
    // Scale letter spacing
    if (!std::isnan(textAttributes.letterSpacing) &&
        textAttributes.letterSpacing > 0) {
      textAttributes.letterSpacing *= scaleFactor;
    }
  }

  textAttributes.layoutDirection =
      YGNodeLayoutGetDirection(&yogaNode_) == YGDirectionRTL
      ? LayoutDirection::RightToLeft
      : LayoutDirection::LeftToRight;
  auto attributedString = AttributedString{};
  auto attachments = Attachments{};
  buildAttributedString(textAttributes, *this, attributedString, attachments);

  content_ = Content{
      attributedString, getConcreteProps().paragraphAttributes, attachments};

  return content_.value();
}

Content ParagraphShadowNode::getContentWithMeasuredAttachments(
    LayoutContext const &layoutContext,
    LayoutConstraints const &layoutConstraints) const {
  auto content = getContent(layoutContext);

  if (content.attachments.empty()) {
    // Base case: No attachments, nothing to do.
    return content;
  }

  auto localLayoutConstraints = layoutConstraints;
  // Having enforced minimum size for text fragments doesn't make much sense.
  localLayoutConstraints.minimumSize = Size{0, 0};

  auto &fragments = content.attributedString.getFragments();

  for (auto const &attachment : content.attachments) {
    auto laytableShadowNode =
        traitCast<LayoutableShadowNode const *>(attachment.shadowNode);

    if (laytableShadowNode == nullptr) {
      continue;
    }

    auto size =
        laytableShadowNode->measure(layoutContext, localLayoutConstraints);

    // Rounding to *next* value on the pixel grid.
    size.width += 0.01f;
    size.height += 0.01f;
    size = roundToPixel<&ceil>(size, layoutContext.pointScaleFactor);

    auto fragmentLayoutMetrics = LayoutMetrics{};
    fragmentLayoutMetrics.pointScaleFactor = layoutContext.pointScaleFactor;
    fragmentLayoutMetrics.frame.size = size;
    fragments[attachment.fragmentIndex].parentShadowView.layoutMetrics =
        fragmentLayoutMetrics;
  }

  return content;
}

void ParagraphShadowNode::setTextLayoutManager(
    std::shared_ptr<TextLayoutManager const> textLayoutManager) {
  ensureUnsealed();
  getStateData().paragraphLayoutManager.setTextLayoutManager(
      std::move(textLayoutManager));
  initYogaBaselineFunc();
}

void ParagraphShadowNode::updateStateIfNeeded(Content const &content) {
  ensureUnsealed();

  auto &state = getStateData();

  react_native_assert(state.paragraphLayoutManager.getTextLayoutManager());

  if (state.attributedString == content.attributedString) {
    return;
  }

  setStateData(ParagraphState{
          content.attributedString,
          content.paragraphAttributes,
          state.paragraphLayoutManager});
}
    
/*
 * Initializes the baseline function for Yoga layout engine.
 *
 * This function sets up the baseline callback for Text components, enabling
 * correct baseline offset calculation when parent containers use
 * alignItems: 'baseline'.
 *
 * Call timing: Called in setTextLayoutManager to ensure TextLayoutManager is set
 */
void ParagraphShadowNode::initYogaBaselineFunc() {
  YGNodeSetContext(&yogaNode_, this);
  YGNodeSetBaselineFunc(&yogaNode_, paragraphBaselineFunc);
}

/*
 * Baseline callback function for Yoga layout engine.
 *
 * This function is called by the Yoga layout engine to calculate the baseline
 * value for Text components. The baseline value is the distance from the
 * text baseline to the top (ascender), used for baseline alignment.
 *
 * @param node Reference to the Yoga node, used to get ParagraphShadowNode instance via YGNodeGetContext
 * @param width Text width constraint, used for text measurement
 * @param height Text height constraint, used for text measurement
 * @return float Baseline value (ascender), which is the distance from baseline to top
 *
 * Notes:
 * - Uses first line's baseline value, following CSS specification
 * - Returns 0 for empty text to avoid layout errors
 * - Ascender is a positive value representing distance upward from baseline
 */
float ParagraphShadowNode::paragraphBaselineFunc(YGNodeRef node, float width, float height) {
  auto shadowNode = reinterpret_cast<ParagraphShadowNode const *>(YGNodeGetContext(node));
  if (!shadowNode) {
    return 0.0f;
  }

  auto const &state = shadowNode->getStateData();
  auto textLayoutManager = state.paragraphLayoutManager.getTextLayoutManager();
  if (!textLayoutManager) {
    return 0.0f;
  }
        
  auto layoutMetrics = shadowNode->getLayoutMetrics();
  auto layoutConstraints = LayoutConstraints{Size{width, height}, Size{width, height}, layoutMetrics.layoutDirection};
  auto layoutContext = reinterpret_cast<LayoutContext const *>(YGNodeGetContext(node));
  auto content = shadowNode->getContentWithMeasuredAttachments(*layoutContext, layoutConstraints);
  auto const &props = shadowNode->getConcreteProps();
  auto attributedString = content.attributedString;

  if (attributedString.isEmpty()) {
    // Note: zero-width space is insufficient in some cases (e.g. when we need
    // to measure the "height" of the font).
    // TODO T67606511: We will redefine the measurement of empty strings as part
    // of T67606511
    auto string = BaseTextShadowNode::getEmptyPlaceholder();
    auto textAttributes = TextAttributes::defaultTextAttributes();
    textAttributes.fontSizeMultiplier = layoutContext->fontSizeMultiplier;
    textAttributes.apply(props.textAttributes);
    attributedString.appendFragment({string, textAttributes, {}});
  }

  auto nativeTextLayoutManager = textLayoutManager->getNativeTextLayoutManager();
  auto textMeasurer = static_cast<rnoh::TextMeasurer*>(nativeTextLayoutManager);
  auto lineMetrics = textMeasurer->getLineMetrics(
      content.attributedString,
      props.paragraphAttributes,
      {{width, height}, {width, height}});
  if (lineMetrics.empty()) {
    return 0.0f;
  }

  return lineMetrics[0].ascender;
}

#pragma mark - LayoutableShadowNode

Size ParagraphShadowNode::measureContent(
    LayoutContext const &layoutContext,
    LayoutConstraints const &layoutConstraints) const {
  auto content =
      getContentWithMeasuredAttachments(layoutContext, layoutConstraints);

  auto attributedString = content.attributedString;
  if (attributedString.isEmpty()) {
    // Note: `zero-width space` is insufficient in some cases (e.g. when we need
    // to measure the "height" of the font).
    // TODO T67606511: We will redefine the measurement of empty strings as part
    // of T67606511
    auto string = BaseTextShadowNode::getEmptyPlaceholder();
    auto textAttributes = TextAttributes::defaultTextAttributes();
    textAttributes.fontSizeMultiplier = layoutContext.fontSizeMultiplier;
    textAttributes.apply(getConcreteProps().textAttributes);
    attributedString.appendFragment({string, textAttributes, {}});
  }

  return getStateData()
      .paragraphLayoutManager
      .measure(attributedString, content.paragraphAttributes, layoutConstraints)
      .size;
}

void ParagraphShadowNode::layout(LayoutContext layoutContext) {
  ensureUnsealed();

  auto layoutMetrics = getLayoutMetrics();
  auto availableSize = layoutMetrics.getContentFrame().size;

  auto layoutConstraints = LayoutConstraints{
      availableSize, availableSize, layoutMetrics.layoutDirection};
  auto content =
      getContentWithMeasuredAttachments(layoutContext, layoutConstraints);

  updateStateIfNeeded(content);

  auto measurement = getStateData().paragraphLayoutManager.measure(
      content.attributedString, content.paragraphAttributes, layoutConstraints);

  if (getConcreteProps().onTextLayout) {
    auto linesMeasurements = getStateData().paragraphLayoutManager.measureLines(
        content.attributedString,
        content.paragraphAttributes,
        measurement.size);
    getConcreteEventEmitter().onTextLayout(linesMeasurements);
  }

  if (content.attachments.empty()) {
    // No attachments to layout.
    return;
  }

  //  Iterating on attachments, we clone shadow nodes and moving
  //  `paragraphShadowNode` that represents clones of `this` object.
  auto paragraphShadowNode = static_cast<ParagraphShadowNode *>(this);
  // `paragraphOwningShadowNode` is owning pointer to`paragraphShadowNode`
  // (besides the initial case when `paragraphShadowNode == this`), we need this
  // only to keep it in memory for a while.
  auto paragraphOwningShadowNode = ShadowNode::Unshared{};

  react_native_assert(
      content.attachments.size() == measurement.attachments.size());

  for (size_t i = 0; i < content.attachments.size(); i++) {
    auto &attachment = content.attachments.at(i);

    if (traitCast<LayoutableShadowNode const *>(attachment.shadowNode) ==
        nullptr) {
      // Not a layoutable `ShadowNode`, no need to lay it out.
      continue;
    }

    auto clonedShadowNode = ShadowNode::Unshared{};

    paragraphOwningShadowNode = paragraphShadowNode->cloneTree(
        attachment.shadowNode->getFamily(),
        [&](ShadowNode const &oldShadowNode) {
          clonedShadowNode = oldShadowNode.clone({});
          return clonedShadowNode;
        });
    paragraphShadowNode =
        static_cast<ParagraphShadowNode *>(paragraphOwningShadowNode.get());

    auto &layoutableShadowNode =
        traitCast<LayoutableShadowNode &>(*clonedShadowNode);

    auto attachmentFrame = measurement.attachments[i].frame;
    auto attachmentSize = roundToPixel<&ceil>(
        attachmentFrame.size, layoutMetrics.pointScaleFactor);
    auto attachmentOrigin = roundToPixel<&round>(
        attachmentFrame.origin, layoutMetrics.pointScaleFactor);
    auto attachmentLayoutContext = layoutContext;
    auto attachmentLayoutConstrains = LayoutConstraints{
        attachmentSize, attachmentSize, layoutConstraints.layoutDirection};

    // Laying out the `ShadowNode` and the subtree starting from it.
    layoutableShadowNode.layoutTree(
        attachmentLayoutContext, attachmentLayoutConstrains);

    // Altering the origin of the `ShadowNode` (which is defined by text layout,
    // not by internal styles and state).
    auto attachmentLayoutMetrics = layoutableShadowNode.getLayoutMetrics();
    attachmentLayoutMetrics.frame.origin = attachmentOrigin;
    layoutableShadowNode.setLayoutMetrics(attachmentLayoutMetrics);
  }

  // If we ended up cloning something, we need to update the list of children to
  // reflect the changes that we made.
  if (paragraphShadowNode != this) {
    this->children_ =
        static_cast<ParagraphShadowNode const *>(paragraphShadowNode)
            ->children_;
  }
}

} // namespace facebook::react
