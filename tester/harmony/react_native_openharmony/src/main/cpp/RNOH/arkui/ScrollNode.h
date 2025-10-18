/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/graphics/Point.h>
#include "ArkUINode.h"
#include "NativeNodeApi.h"

namespace rnoh {
class ScrollNodeDelegate {
 public:
  virtual ~ScrollNodeDelegate() noexcept(false) = default;
  virtual void onScroll(){};
  virtual void onScrollStart(){};
  virtual void onScrollStop(){};
  virtual float onScrollFrameBegin(float offset, int32_t scrollState) {
    return offset;
  };
  virtual void onAppear(){};
};

class ScrollNode : public ArkUINode {
 protected:
  using Point = facebook::react::Point;

  ArkUI_NodeHandle m_childArkUINodeHandle;
  ScrollNodeDelegate* m_scrollNodeDelegate;

 public:
  explicit ScrollNode(const ArkUINode::Context::Shared& context = nullptr);
  ~ScrollNode() override;

  void insertChild(ArkUINode& child);
  void removeChild(ArkUINode& child);
  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;

  Point getScrollOffset() const;
  void setScrollNodeDelegate(ScrollNodeDelegate* scrollNodeDelegate);
  ScrollNode& setHorizontal(bool horizontal);
  ScrollNode& setEnableScrollInteraction(bool enableScrollInteraction);
  ScrollNode& setFriction(float friction);
  ScrollNode& setEdgeEffect(bool bounces, bool alwaysBounces);
  ScrollNode& setScrollBarDisplayMode(
      ArkUI_ScrollBarDisplayMode scrollBarDisplayMode);
  ScrollNode& setScrollBarColor(uint32_t scrollBarColor);
  ScrollNode& setScrollSnap(
      ArkUI_ScrollSnapAlign scrollSnapAlign,
      bool snapToStart,
      bool snapToEnd,
      const std::vector<facebook::react::Float>& snapPoints);
  ScrollNode& setEnablePaging(bool enablePaging);
  ScrollNode& setCenterContent(bool centerContent);
  ScrollNode& setFadingEdge(float length);
  ScrollNode& resetScrollSnap();
  void scrollTo(float x, float y, bool animated, bool scrollToOverflowEnabled = false);
  void setNestedScroll(ArkUI_ScrollNestedMode scrollNestedMode);
  void setNestedScroll(ArkUI_ScrollNestedMode scrollForward, ArkUI_ScrollNestedMode scrollBackward);
  void setScrollOverScrollMode(std::string const& overScrollMode, bool isContentSmallerThanContainer = false);
  void setNestedScrollEnabled(bool nestedScrollEnabled);
  void setEndFillColor(uint32_t color);
};

} // namespace rnoh