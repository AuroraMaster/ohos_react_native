/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <arkui/native_type.h>
#include "react/renderer/components/scrollview/primitives.h"
#include "react/renderer/components/view/conversions.h"
#include "react/renderer/graphics/RectangleEdges.h"

namespace rnoh {
inline ArkUI_ScrollSnapAlign getArkUI_ScrollSnapAlign(
    facebook::react::ScrollViewSnapToAlignment snapToAlignment) {
  if (snapToAlignment == facebook::react::ScrollViewSnapToAlignment::Start) {
    return ArkUI_ScrollSnapAlign::ARKUI_SCROLL_SNAP_ALIGN_START;
  } else if (
      snapToAlignment == facebook::react::ScrollViewSnapToAlignment::Center) {
    return ArkUI_ScrollSnapAlign::ARKUI_SCROLL_SNAP_ALIGN_CENTER;
  } else if (
      snapToAlignment == facebook::react::ScrollViewSnapToAlignment::End) {
    return ArkUI_ScrollSnapAlign::ARKUI_SCROLL_SNAP_ALIGN_END;
  } else {
    return ArkUI_ScrollSnapAlign::ARKUI_SCROLL_SNAP_ALIGN_NONE;
  }
}

inline facebook::react::RectangleEdges<facebook::react::Float> resolveEdges(
    YGStyle::Edges const& yogaEdges,
    bool isRTL = false) {
  auto left =
      facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeLeft]);
  auto top = facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeTop]);
  auto right =
      facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeRight]);
  auto bottom =
      facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeBottom]);

  auto start =
      facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeStart]);
  auto end = facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeEnd]);

  auto horizontal =
      facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeHorizontal]);
  auto vertical =
      facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeVertical]);

  auto all = facebook::react::optionalFloatFromYogaValue(yogaEdges[YGEdgeAll]);

  facebook::react::Float leftEdge = 0.0;
  facebook::react::Float topEdge = 0.0;
  facebook::react::Float bottomEdge = 0.0;
  facebook::react::Float rightEdge = 0.0;

  if (all.has_value()) {
    leftEdge = topEdge = bottomEdge = rightEdge = all.value();
  }

  if (horizontal.has_value()) {
    leftEdge = rightEdge = horizontal.value();
  }

  if (vertical.has_value()) {
    topEdge = bottomEdge = vertical.value();
  }

  if (isRTL) {
    leftEdge = end.value_or(leftEdge);
    rightEdge = start.value_or(rightEdge);
  } else {
    leftEdge = start.value_or(leftEdge);
    rightEdge = end.value_or(rightEdge);
  }

  leftEdge = left.value_or(leftEdge);
  topEdge = top.value_or(topEdge);
  rightEdge = right.value_or(rightEdge);
  bottomEdge = bottom.value_or(bottomEdge);

  return {leftEdge, topEdge, rightEdge, bottomEdge};
}

inline facebook::react::Point getEndPoint(const facebook::react::Rect& rect) {
  return facebook::react::Point{
      rect.origin.x + rect.size.width, rect.origin.y + rect.size.height};
}

inline bool rectIntersects(
    const facebook::react::Rect& rect1,
    const facebook::react::Rect& rect2) {
  return !(
      ((rect2.origin.x < rect1.origin.x &&
        getEndPoint(rect2).x < rect1.origin.x) ||
       (rect2.origin.x > getEndPoint(rect1).x &&
        getEndPoint(rect2).x > getEndPoint(rect1).x)) ||
      ((rect2.origin.y < rect1.origin.y &&
        getEndPoint(rect2).y < rect1.origin.y) ||
       (rect2.origin.y > getEndPoint(rect1).y &&
        getEndPoint(rect2).y > getEndPoint(rect1).y)));
}

} // namespace rnoh