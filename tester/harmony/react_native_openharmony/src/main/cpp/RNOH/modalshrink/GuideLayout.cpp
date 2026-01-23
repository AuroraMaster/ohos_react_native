/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "GuideLayout.h"
#include <react/renderer/core/ShadowNode.h>
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
    // Get Modal's tag
    int32_t modalTag = shadowNode->getTag();

    // Check if this Modal has already been scaled
    if (scaledModalTags_.find(modalTag) != scaledModalTags_.end()) {
      return true; // Already processed, don't scale again
    }

    // Found Modal node, scale all its children
    // Use scaling function with percentage handling
    bool hasScaledPercent = false;
    size_t childCount = YGNodeGetChildCount(rootYogaNode);
    for (size_t i = 0; i < childCount; ++i) {
      YGNodeRef child = YGNodeGetChild(rootYogaNode, i);
      scaleYogaNodeStyleWithPercentHandling(child, hasScaledPercent);
      // Collect all node Tags in subtree for font scaling judgment
      collectModalSubtreeTags(child);
    }

    // Mark this Modal as already scaled
    scaledModalTags_.insert(modalTag);

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
    bool& hasScaledPercent) {
  if (!yogaNode) {
    return;
  }

  auto style = yogaNode->getStyle();
  bool modified = false;

  // Scale width - supports percentage
  YGValue width = style.dimensions()[YGDimensionWidth];
  if (width.unit == YGUnitPoint && width.value > 0) {
    style.dimensions()[YGDimensionWidth] =
        YGValue{width.value * scaleFactor_, YGUnitPoint};
    modified = true;
  } else if (
      width.unit == YGUnitPercent && width.value > 0 && !hasScaledPercent) {
    // Only scale the first node with percentage width encountered
    style.dimensions()[YGDimensionWidth] =
        YGValue{width.value * scaleFactor_, YGUnitPercent};
    hasScaledPercent = true;
    modified = true;
  }

  // Scale height - supports percentage - supports percentage
  YGValue height = style.dimensions()[YGDimensionHeight];
  if (height.unit == YGUnitPoint && height.value > 0) {
    style.dimensions()[YGDimensionHeight] =
        YGValue{height.value * scaleFactor_, YGUnitPoint};
    modified = true;
  } else if (
      height.unit == YGUnitPercent && height.value > 0 && !hasScaledPercent) {
    // Only scale the first node with percentage height encountered
    style.dimensions()[YGDimensionHeight] =
        YGValue{height.value * scaleFactor_, YGUnitPercent};
    hasScaledPercent = true;
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
  for (int edge = YGEdgeLeft; edge <= YGEdgeAll; ++edge) {
    YGValue padding = style.padding()[static_cast<YGEdge>(edge)];
    if (padding.unit == YGUnitPoint && padding.value > 0) {
      style.padding()[static_cast<YGEdge>(edge)] =
          YGValue{padding.value * scaleFactor_, YGUnitPoint};
      modified = true;
    }
  }

  // Scale position (only process absolute values)
  for (int edge = YGEdgeLeft; edge <= YGEdgeAll; ++edge) {
    YGValue position = style.position()[static_cast<YGEdge>(edge)];
    if (position.unit == YGUnitPoint && position.value != 0) {
      style.position()[static_cast<YGEdge>(edge)] =
          YGValue{position.value * scaleFactor_, YGUnitPoint};
      modified = true;
    }
  }

  // Scale gap (only process absolute values)
  for (int gutter = YGGutterColumn; gutter <= YGGutterAll; ++gutter) {
    YGValue gap = style.gap()[static_cast<YGGutter>(gutter)];
    if (gap.unit == YGUnitPoint && gap.value > 0) {
      style.gap()[static_cast<YGGutter>(gutter)] =
          YGValue{gap.value * scaleFactor_, YGUnitPoint};
      modified = true;
    }
  }

  // Scale flexBasis (only process absolute values)
  auto flexBasisRef = style.flexBasis();
  YGValue flexBasis = static_cast<YGValue>(
      static_cast<facebook::yoga::detail::CompactValue>(flexBasisRef));
  if (flexBasis.unit == YGUnitPoint && flexBasis.value > 0) {
    style.flexBasis() = YGValue{flexBasis.value * scaleFactor_, YGUnitPoint};
    modified = true;
  }

  if (modified) {
    yogaNode->setStyle(style);
    yogaNode->setDirty(true);
  }

  // Recursively process child nodes, pass the same percentage flag
  size_t childCount = YGNodeGetChildCount(yogaNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(yogaNode, i);
    scaleYogaNodeStyleWithPercentHandling(child, hasScaledPercent);
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
      modalSubtreeTags_.insert(tag);

      // If it's a Paragraph node, mark it as needing content cache refresh
      const char* componentName = shadowNode->getComponentName();
      if (componentName && std::strcmp(componentName, "Paragraph") == 0) {
        needsContentRefreshTags_.insert(tag);
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

  float nodeHeight = YGNodeLayoutGetHeight(node);
  float maxHeight = 0;

  // Check if current node's style height is fixed (not flex-stretched)
  auto style = node->getStyle();
  YGValue styleHeight = style.dimensions()[YGDimensionHeight];

  // If node has fixed height style (Point or Percent), and layout height
  // exceeds screen This indicates it's an actual content container
  bool hasFixedHeight =
      (styleHeight.unit == YGUnitPoint && styleHeight.value > 0) ||
      (styleHeight.unit == YGUnitPercent && styleHeight.value > 0);

  if (hasFixedHeight && nodeHeight > 0) {
    // This is a node with fixed height content
    maxHeight = nodeHeight;
  }

  // Recursively check child nodes
  size_t childCount = YGNodeGetChildCount(node);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(node, i);
    float childMaxHeight = findMaxContentHeight(child, screenHeight);
    if (childMaxHeight > maxHeight) {
      maxHeight = childMaxHeight;
    }
  }

  // If no fixed-height child nodes found, check total height stretched by
  // children
  if (maxHeight == 0 && childCount > 0) {
    float contentBottom = 0;
    for (size_t i = 0; i < childCount; ++i) {
      YGNodeRef child = YGNodeGetChild(node, i);
      float childHeight = YGNodeLayoutGetHeight(child);
      float childTop = YGNodeLayoutGetTop(child);
      float childBottom = childTop + childHeight;
      if (childBottom > contentBottom) {
        contentBottom = childBottom;
      }
    }
    // Only consider when content stretched by children exceeds screen
    if (contentBottom > screenHeight) {
      maxHeight = contentBottom;
    }
  }

  return maxHeight;
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

bool GuideLayout::checkModalNeedsScaling(
    YGNodeRef rootYogaNode,
    float screenHeight) {
  if (!rootYogaNode || !enableModalContentShrink_ || screenHeight <= 0) {
    return false;
  }

  // Subtract estimated height of status bar and navigation bar for scale
  // calculation
  constexpr float SYSTEM_BARS_HEIGHT = 80.0f;
  float availableHeight = screenHeight - SYSTEM_BARS_HEIGHT;
  if (availableHeight <= 0) {
    availableHeight = screenHeight;
  }

  // Cache available height
  screenHeight_ = availableHeight;

  // Find Modal node
  YGNodeRef modalNode = findModalNode(rootYogaNode);
  if (!modalNode) {
    // No Modal, reset scale factor
    scaleFactor_ = 1.0f;
    return false;
  }

  // Recursively find maximum content height in Modal subtree (use original
  // screen height as reference)
  float maxContentHeight = findMaxContentHeight(modalNode, screenHeight);

  // Determine if scaling is needed: content height exceeds original screen
  // height
  bool needsScale = maxContentHeight > screenHeight;

  if (needsScale) {
    // Auto-calculate scale factor: available height / content height
    float calculatedScale =
        (maxContentHeight > 0) ? availableHeight / maxContentHeight : 1.0f;

    // Scale factor cannot be less than MIN_SCALE_FACTOR (0.85)
    scaleFactor_ = (calculatedScale < MIN_SCALE_FACTOR) ? MIN_SCALE_FACTOR
                                                        : calculatedScale;
  } else {
    // No scaling needed, reset to 1.0
    scaleFactor_ = 1.0f;
  }

  return needsScale;
}

void GuideLayout::resetYogaTreeState(YGNodeRef node) {
  if (!node) {
    return;
  }

  // Reset layout flag
  node->setHasNewLayout(false);
  node->setDirty(true);

  // Recursively process child nodes
  size_t childCount = YGNodeGetChildCount(node);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(node, i);
    if (child) {
      resetYogaTreeState(child);
    }
  }
}

} // namespace react
} // namespace facebook
