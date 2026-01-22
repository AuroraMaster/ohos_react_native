/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GUIDE_LAYOUT_H
#define GUIDE_LAYOUT_H

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
  // hasScaledPercent: tracks whether the first node with percentage has been
  // scaled
  void scaleYogaNodeStyleWithPercentHandling(
      YGNodeRef yogaNode,
      bool& hasScaledPercent);

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

  // Mark the beginning of a new layout cycle
  void beginLayoutCycle() {
    // Only clear scaling records for each new layout cycle, keep
    // modalSubtreeTags_ to maintain font scaling state
    scaledModalTags_.clear();
    needsContentRefreshTags_.clear();
  }

  // Clear all records when Modal no longer exists
  void clearAllModalState() {
    scaledModalTags_.clear();
    modalSubtreeTags_.clear();
    needsContentRefreshTags_.clear();
  }

  // Reset Modal subtree records (called before rescaling)
  void resetModalSubtreeTags() {
    modalSubtreeTags_.clear();
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

  bool enableModalContentShrink_ = true;

  // Track Modal nodes that have been scaled (using ShadowNode's tag)
  std::unordered_set<int32_t> scaledModalTags_;

  // Track all node Tags in Modal subtree (used for font scaling judgment)
  std::unordered_set<int32_t> modalSubtreeTags_;

  // Track Paragraph nodes that need content cache refresh
  std::unordered_set<int32_t> needsContentRefreshTags_;

  // Collect all node Tags in Modal subtree
  void collectModalSubtreeTags(YGNodeRef yogaNode);

  // Private constructor
  GuideLayout() = default;
};

} // namespace react
} // namespace facebook

#endif // GUIDE_LAYOUT_H