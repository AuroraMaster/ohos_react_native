/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ImageNode.h"

#include <string_view>
#include "NativeNodeApi.h"

static constexpr ArkUI_NodeEventType IMAGE_NODE_EVENT_TYPES[] = {
    NODE_IMAGE_ON_COMPLETE,
    NODE_IMAGE_ON_ERROR,
    NODE_IMAGE_ON_DOWNLOAD_PROGRESS};

namespace rnoh {

using namespace std::literals;
constexpr std::string_view ASSET_PREFIX = "asset://"sv;
const std::string RAWFILE_PREFIX = "resource://RAWFILE/assets/";

ImageNode::ImageNode(const ArkUINode::Context::Shared& context)
    : ArkUINode(context, ArkUI_NodeType::ARKUI_NODE_IMAGE),
      m_childArkUINodeHandle(nullptr),
      m_imageNodeDelegate(nullptr) {
  for (auto eventType : IMAGE_NODE_EVENT_TYPES) {
    registerNodeEvent(eventType);
  }
}

ImageNode::~ImageNode() {
  for (auto eventType : IMAGE_NODE_EVENT_TYPES) {
    unregisterNodeEvent(eventType);
  }
}

void ImageNode::setNodeDelegate(ImageNodeDelegate* imageNodeDelegate) {
  m_imageNodeDelegate = imageNodeDelegate;
}

void ImageNode::onNodeEvent(
    ArkUI_NodeEventType eventType,
    EventArgs& eventArgs) {
  ArkUINode::onNodeEvent(eventType, eventArgs);
  if (eventType == ArkUI_NodeEventType::NODE_IMAGE_ON_COMPLETE) {
    if (m_imageNodeDelegate != nullptr && eventArgs[0].i32 == 1) {
      m_imageNodeDelegate->onComplete(eventArgs[1].f32, eventArgs[2].f32);
    }
  }

  if (eventType == ArkUI_NodeEventType::NODE_IMAGE_ON_ERROR) {
    if (m_imageNodeDelegate != nullptr) {
      m_imageNodeDelegate->onError(eventArgs[0].i32);
    }
  }

  if (eventType == ArkUI_NodeEventType::NODE_IMAGE_ON_DOWNLOAD_PROGRESS) {
    if (m_imageNodeDelegate != nullptr) {
      m_imageNodeDelegate->onProgress(eventArgs[0].u32, eventArgs[1].u32);
    }
  }
}

ImageNode& ImageNode::setSources(std::string const& uri, std::string prefix) {
  m_uri = uri;
  ArkUI_AttributeItem item;
  std::string absolutePath = prefix == "" ? RAWFILE_PREFIX : prefix;
  if (uri.rfind(ASSET_PREFIX, 0) == 0) {
    absolutePath += uri.substr(ASSET_PREFIX.size());
    item = {.string = absolutePath.c_str()};
  } else {
    item = {.string = uri.c_str()};
  }
  m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_SRC, &item);
  return *this;
}

ImageNode& ImageNode::setResizeMode(
    facebook::react::ImageResizeMode const& mode) {
  int32_t val = ARKUI_OBJECT_FIT_COVER;
  if (mode == facebook::react::ImageResizeMode::Cover) {
    val = ARKUI_OBJECT_FIT_COVER;
  } else if (mode == facebook::react::ImageResizeMode::Contain) {
    val = ARKUI_OBJECT_FIT_CONTAIN;
  } else if (mode == facebook::react::ImageResizeMode::Stretch) {
    val = ARKUI_OBJECT_FIT_FILL;
  } else if (
      mode == facebook::react::ImageResizeMode::Center ||
      mode == facebook::react::ImageResizeMode::Repeat) {
    val = ARKUI_OBJECT_FIT_NONE;
  }

  ArkUI_NumberValue value[] = {{.i32 = val}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_OBJECT_FIT, &item);
  return *this;
}

ImageNode& ImageNode::setTintColor(
    facebook::react::SharedColor const& sharedColor) {
  if (!sharedColor) { // restore default value
    m_nodeApi->resetAttribute(m_nodeHandle, NODE_IMAGE_COLOR_FILTER);
    return *this;
  }

  facebook::react::ColorComponents com = colorComponentsFromColor(sharedColor);
  ArkUI_NumberValue value[] = {
      {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = com.red},   {.f32 = 0},
      {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = com.green}, {.f32 = 0},
      {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = com.blue},  {.f32 = 0},
      {.f32 = 0}, {.f32 = 0}, {.f32 = 0}, {.f32 = com.alpha}, {.f32 = 0}};

  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_COLOR_FILTER, &item);
  return *this;
}

ImageNode& ImageNode::setBlur(facebook::react::Float blur) {
  ArkUI_NumberValue value[] = {{.f32 = static_cast<float>(blur)}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_BLUR, &item);
  return *this;
}

ImageNode& ImageNode::setObjectRepeat(
    facebook::react::ImageResizeMode const& resizeMode) {
  int32_t val = ARKUI_IMAGE_REPEAT_NONE;
  if (resizeMode == facebook::react::ImageResizeMode::Repeat) {
    val = ARKUI_IMAGE_REPEAT_XY;
  }

  ArkUI_NumberValue value[] = {{.i32 = val}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_OBJECT_REPEAT, &item);
  return *this;
}

ImageNode& ImageNode::setInterpolation(int32_t interpolation) {
  ArkUI_NumberValue value[] = {{.i32 = interpolation}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_INTERPOLATION, &item);
  return *this;
}

ImageNode& ImageNode::setDraggable(bool draggable) {
  ArkUI_NumberValue value[] = {{.i32 = static_cast<int32_t>(draggable)}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_DRAGGABLE, &item);
  return *this;
}

ImageNode& ImageNode::setFocusable(bool focusable) {
  ArkUI_NumberValue value[] = {{.i32 = static_cast<int32_t>(focusable)}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_FOCUSABLE, &item);
  return *this;
}

ImageNode& ImageNode::setResizeMethod(std::string const& resizeMethod) {
  auto autoResize = (resizeMethod != "scale") ? 1 : 0;
  ArkUI_NumberValue value[] = {{.i32 = autoResize}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_AUTO_RESIZE, &item);
  return *this;
}

ImageNode& ImageNode::setAlt(std::string const& uri, std::string prefix) {
  if (!uri.empty()) {
    std::string resourceStr = prefix == "" ? RAWFILE_PREFIX : prefix;
    resourceStr += uri.substr(ASSET_PREFIX.size());
    ArkUI_AttributeItem item = {.string = resourceStr.c_str()};
    m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_ALT, &item);
  }
  return *this;
}

ImageNode& ImageNode::setCapInsets(facebook::react::EdgeInsets const& capInsets, float dpi)
{
  if (capInsets == facebook::react::EdgeInsets::ZERO) {
    return *this;
  }

  float left = capInsets.left / (dpi * 2);
  float right = capInsets.right < 1 ? 1 : capInsets.right / (dpi * 2); // arkui need right >= 1 if wants capInsets works
  float top = capInsets.top / (dpi * 2);
  float bottom =  capInsets.bottom < 1 ? 1 : capInsets.bottom / (dpi * 2); // arkui need bottom >= 1 if wants capInsets works
  ArkUI_NumberValue value[] = {{.f32 = left}, {.f32 = top}, {.f32 = right}, {.f32 = bottom}};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_IMAGE_RESIZABLE, &item);
  return *this;
}

ImageNode& ImageNode::setFadeDuration(int32_t duration) {
  // TODO: duration should have a range and maybe need to be checked here.
  ArkUI_NumberValue value[] = {{ .f32 = 0.0 }, { .i32 = duration }, { .i32 = ARKUI_CURVE_LINEAR }};
  ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
  m_nodeApi->setAttribute(m_nodeHandle, NODE_OPACITY_TRANSITION, &item);
  return *this;
}

ImageNode& ImageNode::setAccessibilityMode(
  facebook::react::ImportantForAccessibility importance) {
  if (importance == facebook::react::ImportantForAccessibility::Auto) {
    ArkUINode::setAccessibilityMode(
        ArkUI_AccessibilityMode::ARKUI_ACCESSIBILITY_MODE_ENABLED);
  } else {
    ArkUINode::setAccessibilityMode(importance);
  }
  return *this;
}
ImageNode& ImageNode::resetFocusable() {
  m_nodeApi->resetAttribute(m_nodeHandle, NODE_FOCUSABLE);
  return *this;
}
ImageNode& ImageNode::resetResizeMethod() {
  m_nodeApi->resetAttribute(m_nodeHandle, NODE_IMAGE_AUTO_RESIZE);
  return *this;
}

std::string ImageNode::getUri() {
  return m_uri;
}
} // namespace rnoh
