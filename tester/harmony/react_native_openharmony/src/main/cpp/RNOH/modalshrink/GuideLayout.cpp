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

  // Save original style before any modification (only save once per node)
  if (originalStyles_.find(yogaNode) == originalStyles_.end()) {
    originalStyles_[yogaNode] = yogaNode->getStyle();
  }

  auto style = yogaNode->getStyle();
  bool modified = false;

  // Check if current node has percentage width/height
  YGValue width = style.dimensions()[YGDimensionWidth];
  YGValue height = style.dimensions()[YGDimensionHeight];
  bool currentHasPercentWidth = (width.unit == YGUnitPercent && width.value > 0);
  bool currentHasPercentHeight = (height.unit == YGUnitPercent && height.value > 0);

  // Scale width - supports percentage
  if (width.unit == YGUnitPoint && width.value > 0) {
    style.dimensions()[YGDimensionWidth] =
        YGValue{width.value * scaleFactor_, YGUnitPoint};
    modified = true;
  } else if (currentHasPercentWidth && !parentHasPercentWidth) {
    // Scale percentage width only if parent doesn't have percentage width
    // If parent has percentage, child's percentage is relative to parent's scaled size
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
    // If parent has percentage, child's percentage is relative to parent's scaled size
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
  for (int edge = YGEdgeLeft; edge <= YGEdgeAll; ++edge) {
    YGValue padding = style.padding()[static_cast<YGEdge>(edge)];
    if (padding.unit == YGUnitPoint && padding.value > 0) {
      style.padding()[static_cast<YGEdge>(edge)] =
          YGValue{padding.value * scaleFactor_, YGUnitPoint};
      modified = true;
    }
  }

  if (modified) {
    yogaNode->setStyle(style);
    yogaNode->setDirty(true);
  }

  // Recursively process child nodes
  // Pass whether current node or any ancestor has percentage width/height
  bool childParentHasPercentWidth = parentHasPercentWidth || currentHasPercentWidth;
  bool childParentHasPercentHeight = parentHasPercentHeight || currentHasPercentHeight;

  size_t childCount = YGNodeGetChildCount(yogaNode);
  for (size_t i = 0; i < childCount; ++i) {
    YGNodeRef child = YGNodeGetChild(yogaNode, i);
    scaleYogaNodeStyleWithPercentHandling(child, childParentHasPercentWidth, childParentHasPercentHeight);
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

float GuideLayout::calculateTopDistance(YGNodeRef modalNode) {
  // Calculate top distance: L4.top + L5.top
  // Modal (L0) -> L1 -> L2 -> L3 -> L4 -> L5

  // Get L1: Modal's first child
  YGNodeRef L1 = YGNodeGetChild(modalNode, 0);
  if (!L1) {return 100.0f;}

  // Get L2: L1's first child
  YGNodeRef L2 = YGNodeGetChild(L1, 0);
  if (!L2) {return 100.0f;}

  // Get L3: L2's first child
  YGNodeRef L3 = YGNodeGetChild(L2, 0);
  if (!L3) {return 100.0f;}

  // Get L4: L3's first child
  YGNodeRef L4 = YGNodeGetChild(L3, 0);
  if (!L4) {return 100.0f;}
  float L4Top = YGNodeLayoutGetTop(L4);

  // Get L5: L4's first child
  YGNodeRef L5 = YGNodeGetChild(L4, 0);
  if (!L5) {return L4Top + 100.0f;}
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
  screenHeight_ = availableHeight;

  // Find Modal node
  YGNodeRef modalNode = findModalNode(rootYogaNode);
  if (!modalNode) {
    // No Modal, reset scale factor
    scaleFactor_ = 1.0f;
    return false;
  }

  // Get Modal's Tag for cache lookup
  void* context = modalNode->getContext();
  int32_t modalTag = -1;
  if (context) {
    auto* shadowNode = static_cast<ShadowNode*>(context);
    if (shadowNode) {
      modalTag = shadowNode->getTag();
    }
  }

  // Check if we have cached scale info for this Modal
  if (modalTag >= 0) {
    auto cacheIt = modalScaleCache_.find(modalTag);
    if (cacheIt != modalScaleCache_.end()) {
      // Use cached values
      scaleFactor_ = cacheIt->second.scaleFactor;
      topDistance_ = cacheIt->second.topDistance;
      return cacheIt->second.needsScale;
    }
  }

  // Recursively find maximum content height in Modal subtree (use original
  // screen height as reference)
  float maxContentHeight = findMaxContentHeight(modalNode, screenHeight);

  // Calculate top distance: L4.top + L5.top
  topDistance_ = calculateTopDistance(modalNode);

  bool needsScale = maxContentHeight > screenHeight + 0.5;

  // Additional scaling condition: if topDistance_ < 30
  constexpr float MIN_TOP_THRESHOLD = 30.0f;
  bool needsScaleForSmallTop = (topDistance_ < MIN_TOP_THRESHOLD);
  if (needsScale) {
    // Auto-calculate scale factor: available height / content height
    float calculatedScale =
        (maxContentHeight > 0) ? availableHeight / maxContentHeight : 1.0f;
    // Scale factor cannot be less than MIN_SCALE_FACTOR (0.85)
    scaleFactor_ = (calculatedScale < MIN_SCALE_FACTOR) ? MIN_SCALE_FACTOR
                                                        : calculatedScale;
  } else if (needsScaleForSmallTop) {
    // Scale when top offset is too small (< 30)
    // Scale factor = (screenHeight - 30) / (screenHeight - topDistance_)
    float adjustedAvailableHeight = screenHeight - MIN_TOP_THRESHOLD;
    float adjustedContentHeight = screenHeight - topDistance_;
    float calculatedScale = (adjustedContentHeight > 0)
        ? adjustedAvailableHeight / adjustedContentHeight : 1.0f;
    calculatedScale = (calculatedScale < scaleFactor_) ? calculatedScale
                                                        : scaleFactor_;
    // Scale factor cannot be less than MIN_SCALE_FACTOR (0.85)
    scaleFactor_ = (calculatedScale < MIN_SCALE_FACTOR) ? MIN_SCALE_FACTOR
                                                        : calculatedScale;
    needsScale = true;
  } else {
    // No scaling needed, reset to 1.0
    scaleFactor_ = 1.0f;
  }

  // Cache the calculated scale info for this Modal
  if (modalTag >= 0) {
    ModalScaleInfo info;
    info.scaleFactor = scaleFactor_;
    info.topDistance = topDistance_;
    info.needsScale = needsScale;
    modalScaleCache_[modalTag] = info;
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

void GuideLayout::restoreOriginalStyles() {
  for (const auto& pair : originalStyles_) {
    YGNodeRef node = pair.first;
    const YGStyle& style = pair.second;
    if (node) {
      node->setStyle(style);
      // No need to setDirty since layout calculation is already done
    }
  }
  originalStyles_.clear();
}

} // namespace react
} // namespace facebook