/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "GuideLayout.h"
#include <react/renderer/components/view/YogaStylableProps.h>
#include <react/renderer/core/ShadowNode.h>
#include <cmath>
#include <cstring>

namespace facebook {
namespace react {

bool GuideLayout::scanAndScaleModalSubtrees(YGNodeRef rootYogaNode) {
  if (!rootYogaNode || !enableModalContentShrink_) {
    return false;
  }

  bool foundModal = false;

  // Safety check: ensure context is valid
  void* context = rootYogaNode->getContext();
  if (!context) {
    // No context, recursively check child nodes
    size_t childCount = YGNodeGetChildCount(rootYogaNode);
    for (size_t i = 0; i < childCount; ++i) {
      YGNodeRef child = YGNodeGetChild(rootYogaNode, i);
      if (child && scanAndScaleModalSubtrees(child)) {
        foundModal = true;
      }
    }
    return foundModal;
  }

  auto* shadowNode = static_cast<ShadowNode*>(context);
  if (!shadowNode) {
    return false;
  }

  const char* componentName = shadowNode->getComponentName();
  if (!componentName) {
    return false;
  }

  if (std::strcmp(componentName, "ModalHostView") == 0) {
    // Found Modal node, scale all its children
    // Use scaling function with percentage handling
    // Modal node itself doesn't have percentage, so pass false for both
    size_t childCount = YGNodeGetChildCount(rootYogaNode);
    for (size_t i = 0; i < childCount; ++i) {
      YGNodeRef child = YGNodeGetChild(rootYogaNode, i);
      scaleYogaNodeStyleWithPercentHandling(child, false, false);
      // Collect all node Tags in subtree for font scaling judgment
      collectModalSubtreeTags(child);
    }

    // Add marginTop to L5 node for vertical centering after scaling
    // Modal (L0) -> L1 -> L2 -> L3 -> L4 -> L5
    // Calculate offset: screenHeight * (1 - scaleFactor) / 2
    if (scaleFactor_ < 1.0f && screenHeight_ > 0) {
      YGNodeRef L4 = findL4Node(rootYogaNode);
      if (L4 && YGNodeGetChildCount(L4) > 0) {
        YGNodeRef L5 = YGNodeGetChild(L4, 0);
        if (L5) {
          float marginTopOffset = screenHeight_ * (1.0f - scaleFactor_) / 2.0f;
          auto styleL5 = L5->getStyle();

          // Get existing marginTop and add offset
          YGValue existingMargin = styleL5.margin()[YGEdgeTop];
          float existingValue = 0.0f;
          if (existingMargin.unit == YGUnitPoint &&
              !YGFloatIsUndefined(existingMargin.value)) {
            existingValue = existingMargin.value;
          }

          styleL5.margin()[YGEdgeTop] =
              YGValue{existingValue + marginTopOffset, YGUnitPoint};
          L5->setStyle(styleL5);
          L5->setDirty(true);
        }
      }
    }

    foundModal = true;
    // No need to continue recursion, Modal's subtree has been processed
    return true;
  }

  // Recursively scan child nodes
  size_t childCount = YGNodeGetChildCount(rootYogaNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(rootYogaNode, i);
    if (child && scanAndScaleModalSubtrees(child)) {
      foundModal = true;
    }
  }

  return foundModal;
}

void GuideLayout::scaleYogaNodeStyleWithPercentHandling(
    YGNodeRef yogaNode,
    bool parentHasPercentWidth,
    bool parentHasPercentHeight) {
  if (!yogaNode) {
    return;
  }

  auto& style = yogaNode->getStyle();
  bool modified = false;

  // Check if current node has percentage width/height
  YGValue width = style.dimensions()[YGDimensionWidth];
  YGValue height = style.dimensions()[YGDimensionHeight];
  bool currentHasPercentWidth =
      (width.unit == YGUnitPercent && width.value > 0);
  bool currentHasPercentHeight =
      (height.unit == YGUnitPercent && height.value > 0);

  // Check if current node has explicit dimensions (points or percentage).
  // Nodes without explicit dimensions (e.g., flex layout) derive their size
  // from the parent container. Scaling their padding would alter the content
  // area and cause percentage-based children to be scaled incorrectly.
  bool hasExplicitWidth =
      (width.unit == YGUnitPoint && !YGFloatIsUndefined(width.value) &&
       width.value > 0) ||
      currentHasPercentWidth;
  bool hasExplicitHeight =
      (height.unit == YGUnitPoint && !YGFloatIsUndefined(height.value) &&
       height.value > 0) ||
      currentHasPercentHeight;

  // Scale width - supports percentage
  if (width.unit == YGUnitPoint && width.value > 0) {
    style.dimensions()[YGDimensionWidth] =
        YGValue{width.value * scaleFactor_, YGUnitPoint};
    modified = true;
  } else if (currentHasPercentWidth && !parentHasPercentWidth) {
    // Scale percentage width only if parent doesn't have percentage width
    // If parent has percentage, child's percentage is relative to parent's
    // scaled size
    style.dimensions()[YGDimensionWidth] =
        YGValue{width.value * scaleFactor_, YGUnitPercent};
    modified = true;
  }

  // Scale height - supports percentage
  if (height.unit == YGUnitPoint && height.value > 0) {
    style.dimensions()[YGDimensionHeight] =
        YGValue{height.value * scaleFactor_, YGUnitPoint};
    modified = true;
  } else if (currentHasPercentHeight && !parentHasPercentHeight) {
    // Scale percentage height only if parent doesn't have percentage height
    // If parent has percentage, child's percentage is relative to parent's
    // scaled size
    style.dimensions()[YGDimensionHeight] =
        YGValue{height.value * scaleFactor_, YGUnitPercent};
    modified = true;
  }

  // Scale minimum dimensions (only process absolute values)
  YGValue minWidth = style.minDimensions()[YGDimensionWidth];
  if (minWidth.unit == YGUnitPoint && minWidth.value > 0) {
    style.minDimensions()[YGDimensionWidth] =
        YGValue{minWidth.value * scaleFactor_, YGUnitPoint};
    modified = true;
  }

  YGValue minHeight = style.minDimensions()[YGDimensionHeight];
  if (minHeight.unit == YGUnitPoint && minHeight.value > 0) {
    style.minDimensions()[YGDimensionHeight] =
        YGValue{minHeight.value * scaleFactor_, YGUnitPoint};
    modified = true;
  }

  // Scale maximum dimensions (only process absolute values)
  YGValue maxWidth = style.maxDimensions()[YGDimensionWidth];
  if (maxWidth.unit == YGUnitPoint && maxWidth.value > 0) {
    style.maxDimensions()[YGDimensionWidth] =
        YGValue{maxWidth.value * scaleFactor_, YGUnitPoint};
    modified = true;
  }

  YGValue maxHeight = style.maxDimensions()[YGDimensionHeight];
  if (maxHeight.unit == YGUnitPoint && maxHeight.value > 0) {
    style.maxDimensions()[YGDimensionHeight] =
        YGValue{maxHeight.value * scaleFactor_, YGUnitPoint};
    modified = true;
  }

  // Scale margin (only process absolute values)
  for (int edge = YGEdgeLeft; edge <= YGEdgeAll; ++edge) {
    YGValue margin = style.margin()[static_cast<YGEdge>(edge)];
    if (margin.unit == YGUnitPoint && margin.value > 0) {
      style.margin()[static_cast<YGEdge>(edge)] =
          YGValue{margin.value * scaleFactor_, YGUnitPoint};
      modified = true;
    }
  }

  // Scale padding (only process absolute values)
  // Skip padding on axes where the node has no explicit dimension (flex layout),
  // because scaling such padding would change the content area and interfere
  // with the scaling of percentage-based children.
  for (int edge = YGEdgeLeft; edge <= YGEdgeAll; ++edge) {
    auto padEdge = static_cast<YGEdge>(edge);

    // Skip horizontal padding when node has no explicit width
    if (!hasExplicitWidth &&
        (padEdge == YGEdgeLeft || padEdge == YGEdgeRight ||
         padEdge == YGEdgeStart || padEdge == YGEdgeEnd ||
         padEdge == YGEdgeHorizontal)) {
      continue;
    }
    // Skip vertical padding when node has no explicit height
    if (!hasExplicitHeight &&
        (padEdge == YGEdgeTop || padEdge == YGEdgeBottom ||
         padEdge == YGEdgeVertical)) {
      continue;
    }
    // Edge::All applies to all axes. When one axis has an explicit dimension
    // but the other does not, we cannot simply scale or skip Edge::All.
    // Instead, decompose it: scale only the axes that have explicit dimensions.
    if (padEdge == YGEdgeAll) {
      YGValue padding = style.padding()[YGEdgeAll];
      if (padding.unit == YGUnitPoint && !YGFloatIsUndefined(padding.value) &&
          padding.value > 0) {
        if (hasExplicitWidth && hasExplicitHeight) {
          // Both axes explicit: scale Edge::All as before
          style.padding()[YGEdgeAll] =
              YGValue{padding.value * scaleFactor_, YGUnitPoint};
          modified = true;
        } else if (hasExplicitWidth && !hasExplicitHeight) {
          // Only width explicit: scale horizontal padding, keep vertical
          style.padding()[YGEdgeHorizontal] =
              YGValue{padding.value * scaleFactor_, YGUnitPoint};
          modified = true;
        } else if (!hasExplicitWidth && hasExplicitHeight) {
          // Only height explicit: scale vertical padding, keep horizontal
          style.padding()[YGEdgeVertical] =
              YGValue{padding.value * scaleFactor_, YGUnitPoint};
          modified = true;
        }
        // Neither axis explicit: skip entirely
      }
      continue;
    }

    YGValue padding = style.padding()[padEdge];
    if (padding.unit == YGUnitPoint && !YGFloatIsUndefined(padding.value) &&
        padding.value > 0) {
      style.padding()[padEdge] =
          YGValue{padding.value * scaleFactor_, YGUnitPoint};
      modified = true;
    }
  }

  if (modified) {
    yogaNode->setDirty(true);
  }

  // Recursively process child nodes
  // Pass whether current node or any ancestor has percentage width/height
  bool childParentHasPercentWidth =
      parentHasPercentWidth || currentHasPercentWidth;
  bool childParentHasPercentHeight =
      parentHasPercentHeight || currentHasPercentHeight;

  size_t childCount = YGNodeGetChildCount(yogaNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(yogaNode, i);
    scaleYogaNodeStyleWithPercentHandling(
        child, childParentHasPercentWidth, childParentHasPercentHeight);
  }
}

void GuideLayout::collectModalSubtreeTags(YGNodeRef yogaNode) {
  if (!yogaNode) {
    return;
  }

  // Safety check: ensure context is valid
  void* context = yogaNode->getContext();
  if (context) {
    auto* shadowNode = static_cast<ShadowNode*>(context);
    if (shadowNode) {
      int32_t tag = shadowNode->getTag();
      {
        std::lock_guard<std::mutex> lock(mutex_);
        modalSubtreeTags_.insert(tag);

        // If it's a Paragraph node, mark it as needing content cache refresh
        const char* componentName = shadowNode->getComponentName();
        if (componentName && std::strcmp(componentName, "Paragraph") == 0) {
          needsContentRefreshTags_.insert(tag);
        }
      }
    }
  }

  // Recursively collect child nodes
  size_t childCount = YGNodeGetChildCount(yogaNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(yogaNode, i);
    if (child) {
      collectModalSubtreeTags(child);
    }
  }
}

float GuideLayout::findMaxContentHeight(YGNodeRef node, float screenHeight) {
  if (!node) {
    return 0;
  }

  // Get node's own layout height
  float selfHeight = YGNodeLayoutGetHeight(node);

  // Calculate the maximum bottom position stretched by child nodes
  float maxChildBottom = 0;
  size_t childCount = YGNodeGetChildCount(node);

  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(node, i);
    if (!child) {
      continue;
    }

    // Skip absolute positioned nodes - they are out of normal document flow
    auto childStyle = child->getStyle();
    if (childStyle.positionType() == YGPositionTypeAbsolute) {
      continue;
    }

    // Get child's top offset relative to parent
    float childTop = YGNodeLayoutGetTop(child);

    // Recursively get child's effective height (considering its nested
    // children)
    float childEffectiveHeight = findMaxContentHeight(child, screenHeight);

    // Child's bottom position in parent's coordinate system
    float childBottom = childTop + childEffectiveHeight;

    if (childBottom > maxChildBottom) {
      maxChildBottom = childBottom;
    }
  }

  // Determine effective height: max(self height, height stretched by children)
  // For both percentage and non-percentage nodes, use the same rule so that
  // padding/border and node's own height are not undercounted
  float effectiveHeight =
      (maxChildBottom > selfHeight) ? maxChildBottom : selfHeight;

  return effectiveHeight;
}

YGNodeRef GuideLayout::findModalNode(YGNodeRef rootYogaNode) {
  if (!rootYogaNode) {
    return nullptr;
  }

  // Safety check: ensure context is valid
  void* context = rootYogaNode->getContext();
  if (!context) {
    return nullptr;
  }

  // Get corresponding ShadowNode from YogaNode's context
  auto* shadowNode = static_cast<ShadowNode*>(context);
  if (!shadowNode) {
    return nullptr;
  }

  const char* componentName = shadowNode->getComponentName();
  if (componentName && std::strcmp(componentName, "ModalHostView") == 0) {
    return rootYogaNode;
  }

  // Recursively find child nodes
  size_t childCount = YGNodeGetChildCount(rootYogaNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(rootYogaNode, i);
    if (child) {
      YGNodeRef found = findModalNode(child);
      if (found) {
        return found;
      }
    }
  }

  return nullptr;
}

YGNodeRef GuideLayout::findL4Node(YGNodeRef modalNode) {
  // Find L4 node: Modal (L0) -> L1 -> L2 -> L3 -> L4
  // At each level, pick the first child that itself has children,
  // to skip leaf nodes like overlay.
  if (!modalNode) {
    return nullptr;
  }

  auto pickChild = [](YGNodeRef parent) -> YGNodeRef {
    if (!parent) {
      return nullptr;
    }
    size_t count = YGNodeGetChildCount(parent);
    for (size_t i = 0; i < count; ++i) {
      YGNodeRef child = YGNodeGetChild(parent, i);
      if (child && YGNodeGetChildCount(child) > 0) {
        return child;
      }
    }
    // fallback: return first child even if it has no children
    return (count > 0) ? YGNodeGetChild(parent, 0) : nullptr;
  };

  // L0 -> L1
  YGNodeRef L1 = pickChild(modalNode);
  if (!L1) {
    return nullptr;
  }

  // L1 -> L2
  YGNodeRef L2 = pickChild(L1);
  if (!L2) {
    return nullptr;
  }

  // L2 -> L3
  YGNodeRef L3 = pickChild(L2);
  if (!L3) {
    return nullptr;
  }
  // L3 -> L4
  YGNodeRef L4 = pickChild(L3);
  return L4;
}

float GuideLayout::calculateTopDistance(YGNodeRef L4) {
  // Calculate top distance: L4.top + L5.top
  // L4 -> L5
  const int DEFAULT_VALUE = 100;

  if (!L4) {
    return DEFAULT_VALUE;
  }

  float L4Top = YGNodeLayoutGetTop(L4);

  // Get L5: L4's first child
  if (YGNodeGetChildCount(L4) == 0) {
    return DEFAULT_VALUE;
  }
  YGNodeRef L5 = YGNodeGetChild(L4, 0);
  if (!L5) {
    return DEFAULT_VALUE;
  }
  float L5Top = YGNodeLayoutGetTop(L5);

  return L4Top + L5Top;
}

bool GuideLayout::checkModalNeedsScaling(
    YGNodeRef rootYogaNode,
    float screenHeight) {
  if (!rootYogaNode || !enableModalContentShrink_ || screenHeight <= 0) {
    return false;
  }

  // Subtract estimated height of status bar and navigation bar for scale
  // calculation
  constexpr float SYSTEM_BARS_HEIGHT = 45.0f;
  float availableHeight = screenHeight - SYSTEM_BARS_HEIGHT;
  if (availableHeight <= 0) {
    availableHeight = screenHeight;
  }

  // Cache available height
  {
    std::lock_guard<std::mutex> lock(mutex_);
    screenHeight_ = availableHeight;
  }

  // Find Modal node
  YGNodeRef modalNode = findModalNode(rootYogaNode);
  if (!modalNode) {
    // No Modal, reset scale factor
    std::lock_guard<std::mutex> lock(mutex_);
    scaleFactor_ = 1.0f;
    return false;
  }

  // Get Modal Tag for dedup check
  void* modalContext = modalNode->getContext();
  int32_t modalTag = -1;
  if (modalContext) {
    auto* modalShadowNode = static_cast<ShadowNode*>(modalContext);
    if (modalShadowNode) {
      modalTag = modalShadowNode->getTag();
    }
  }

  // Get L4 node from Modal
  YGNodeRef L4 = findL4Node(modalNode);
  if (!L4) {
    // L4 not found, cannot proceed with scaling
    std::lock_guard<std::mutex> lock(mutex_);
    scaleFactor_ = 1.0f;
    return false;
  }

  // Recursively find maximum content height in L5 subtree
  // At this point, if Modal was previously scaled, the caller has already
  // restored original styles and relaid out, so layout results are original
  YGNodeRef L5 =
      (YGNodeGetChildCount(L4) > 0) ? YGNodeGetChild(L4, 0) : nullptr;
  float maxContentHeight = L5 ? findMaxContentHeight(L5, screenHeight) : 0;

  // Check cache: if content hasn't changed, reuse cached result
  if (modalTag >= 0) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto cacheIt = modalScaleCache_.find(modalTag);
    if (cacheIt != modalScaleCache_.end()) {
      bool contentChanged =
          std::abs(maxContentHeight - cacheIt->second.maxContentHeight) > 1.0f;
      bool screenChanged =
          std::abs(screenHeight - cacheIt->second.screenHeight) > 1.0f;
      if (!contentChanged && !screenChanged) {
        scaleFactor_ = cacheIt->second.scaleFactor;
        topDistance_ = cacheIt->second.topDistance;
        return cacheIt->second.needsScale;
      }
      // Content changed, clear cache and recalculate
      scaledModalTags_.erase(modalTag);
      modalScaleCache_.erase(cacheIt);
    }
  }

  // Calculate top distance: L4.top + L5.top
  float topDist = calculateTopDistance(L4);

  bool needsScale = maxContentHeight > screenHeight + 0.5;

  // Additional scaling condition: if topDistance_ < 30
  constexpr float MIN_TOP_THRESHOLD = 30.0f;
  bool needsScaleForSmallTop = (topDist < MIN_TOP_THRESHOLD);
  float newScaleFactor = scaleFactor_;
  if (needsScale) {
    // Auto-calculate scale factor: available height / content height
    float calculatedScale =
        (maxContentHeight > 0) ? availableHeight / maxContentHeight : 1.0f;
    // Scale factor cannot be less than MIN_SCALE_FACTOR (0.85)
    newScaleFactor = (calculatedScale < MIN_SCALE_FACTOR) ? MIN_SCALE_FACTOR
                                                         : calculatedScale;
  } else if (needsScaleForSmallTop) {
    // Scale when top offset is too small (< 30)
    // Scale factor = (screenHeight - 30) / (screenHeight - topDistance_)
    float adjustedAvailableHeight = screenHeight - MIN_TOP_THRESHOLD;
    float adjustedContentHeight = screenHeight - topDist;
    float calculatedScale = (adjustedContentHeight > 0)
        ? adjustedAvailableHeight / adjustedContentHeight
        : 1.0f;
    calculatedScale =
        (calculatedScale < newScaleFactor) ? calculatedScale : newScaleFactor;
    // Scale factor cannot be less than MIN_SCALE_FACTOR (0.85)
    newScaleFactor = (calculatedScale < MIN_SCALE_FACTOR) ? MIN_SCALE_FACTOR
                                                         : calculatedScale;
    needsScale = true;
  } else {
    // No scaling needed, reset to 1.0
    newScaleFactor = 1.0f;
  }

  // Cache the calculated scale info and mark as scaled
  {
    std::lock_guard<std::mutex> lock(mutex_);
    scaleFactor_ = newScaleFactor;
    topDistance_ = topDist;
    if (modalTag >= 0) {
      ModalScaleInfo info;
      info.scaleFactor = scaleFactor_;
      info.topDistance = topDistance_;
      info.maxContentHeight = maxContentHeight;
      info.screenHeight = screenHeight;
      info.needsScale = needsScale;
      modalScaleCache_[modalTag] = info;
      if (needsScale) {
        scaledModalTags_.insert(modalTag);
      }
    }
  }

  return needsScale;
}

void GuideLayout::resetYogaTreeState(YGNodeRef nodeRef) {
  if (!nodeRef) {
    return;
  }

  nodeRef->setHasNewLayout(false);
  nodeRef->setDirty(true);

  size_t childCount = YGNodeGetChildCount(nodeRef);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(nodeRef, i);
    if (child) {
      resetYogaTreeState(child);
    }
  }
}

// Check if a Modal has already been scaled by looking up its tag in the
// scaledModalTags_ set. Used to decide whether original styles need to be
// restored before re-evaluating scaling.
bool GuideLayout::isModalAlreadyScaled(YGNodeRef modalNode) {
  if (!modalNode) {
    return false;
  }
  void* context = modalNode->getContext();
  if (!context) {
    return false;
  }
  auto* shadowNode = static_cast<ShadowNode*>(context);
  if (!shadowNode) {
    return false;
  }
  int32_t tag = shadowNode->getTag();
  std::lock_guard<std::mutex> lock(mutex_);
  return scaledModalTags_.find(tag) != scaledModalTags_.end();
}

// Restore original yoga styles for all children of the given Modal node.
// Called before re-evaluating scaling so that checkModalNeedsScaling reads
// accurate original layout results instead of previously scaled values.
void GuideLayout::restoreModalSubtreeStyles(YGNodeRef modalNode) {
  if (!modalNode) {
    return;
  }
  size_t childCount = YGNodeGetChildCount(modalNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(modalNode, i);
    if (child) {
      restoreOriginalStyles(child);
    }
  }
}

void GuideLayout::restoreOriginalStyles(YGNodeRef yogaNode) {
  if (!yogaNode) {
    return;
  }

  // Get the ShadowNode from context to access original props
  void* context = yogaNode->getContext();
  if (context) {
    auto* shadowNode = static_cast<ShadowNode*>(context);
    if (shadowNode) {
      auto props = shadowNode->getProps();
      if (props) {
        auto* yogaProps = dynamic_cast<const YogaStylableProps*>(props.get());
        if (yogaProps) {
          // Restore the original yogaStyle from props
          // This undoes any previous scaling that was applied to yogaNode_
          yogaNode->setStyle(yogaProps->yogaStyle);
        }
      }
    }
  }

  // Recursively restore children
  size_t childCount = YGNodeGetChildCount(yogaNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(yogaNode, i);
    if (child) {
      restoreOriginalStyles(child);
    }
  }
}

void GuideLayout::clearModalScalingState(YGNodeRef modalNode) {
  if (!modalNode) {
    return;
  }

  // Collect all Paragraph nodes in the Modal subtree so their content
  // caches get invalidated (needsContentRefreshTags_ is populated inside
  // collectModalSubtreeTags). After collection we move only the refresh
  // tags out, then clear the rest of the scaling bookkeeping.
  {
    std::lock_guard<std::mutex> lock(mutex_);
    modalSubtreeTags_.clear();
    needsContentRefreshTags_.clear();
  }

  // Re-collect subtree tags - this fills both modalSubtreeTags_ and
  // needsContentRefreshTags_ for every Paragraph under the Modal.
  size_t childCount = YGNodeGetChildCount(modalNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(modalNode, i);
    if (child) {
      collectModalSubtreeTags(child);
    }
  }

  // Now clear all scaling state but keep needsContentRefreshTags_ so that
  // ParagraphShadowNode::getContent() will invalidate its cached content
  // (which still holds the old scaled font sizes).
  {
    std::lock_guard<std::mutex> lock(mutex_);
    modalSubtreeTags_.clear();
    modalScaleCache_.clear();
    scaledModalTags_.clear();
    scaleFactor_ = 1.0f;
    topDistance_ = 0.0f;
  }
}

} // namespace react
} // namespace facebook