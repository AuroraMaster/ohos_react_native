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
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <folly/dynamic.h>
#include <react/renderer/components/rncore/Props.h>
#include <react/renderer/components/view/AccessibilityPrimitives.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <react/renderer/graphics/Color.h>
#include <react/renderer/graphics/Float.h>
#include <react/renderer/graphics/Rect.h>
#include <react/renderer/graphics/Transform.h>
#include <memory>
#include <stdexcept>
#include "glog/logging.h"
#include "react/renderer/components/view/primitives.h"
#include "NodeApi.h"

namespace rnoh {

namespace {
enum class ArkuiHitTestMode : int32_t {
  DEFAULT,
  BLOCK,
  TRANSPARENT,
  NONE,
};
} // namespace

constexpr float OH_SDK_API_LEVEL_14 = 14.0f;
constexpr float OH_SDK_API_LEVEL_15 = 15.0f;

class ArkUINode;

class ArkUINodeDelegate {
 public:
  virtual ~ArkUINodeDelegate() = default;
  virtual void onArkUINodeDestroy(ArkUINode* /*node*/){};
  virtual void onArkUINodeAccessibilityAction(
      ArkUINode* node,
      const std::string& actionName){};
  virtual void onArkUINodeTouchIntercept(const ArkUI_UIInputEvent* event){};
};

class ArkUINode {
  ArkUINodeDelegate* m_arkUINodeDelegate = nullptr;
  public:
  struct Context {
    Context() = default;
    explicit Context(std::shared_ptr<NodeApi> nodeApi) : nodeApi(nodeApi) {}
    using Shared = std::shared_ptr<Context>;
    std::shared_ptr<NodeApi> nodeApi = nullptr;
  };

 protected:
  ArkUINode(const ArkUINode& other) = delete;
  ArkUINode& operator=(const ArkUINode& other) = delete;

  ArkUINode& operator=(ArkUINode&& other) noexcept;
  ArkUINode(ArkUINode&& other) noexcept;

  using EventArgs = ArkUI_NumberValue[MAX_COMPONENT_EVENT_ARG_NUM];

 public:
  enum {
      UI_INPUT_EVENT_SOURCE_TYPE_UNKNOWN = 0,
      UI_INPUT_EVENT_SOURCE_TYPE_MOUSE = 1,
      UI_INPUT_EVENT_SOURCE_TYPE_TOUCH_SCREEN = 2,
      UI_INPUT_EVENT_SOURCE_TYPE_KEY = 4
  };
  using Alignment = ArkUI_Alignment;

  ArkUI_NodeHandle getArkUINodeHandle();
  ArkUINode(ArkUI_NodeHandle nodeHandle);
  ArkUINode(const ArkUINode::Context::Shared context, ArkUI_NodeType nodeType);

  void setArkUINodeDelegate(ArkUINodeDelegate* arkUiNodeDelegate);

  void markDirty();
  void saveSize(int32_t width, int32_t height);
  int32_t getSavedWidth();
  int32_t getSavedHeight();

  virtual ArkUINode& setAccessibilityRole(std::string const& role);
  virtual ArkUINode& setPosition(facebook::react::Point const& position);
  virtual ArkUINode& setSize(facebook::react::Size const& size);
  virtual ArkUINode& setLayoutRect(
    facebook::react::Point const& position, facebook::react::Size const& size,
    facebook::react::Float pointScaleFactor);
  virtual ArkUINode& setHeight(float height);
  virtual ArkUINode& setWidth(float width);
  virtual ArkUINode& setBorderWidth(
      facebook::react::BorderWidths const& borderWidths);
  virtual ArkUINode& setBorderColor(
      facebook::react::BorderColors const& borderColors);
  virtual ArkUINode& setBorderRadius(
      facebook::react::BorderRadii const& borderRadius);
  virtual ArkUINode& setBorderStyle(
      facebook::react::BorderStyles const& borderStyles);
  virtual ArkUINode& setBackgroundColor(
      facebook::react::SharedColor const& color);
  virtual ArkUINode& setTransform(
      facebook::react::Transform const& transform,
      facebook::react::Float pointScaleFactor);
  virtual ArkUINode& setTranslate(float x, float y, float z = 0.0f);
  virtual ArkUINode& setShadow(
      facebook::react::SharedColor const& shadowColor,
      facebook::react::Size const& shadowOffset,
      float const shadowOpacity,
      float const shadowRadius,
      facebook::react::Float pointScaleFactor);
  virtual ArkUINode& setHitTestMode(
      facebook::react::PointerEventsMode const& pointerEvents);
  virtual ArkUINode& setAccessibilityActions(
      const std::vector<facebook::react::AccessibilityAction>& rnActions);
  virtual ArkUINode& setAccessibilityDescription(
      std::string const& accessibilityDescription);
  virtual ArkUINode& setAccessibilityState(
      const facebook::react::AccessibilityState& state);
  virtual ArkUINode& setAccessibilityLevel(
      facebook::react::ImportantForAccessibility importance);
  virtual ArkUINode& setAccessibilityMode(
      facebook::react::ImportantForAccessibility importance);
  virtual ArkUINode& setAccessibilityMode(ArkUI_AccessibilityMode mode);
  virtual ArkUINode& setAccessibilityText(
      std::string const& accessibilityLabel);
  virtual ArkUINode& setAccessibilityGroup(bool accessible);
  virtual ArkUINode& setId(std::string const& id);
  virtual std::string getId() const;  
  virtual ArkUINode& setOpacity(facebook::react::Float opacity);
  virtual ArkUINode& setClip(bool clip);
  virtual ArkUINode& setAlignment(Alignment alignment);
  virtual ArkUINode& setTranslateTransition(
      float translateX,
      float translateY,
      int32_t animationDurationMillis);
  virtual ArkUINode& resetTranslateTransition();
  virtual ArkUINode& setOpacityTransition(int32_t animationDurationMillis);
  virtual ArkUINode& resetOpacityTransition();
  virtual ArkUINode& setOffset(float x, float y);
  virtual ArkUINode& setEnabled(bool enabled);
  virtual ArkUINode& setFocusStatus(int32_t focus);
  virtual ArkUINode&
  setMargin(float left, float top, float right, float bottom);
  virtual ArkUINode&
  setPadding(float left, float top, float right, float bottom);
  virtual ArkUINode& setVisibility(ArkUI_Visibility visibility);
  virtual ArkUINode& setRenderGroup(bool flag);
  virtual ArkUINode& setDirection(ArkUI_Direction direction);
  virtual ArkUINode& setZIndex(float index);

  virtual ArkUINode& resetAccessibilityText();

  virtual ArkUI_IntOffset getLayoutPosition();

  // method for handling node events with ArkUI_NodeComponentEvent payload
  // type
  virtual void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs);
  // method for handling node events with ArkUI_StringAsyncEvent payload type
  virtual void onNodeEvent(
      ArkUI_NodeEventType eventType,
      std::string_view eventString);
  /**
   * @brief Method for handling node events without a specific type
   * @param eventType Event type identifier
   * @param ArkUI_NodeEvent* pointer to the event
   */
  virtual void onNodeEvent(
      ArkUI_NodeEventType eventType,
      ArkUI_NodeEvent* event);
  virtual void onTouchIntercept(const ArkUI_UIInputEvent* event);
  
  /**
   * @brief Verify the focus status of the TextInput.
   * @return Focus status: true-focused, false-unfocused.
   */
  virtual bool isFocused();

  virtual ~ArkUINode() noexcept;

 protected:
  void maybeThrow(int32_t status) {
    // TODO: map status to error message, maybe add a new error type
    static const auto ARKUI_ERROR_CODE_NOT_SUPPORTED_FOR_ARKTS_NODE = 106103;
    if (status == ARKUI_ERROR_CODE_NOT_SUPPORTED_FOR_ARKTS_NODE) {
      // This is a quick fix for a problem that has arisen after updating the
      // ROM and SDK.
      LOG(WARNING) << "Couldn't set a property on ArkTS node";
      return;
    }
    if (status != 0) {
      auto message = std::string("ArkUINode operation failed with status: ") +
          std::to_string(status);
      LOG(ERROR) << message;
      throw std::runtime_error(std::move(message));
    }
  }
    
  void registerNodeEvent(ArkUI_NodeEventType eventType);
  void unregisterNodeEvent(ArkUI_NodeEventType eventType);  
  const ArkUI_AttributeItem& getAttribute(
      ArkUI_NodeAttributeType attribute) const;

  void setAttribute(
      ArkUI_NodeAttributeType attribute,
      ArkUI_AttributeItem const& item);

  void setAttribute(
      ArkUI_NodeAttributeType attribute,
      std::initializer_list<ArkUI_NumberValue> values);

  template <size_t N>
  void setAttribute(
      ArkUI_NodeAttributeType attribute,
      std::array<ArkUI_NumberValue, N> const& values) {
    ArkUI_AttributeItem item{.value = values.data(), .size = N};
    setAttribute(attribute, item);
  }

  ArkUI_NodeHandle m_nodeHandle;
  std::shared_ptr<NodeApi> m_nodeApi = std::make_shared<NodeApi>();

 private:
  int32_t m_width = 0;
  int32_t m_height = 0;
};
} // namespace rnoh
