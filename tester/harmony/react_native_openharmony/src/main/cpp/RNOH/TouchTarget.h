/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/view/TouchEventEmitter.h>
#include <react/renderer/graphics/Point.h>
#include <react/renderer/graphics/Transform.h>

namespace rnoh {
class TouchTarget {
  using Point = facebook::react::Point;

 public:
  using Shared = std::shared_ptr<TouchTarget>;
  using Weak = std::weak_ptr<TouchTarget>;

  virtual Point computeChildPoint(
      Point const& point,
      TouchTarget::Shared const& child) const;
  /**
   * Provides the current offset of a scrollable touch target.
   */
  virtual Point getCurrentOffset() const {
    return {0, 0};
  }
  virtual bool containsPoint(Point const& point) const = 0;
  virtual bool containsPointInBoundingBox(Point const& point) = 0;
  virtual bool canHandleTouch() const = 0;
  virtual bool canChildrenHandleTouch() const = 0;
  // TODO: The function should have been designed as a pure virtual function.
  virtual bool canSubtreeHandleTouch(Point const& point) { return false; };
  /**
   If a component is designed to handle touch events (e.g., Scroll), it should
   return true if handling is currently in progress. This is done to prevent
   touches from being mistakenly interpreted as taps, particularly during
   scrolling activities.
   */
  virtual bool isHandlingTouches() const {
    return false;
  };
  virtual bool isJSResponder() const {
    return false;
  };
  virtual facebook::react::Tag getTouchTargetTag() const = 0;
  virtual facebook::react::SharedTouchEventEmitter getTouchEventEmitter()
      const = 0;
  virtual std::vector<Shared> getTouchTargetChildren() = 0;
  virtual facebook::react::LayoutMetrics getLayoutMetrics() const = 0;
  virtual facebook::react::Transform getTransform() const = 0;
  virtual TouchTarget::Shared getTouchTargetParent() const = 0;
  virtual facebook::react::Rect getBoundingBox() = 0;
  virtual void markBoundingBoxAsDirty() = 0;
  virtual bool isClippingSubviews() const = 0;
};
} // namespace rnoh