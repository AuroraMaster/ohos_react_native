/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * Used only in C-API based Architecture.
 */
#pragma once
#include <arkui/native_type.h>
#include <RNOH/arkui/NativeNodeApi.h>
#include <react/renderer/components/view/TouchEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/EventEmitter.h>
#include <react/renderer/core/LayoutMetrics.h>
#include <react/renderer/core/Props.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <react/renderer/core/State.h>
#include <react/renderer/components/view/primitives.h>
#include <react/renderer/components/view/ViewProps.h>
#include <vector>
#include <map>
#include "RNOH/ApiVersionCheck.h"
#include "RNOH/ComponentInstance.h"

namespace rnoh {
  using BorderCurve = facebook::react::BorderCurve;
  using BorderStyle = facebook::react::BorderStyle;
  using BorderMetrics = facebook::react::BorderMetrics; 
  using SharedColor = facebook::react::SharedColor;
  using Float = facebook::react::Float;
  using Size = facebook::react::Size;
  static BorderMetrics ARKUI_DEFAULT_BORDER_METRICS = {
    // BorderColors
    {{}, {}, {}, {}},
    // BorderWidths
    {{0.0}, {0.0}, {0.0}, {0.0}},
    // BorderRadii
    {{0.0}, {0.0}, {0.0}, {0.0}},
    // BorderCurves
    {BorderCurve::Circular, BorderCurve::Circular, BorderCurve::Circular, BorderCurve::Circular},
    // BorderStyles
    {BorderStyle::Solid, BorderStyle::Solid, BorderStyle::Solid, BorderStyle::Solid}
  };

  static SharedColor ARKUI_DEFAULT_BACKGROUND_COLOR = {};
  static SharedColor ARKUI_DEFAULT_SHADOW_COLOR = {};
  static Size ARKUI_DEFAULT_SHADOW_OFFSET = {0, -3};
  static Float ARKUI_DEFAULT_SHADOW_OPACITY = {};
  static Float ARKUI_DEFAULT_SHADOW_RADIUS = {3};

inline facebook::react::Rect transformRectAroundPoint(
    const facebook::react::Rect& rect,
    const facebook::react::Point& point,
    const facebook::react::Transform& transform);

/**
 * @api
 */
template <typename ShadowNodeT>
class CppComponentInstance : public ComponentInstance,
                             public ArkUINodeDelegate {
  static_assert(
      std::is_base_of_v<facebook::react::ShadowNode, ShadowNodeT>,
      "ShadowNodeT must be a subclass of facebook::react::ShadowNode");

 protected:
  using ConcreteProps = typename ShadowNodeT::ConcreteProps;
  using ConcreteState = typename ShadowNodeT::ConcreteState;
  using ConcreteEventEmitter = typename ShadowNodeT::ConcreteEventEmitter;
  using SharedConcreteProps = std::shared_ptr<const ConcreteProps>;
  using SharedConcreteState = std::shared_ptr<const ConcreteState>;
  using SharedConcreteEventEmitter =
      std::shared_ptr<const ConcreteEventEmitter>;

 public:
  using Shared = std::shared_ptr<CppComponentInstance>;
  const std::unordered_set<std::string> SVG_TYPE_ARR = {"RNSVGCircle", "RNSVGClipPath", "RNSVGDefs", "RNSVGEllipse", "RNSVGForeignObject", "RNSVGGroup", "RNSVGSvgView", "RNSVGImage", "RNSVGLine", "RNSVGLinearGradient", "RNSVGRadialGradient", "RNSVGMarker", "RNSVGMask", "RNSVGPath", "RNSVGPattern", "RNSVGRadialGradient", "RNSVGRect", "RNSVGSymbol", "RNSVGTSpan", "RNSVGText", "RNSVGTextPath", "RNSVGUse"};

  CppComponentInstance(Context context)
      : ComponentInstance(std::move(context)) {}

  void onCreate() override {
    std::string componentName = this->getComponentName();
    if (SVG_TYPE_ARR.find(componentName) != SVG_TYPE_ARR.end()) {
        return;
    }
    this->getLocalRootArkUINode().setArkUINodeDelegate(this);
  }

  facebook::react::Tag getTag() const {
    return m_tag;
  }

  facebook::react::Props::Shared getProps() const override {
    return m_props;
  }

  SharedConcreteState const& getState() const {
    return m_state;
  }

  SharedConcreteEventEmitter const& getEventEmitter() const {
    return m_eventEmitter;
  }

  const std::string& getAccessibilityLabel() const override {
    return m_accessibilityLabel;
  }
  
  bool getAccessibilityGroup() const override {
    return m_accessibilityGroup;
  }

  /**
   * TODO: change to private — those methods are intended to be called
   * only by MountingManagerCAPI which is a friend of this class
   * (latestRNOHVersion: 0.72.27)
   */
 public:
  void setProps(facebook::react::Props::Shared props) final {
    auto newProps = std::dynamic_pointer_cast<const ConcreteProps>(props);
    if (!newProps) {
      return;
    }
    this->onPropsChanged(newProps);
    m_props = newProps;
  }

  void setState(facebook::react::State::Shared state) final {
    auto newState = std::dynamic_pointer_cast<const ConcreteState>(state);
    if (!newState) {
      return;
    }

    this->onStateChanged(newState);
    m_state = newState;
  }

  void setEventEmitter(facebook::react::SharedEventEmitter eventEmitter) final {
    auto newEventEmitter =
        std::dynamic_pointer_cast<const ConcreteEventEmitter>(eventEmitter);
    if (!newEventEmitter) {
      return;
    }
    this->onEventEmitterChanged(newEventEmitter);
    m_eventEmitter = newEventEmitter;
  }

  void setLayout(facebook::react::LayoutMetrics layoutMetrics) override {
    this->onLayoutChanged(layoutMetrics);
//     bool changeFlag =  (layoutMetrics != m_layoutMetrics);
    m_layoutMetrics = layoutMetrics;
//     if (changeFlag) {
//       auto parent =  getParent().lock();
//       if (parent != nullptr) {
//         parent->onChildLayoutChange(shared_from_this());
//       }
//     }
  }

 public:
  // TouchTarget implementation
  facebook::react::LayoutMetrics getLayoutMetrics() const override {
    return m_layoutMetrics;
  }

  bool containsPoint(facebook::react::Point const& point) const override {
    auto hitRect = getHitRect();
    return hitRect.containsPoint(point);
  }

  bool containsPointInBoundingBox(
      facebook::react::Point const& point) override {
    auto boundingBox = this->getBoundingBox();
    return boundingBox.containsPoint(point);
  }

  bool canHandleTouch() const override {
    auto parent = m_parent.lock();
    if (parent && !parent->canChildrenHandleTouch()) {
      return false;
    }
    if (m_props != nullptr) {
      auto props =
          std::dynamic_pointer_cast<const facebook::react::ViewProps>(m_props);
      return props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
          props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly;
    }
    return true;
  };

  bool canChildrenHandleTouch() const override {
    auto parent = m_parent.lock();
    if (parent && !parent->canChildrenHandleTouch()) {
      return false;
    }
    if (m_props != nullptr) {
      auto props =
          std::dynamic_pointer_cast<const facebook::react::ViewProps>(m_props);
      return props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
          props->pointerEvents == facebook::react::PointerEventsMode::BoxNone;
    }
    return true;
  };

  bool canSubtreeHandleTouch(facebook::react::Point const& point) override {
    auto children = getTouchTargetChildren();
    for (auto const& child : children) {
      if (child == nullptr) {
        continue;
      }

      auto childPoint = computeChildPoint(point, child);
      if (child->canHandleTouch() && child->containsPoint(childPoint) &&
          child->getTouchEventEmitter()) {
        return true;
      }

      if (child->canChildrenHandleTouch() &&
          child->containsPointInBoundingBox(childPoint)) {
        if (child->canSubtreeHandleTouch(childPoint)) {
          return true;
        }
      }
    }
    return false;
  };

  facebook::react::Tag getTouchTargetTag() const override {
    return getTag();
  }

  TouchTarget::Shared getTouchTargetParent() const override {
    return getParent().lock();
  }

  facebook::react::SharedTouchEventEmitter getTouchEventEmitter()
      const override {
    return m_eventEmitter;
  }

  std::vector<TouchTarget::Shared> getTouchTargetChildren() override {
    auto children = getChildren();
    return std::vector<TouchTarget::Shared>(children.begin(), children.end());
  }

  facebook::react::Transform getTransform() const override {
    return m_transform;    
  }

  facebook::react::Rect getBoundingBox() override {
    if (!m_boundingBox.has_value()) {
      calculateBoundingBox();
    }
    return m_boundingBox.value();
  };

  bool isClippingSubviews() const override {
    return m_isClipping;
  }

  void markBoundingBoxAsDirty() override {
    if (m_boundingBox.has_value()) {
      m_boundingBox.reset();
      auto parent = getTouchTargetParent();
      while (parent != nullptr && !parent->isClippingSubviews()) {
        parent->markBoundingBoxAsDirty();
        parent = parent->getTouchTargetParent();
      }
    }
  };

 protected:
  virtual void onLayoutChanged(
      facebook::react::LayoutMetrics const& layoutMetrics) {
    this->getLocalRootArkUINode().setLayoutRect(
      layoutMetrics.frame.origin, layoutMetrics.frame.size, layoutMetrics.pointScaleFactor);
    if (layoutMetrics.layoutDirection != m_layoutMetrics.layoutDirection) {
      ArkUI_Direction direction =
          convertLayoutDirection(layoutMetrics.layoutDirection);
      this->getLocalRootArkUINode().setDirection(direction);
    }
    markBoundingBoxAsDirty();
  }

  virtual void onPropsChanged(SharedConcreteProps const& concreteProps) {
    auto props = std::static_pointer_cast<const facebook::react::ViewProps>(
        concreteProps);
    auto old =
        std::static_pointer_cast<const facebook::react::ViewProps>(m_props);
    auto isTransformManagedByAnimated =
        getIgnoredPropKeys().count("transform") > 0;
    facebook::react::Transform defaultTransform;
        
    if (old && *(props->backgroundColor) != *(old->backgroundColor)) {
      this->getLocalRootArkUINode().setBackgroundColor(
            props->backgroundColor);
    } else if (!old && props->backgroundColor != ARKUI_DEFAULT_BACKGROUND_COLOR) {
      this->getLocalRootArkUINode().setBackgroundColor(
            props->backgroundColor);
    } else {
      // Do nothing here.
    }
    m_accessibilityLabel = props->accessibilityLabel;
    if (old) {
      if (props->accessibilityState.disabled !=
              old->accessibilityState.disabled ||
          props->accessibilityState.checked !=
              old->accessibilityState.checked ||
          props->accessibilityState.selected !=
              old->accessibilityState.selected) {
        this->getLocalRootArkUINode().setAccessibilityState(
            props->accessibilityState);
      }
    } else {
        this->getLocalRootArkUINode().setAccessibilityState(
            props->accessibilityState);
    }

    facebook::react::BorderMetrics borderMetrics =
      props->resolveBorderMetrics(this->m_layoutMetrics);
    
    if (old && borderMetrics.borderWidths != m_oldBorderMetrics.borderWidths) {
      this->getLocalRootArkUINode().setBorderWidth(
        borderMetrics.borderWidths);
    } else if (!old && borderMetrics.borderWidths != ARKUI_DEFAULT_BORDER_METRICS.borderWidths) {
      this->getLocalRootArkUINode().setBorderWidth(
          borderMetrics.borderWidths);
    } else {
      // Do nothing here.
    }
    
    if (old && borderMetrics.borderColors != m_oldBorderMetrics.borderColors) {
      this->getLocalRootArkUINode().setBorderColor(
        borderMetrics.borderColors);
    } else if (!old && borderMetrics.borderColors != ARKUI_DEFAULT_BORDER_METRICS.borderColors) {
      this->getLocalRootArkUINode().setBorderColor(
        borderMetrics.borderColors);
    } else {
      // Do nothing here.
    }
        
    if (!old || borderMetrics.borderRadii != m_oldBorderMetrics.borderRadii ||
        !m_isRadiusSetValid) {
      if (this->m_layoutMetrics.frame.size != facebook::react::Size{0, 0}) {
        m_isRadiusSetValid = true;
      }
      this->getLocalRootArkUINode().setBorderRadius(borderMetrics.borderRadii);
    }
    
    if (old && borderMetrics.borderStyles != m_oldBorderMetrics.borderStyles) {
      this->getLocalRootArkUINode().setBorderStyle(
        borderMetrics.borderStyles);
    } else if (!old && borderMetrics.borderStyles != ARKUI_DEFAULT_BORDER_METRICS.borderStyles) {
      this->getLocalRootArkUINode().setBorderStyle(
        borderMetrics.borderStyles);
    } else {
      // Do nothing here.
    }

    if (!old) {
      if (std::abs(static_cast<float>(props->shadowRadius) - 3.0f) < 1e-3 && 
          std::abs(static_cast<float>(props->shadowOffset.width) - 0.0f) < 1e-3 && 
          std::abs(static_cast<float>(props->shadowOffset.height) + 3.0f) < 1e-3 &&
          static_cast<uint32_t>(props->shadowColor) == 0) {
        // Do nothing here.
      } else {
        this->getLocalRootArkUINode().setShadow(
          props->shadowColor,
          props->shadowOffset,
          props->shadowOpacity,
          props->shadowRadius,
          m_layoutMetrics.pointScaleFactor);
      }
    } else if (props->shadowColor != old->shadowColor &&
              props->shadowOffset != old->shadowOffset &&
              props->shadowOpacity != old->shadowOpacity &&
              props->shadowRadius != old->shadowRadius) {
        this->getLocalRootArkUINode().setShadow(
          props->shadowColor,
          props->shadowOffset,
          props->shadowOpacity,
          props->shadowRadius,
          m_layoutMetrics.pointScaleFactor); 
    } else {
        // Do nothing here.   
    }

    if (old) {
      if (props->accessibilityActions != old->accessibilityActions) {
        this->getLocalRootArkUINode().setAccessibilityActions(props->accessibilityActions);
      }
    } else {
      this->getLocalRootArkUINode().setAccessibilityActions(props->accessibilityActions);
    }
        
    if (!isTransformManagedByAnimated) {
      if (!old) {
        if (props->transform != defaultTransform || abs(m_oldPointScaleFactor - 0.0f) > 0.001f) {
          m_oldPointScaleFactor = m_layoutMetrics.pointScaleFactor;
          this->setTransform(props->transform);
          this->getLocalRootArkUINode().setTransform(
            props->transform, m_layoutMetrics.pointScaleFactor);
          markBoundingBoxAsDirty();
        }
      } else if (props->transform != m_transform ||
         abs(m_oldPointScaleFactor - m_layoutMetrics.pointScaleFactor) >
             0.001f) {
        m_oldPointScaleFactor = m_layoutMetrics.pointScaleFactor;
        this->setTransform(props->transform);
        this->getLocalRootArkUINode().setTransform(
          props->transform, m_layoutMetrics.pointScaleFactor);
        markBoundingBoxAsDirty();
      }
    }

    if (!old) {
      if (!props->accessibilityRole.empty()) {
        this->getLocalRootArkUINode().setAccessibilityRole(
          props->accessibilityRole);
      }
    } else if (props->accessibilityRole != old->accessibilityHint) {
      this->getLocalRootArkUINode().setAccessibilityRole(
          props->accessibilityRole);
    } else {
      // Do nothing here.
    }
    
    if (!old) {
      if (!props->accessibilityHint.empty()) {
        this->getLocalRootArkUINode().setAccessibilityDescription(
        props->accessibilityHint);
      }
    } else if (props->accessibilityHint != old->accessibilityHint) {
      this->getLocalRootArkUINode().setAccessibilityDescription(
          props->accessibilityHint);
    } else {
      // Do nothing here.
    }

    if (!old) {
      if (static_cast<int32_t>(props->importantForAccessibility) != 0) {
        this->getLocalRootArkUINode().setAccessibilityMode(
        props->importantForAccessibility);
      }
    } else if (props->importantForAccessibility != old->importantForAccessibility) {
      this->getLocalRootArkUINode().setAccessibilityMode(
        props->importantForAccessibility);
    } else {
       // Do nothing here.
    }

    if (!old) {
      if (!props->accessibilityLabel.empty()) {
        this->getLocalRootArkUINode().setAccessibilityText(
          props->accessibilityLabel);
      }
    } else if (props->accessibilityLabel != old->accessibilityLabel) {
      this->getLocalRootArkUINode().setAccessibilityText(
        props->accessibilityLabel);
    } else {
      // Do nothing here.
    }

    if (!old) {
      if (static_cast<int32_t>(props->accessible) != 0) {
        this->getLocalRootArkUINode().setAccessibilityGroup(props->accessible);
        m_accessibilityGroup = props->accessible;
      }
    } else if (props->accessible != old->accessible) {
      this->getLocalRootArkUINode().setAccessibilityGroup(props->accessible);
      m_accessibilityGroup = props->accessible;
    } else {
      // Do nothing here.
    }

    this->setOpacity(props);

    auto clipContentToBounds = props->getClipsContentToBounds();
    if (!old) {
      // 0 -- Do not clip, 1 -- clip content
      if (static_cast<uint32_t>(clipContentToBounds) != 0) {
        m_isClipping = clipContentToBounds;
        this->getLocalRootArkUINode().setClip(clipContentToBounds);
        markBoundingBoxAsDirty();
      }
    } else if (old->getClipsContentToBounds() != clipContentToBounds) {
      m_isClipping = clipContentToBounds;
      this->getLocalRootArkUINode().setClip(clipContentToBounds);
      markBoundingBoxAsDirty();
    } else {
      // Do nothing here.
    }

    auto rawProps = ViewRawProps::getFromDynamic(props->rawProps);
    if (m_rawProps.needsOffscreenAlphaCompositing !=
        rawProps.needsOffscreenAlphaCompositing) {
      m_rawProps.needsOffscreenAlphaCompositing =
          rawProps.needsOffscreenAlphaCompositing;
      this->getLocalRootArkUINode().setRenderGroup(m_rawProps.needsOffscreenAlphaCompositing);
    }

    this->getLocalRootArkUINode().setId(getIdFromProps(props));

    m_oldBorderMetrics = props->resolveBorderMetrics(this->m_layoutMetrics);
  };

  virtual void onStateChanged(SharedConcreteState const& state){};

  virtual void onEventEmitterChanged(
      SharedConcreteEventEmitter const& /*eventEmitter*/){};

  void calculateBoundingBox() {
    auto newBoundingBox = getHitRect();
    if (!m_isClipping) {
      for (auto& child : m_children) {
        auto childBoundingBox = child->getBoundingBox();
        childBoundingBox.origin += child->getLayoutMetrics().frame.origin;

        auto childCenter = child->getLayoutMetrics().frame.getCenter();
        auto childTransform = child->getTransform();

        childBoundingBox = transformRectAroundPoint(
            childBoundingBox, childCenter, childTransform);
        newBoundingBox.unionInPlace(childBoundingBox);
      }
    }
    m_boundingBox = newBoundingBox;
  };

  void onFinalizeUpdates() override {
    ComponentInstance::onFinalizeUpdates();
    if (m_props != nullptr) {
      if (m_props->accessibilityLabelledBy.value != m_accessibilityLabelledBy) {
        std::string targetId = "";
        if (!m_props->accessibilityLabelledBy.value.empty()) {
          targetId = m_props->accessibilityLabelledBy.value[0];
        }
        if (!targetId.empty()) {
          auto componentInstanceRegistry =
            m_deps->componentInstanceRegistry.lock();
          if (componentInstanceRegistry != nullptr) {
            auto componentInstance =
                componentInstanceRegistry->findById(targetId);
            if (componentInstance != nullptr) {
              std::string newAccessibilityLabel = "";
              if (m_props->accessibilityLabel != "") {
                newAccessibilityLabel += m_props->accessibilityLabel;
              }
              auto targetAccessibilityLabel =
                  componentInstance->getAccessibilityLabel();
              if (!targetAccessibilityLabel.empty()) {
                newAccessibilityLabel += " " + targetAccessibilityLabel;
              }
              if (!newAccessibilityLabel.empty()) {
                this->getLocalRootArkUINode().setAccessibilityText(
                    newAccessibilityLabel);
              }
            } else {
              DLOG(WARNING) << "Couldn't find ComponentInstance with Id: "
                            << targetId;
            }
          }
        }
      }
      m_accessibilityLabelledBy = m_props->accessibilityLabelledBy.value;
    }
  }

  facebook::react::Rect getHitRect() const {
    facebook::react::Point origin = {0, 0};
    auto size = m_layoutMetrics.frame.size;
    if (m_props) {
      origin -= {m_props->hitSlop.left, m_props->hitSlop.top};
      size +=
          {m_props->hitSlop.right + m_props->hitSlop.left,
           m_props->hitSlop.top + m_props->hitSlop.bottom};
    }
    return {origin, size};
  };

 private:

  facebook::react::Transform m_transform =
    facebook::react::Transform::Identity();  

  void setTransform(facebook::react::Transform const& transform) {
    m_transform = transform;
  }

  void setOpacity(facebook::react::SharedViewProps const& props) {
    auto isOpacityManagedByAnimated = getIgnoredPropKeys().count("opacity") > 0;
    auto isTransformManagedByAnimated =
        getIgnoredPropKeys().count("transform") > 0;
    bool shouldSetOpacity = true;

    if (isOpacityManagedByAnimated) {
      shouldSetOpacity = false;
    }

    auto opacity = props->opacity;
    float validOpacity = std::max(0.0f, std::min((float)opacity, 1.0f));
    facebook::react::Transform transform = props->transform;
    if (!isTransformManagedByAnimated &&
        props->backfaceVisibility ==
            facebook::react::BackfaceVisibility::Hidden) {
      facebook::react::Vector vec{0, 0, 1, 0};
      auto resVec = transform * vec;
      if (resVec.z < 0.0) {
        validOpacity = 0.0;
        shouldSetOpacity = true;
      }
    }
    if (shouldSetOpacity) {
      this->getLocalRootArkUINode().setOpacity(validOpacity);
    }
  }

  std::string getCompId() override {
    return getIdFromProps(m_props);
  }

    
 protected:
  void onArkUINodeAccessibilityAction(ArkUINode*, const std::string& actionName)
      override {
    if (m_eventEmitter == nullptr) {
      return;
    }
    m_eventEmitter->onAccessibilityAction(actionName);
  }

  void onArkUINodeTouchIntercept(const ArkUI_UIInputEvent* event) override {
    auto mode = HitTestMode::HTM_NONE;
    if (this->canHandleTouch()) {
      mode = HitTestMode::HTM_DEFAULT;
    } else if (this->canChildrenHandleTouch()) {
      auto x = OH_ArkUI_PointerEvent_GetX(event);
      auto y = OH_ArkUI_PointerEvent_GetY(event);
      if (this->canSubtreeHandleTouch({x, y})) {
        mode = HitTestMode::HTM_DEFAULT;
      }
    } else if (IsAtLeastApi20()) {
      mode = HitTestMode::HTM_BLOCK_DESCENDANTS;
    }
    OH_ArkUI_PointerEvent_SetInterceptHitTestMode(event, mode);
  }

  std::string getIdFromProps(
      facebook::react::SharedViewProps const& props) const {
    if (props->testId != "") {
      return props->testId;
    } else if (props->nativeId != "") {
      return props->nativeId;
    } else {
      std::ostringstream id;
      id << m_tag;
      return id.str();
    }
  }

  struct ViewRawProps {
    bool needsOffscreenAlphaCompositing = false;
    std::optional<bool> shouldRasterizeIOS;
    static ViewRawProps getFromDynamic(folly::dynamic value) {
      auto needsOffscreenAlphaCompositing =
          (value.count("needsOffscreenAlphaCompositing") > 0)
          ? value["needsOffscreenAlphaCompositing"].asBool()
          : false;

      auto shouldRasterizeIOS = (value.count("shouldRasterizeIOS") > 0)
        ? std::optional(value["shouldRasterizeIOS"].asBool())
        : std::nullopt;

      return {needsOffscreenAlphaCompositing, shouldRasterizeIOS};
    }
  };
  ViewRawProps m_rawProps;

  SharedConcreteProps m_props;
  SharedConcreteState m_state;
  SharedConcreteEventEmitter m_eventEmitter;
  std::optional<facebook::react::Rect> m_boundingBox;
  bool m_isClipping = false;
  
    static ArkUI_Direction convertLayoutDirection(
      facebook::react::LayoutDirection layoutDirection) {
    switch (layoutDirection) {
      using facebook::react::LayoutDirection;
      case LayoutDirection::LeftToRight:
        return ArkUI_Direction::ARKUI_DIRECTION_LTR;
      case facebook::react::LayoutDirection::RightToLeft:
        return ArkUI_Direction::ARKUI_DIRECTION_RTL;
      default:
        return ArkUI_Direction::ARKUI_DIRECTION_AUTO;
    }
  }
 private:
  std::vector<std::string> m_accessibilityLabelledBy{};
  std::string m_accessibilityLabel;
  bool m_accessibilityGroup;
};

inline facebook::react::Rect transformRectAroundPoint(
    const facebook::react::Rect& rect,
    const facebook::react::Point& point,
    const facebook::react::Transform& transform) {
  using Point = facebook::react::Point;

  auto leftTopTransformed =
      (Point({rect.getMinX(), rect.getMinY()}) - point) * transform + point;
  auto leftBottomTransformed =
      (Point({rect.getMinX(), rect.getMaxY()}) - point) * transform + point;
  auto rightTopTransformed =
      (Point({rect.getMaxX(), rect.getMinY()}) - point) * transform + point;
  auto rightBottomTransformed =
      (Point({rect.getMaxX(), rect.getMaxY()}) - point) * transform + point;
  return facebook::react::Rect::boundingRect(
      leftTopTransformed,
      leftBottomTransformed,
      rightTopTransformed,
      rightBottomTransformed);
}
} // namespace rnoh
