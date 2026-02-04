/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GUIDE_LAYOUT_H
#define GUIDE_LAYOUT_H

#include <unordered_map>
#include <unordered_set>
#include <yoga/YGNode.h>

// Forward declaration of YGNodeRef
typedef struct YGNode* YGNodeRef;

namespace facebook {
namespace react {

class GuideLayout {
 public:
  // Get singleton instance
  static GuideLayout& getInstance() {
    static GuideLayout instance; // Thread-safe singleton initialization
    return instance;
  }

  // Scaling function with percentage handling
  // parentHasPercentWidth: whether any ancestor node has percentage width
  // parentHasPercentHeight: whether any ancestor node has percentage height
  // If parent has percentage, child's percentage won't be scaled (it inherits from parent's scaling)
  void scaleYogaNodeStyleWithPercentHandling(
      YGNodeRef yogaNode,
      bool parentHasPercentWidth,
      bool parentHasPercentHeight);

  // Scan Yoga tree, find Modal nodes and scale their subtrees
  // Returns whether Modal was found and processed
  bool scanAndScaleModalSubtrees(YGNodeRef rootYogaNode);

  // Find Modal node
  YGNodeRef findModalNode(YGNodeRef rootYogaNode);

  // Check if Modal needs scaling (based on post-layout height)
  // If scaling is needed, automatically calculates and saves scale factor
  // Returns: true indicates scaling is needed
  bool checkModalNeedsScaling(YGNodeRef rootYogaNode, float screenHeight);

  // Recursively find maximum content height in Modal subtree
  float findMaxContentHeight(YGNodeRef node, float screenHeight);

  // Reset Yoga tree layout state (used for recalculation)
  void resetYogaTreeState(YGNodeRef node);

  // Restore original styles after layout calculation
  void restoreOriginalStyles();

  // Set Modal content scaling feature toggle
  void setModalContentShrinkEnabled(bool enabled) {
    enableModalContentShrink_ = enabled;
  }

  // Check if Modal scaling is enabled
  bool isModalContentShrinkEnabled() const {
    return enableModalContentShrink_;
  }

  // Get scale factor (automatically calculated based on content overflow)
  float getScaleFactor() const {
    return scaleFactor_;
  }

  // Get cached screen height
  float getScreenHeight() const {
    return screenHeight_;
  }

  // Get top distance (sum of L4.top and L5.top in Modal subtree)
  float getTopDistance() const {
    return topDistance_;
  }

  // Check if a node is in Modal subtree (used for font scaling)
  bool isInModalSubtree(int32_t tag) const {
    return modalSubtreeTags_.find(tag) != modalSubtreeTags_.end();
  }

  // Check if a node needs content cache refresh (used for Paragraph font
  // scaling)
  bool needsContentRefresh(int32_t tag) const {
    return needsContentRefreshTags_.find(tag) != needsContentRefreshTags_.end();
  }

  // Mark that a node's content cache has been refreshed
  void markContentRefreshed(int32_t tag) {
    needsContentRefreshTags_.erase(tag);
  }

  // Clear all records when Modal no longer exists
  void clearAllModalState() {
    modalSubtreeTags_.clear();
    needsContentRefreshTags_.clear();
    originalStyles_.clear();
    modalScaleCache_.clear();
    topDistance_ = 0.0f;
  }

  // Reset Modal subtree records (called before rescaling)
  void resetModalSubtreeTags() {
    modalSubtreeTags_.clear();
    needsContentRefreshTags_.clear();
  }

  // Delete copy constructor and assignment operator
  GuideLayout(const GuideLayout&) = delete;
  GuideLayout& operator=(const GuideLayout&) = delete;

 private:
  // Minimum scale factor limit
  static constexpr float MIN_SCALE_FACTOR = 0.85f;

  // Auto-calculated scale factor (default 1.0 means no scaling)
  float scaleFactor_ = 1.0f;

  // Cached screen height
  float screenHeight_ = 0.0f;

  // Top distance: sum of L4.top and L5.top in Modal subtree
  // Modal -> L1 -> L2 -> L3 -> L4 -> L5
  float topDistance_ = 0.0f;

  bool enableModalContentShrink_ = true;

  // Track all node Tags in Modal subtree (used for font scaling judgment)
  std::unordered_set<int32_t> modalSubtreeTags_;

  // Track Paragraph nodes that need content cache refresh
  std::unordered_set<int32_t> needsContentRefreshTags_;

  // Store original styles for restoration after layout calculation
  std::unordered_map<YGNodeRef, YGStyle> originalStyles_;

  // Cached scale info for each Modal (key: Modal Tag)
  struct ModalScaleInfo {
    float scaleFactor = 1.0f;
    float topDistance = 0.0f;
    bool needsScale = false;
  };
  std::unordered_map<int32_t, ModalScaleInfo> modalScaleCache_;

  // Collect all node Tags in Modal subtree
  void collectModalSubtreeTags(YGNodeRef yogaNode);

  // Calculate top distance: L4.top + L5.top
  // Modal -> L1 -> L2 -> L3 -> L4 -> L5
  float calculateTopDistance(YGNodeRef modalNode);

  // Private constructor
  GuideLayout() = default;
};

} // namespace react
} // namespace facebook

#endif // GUIDE_LAYOUT_H