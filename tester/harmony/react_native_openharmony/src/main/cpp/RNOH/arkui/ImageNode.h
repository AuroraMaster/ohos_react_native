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
#include <react/renderer/imagemanager/primitives.h>
#include "ArkUINode.h"

namespace rnoh {

class ImageNodeDelegate {
 public:
  virtual ~ImageNodeDelegate() noexcept(false) = default;
  virtual void onComplete(float width, float height) {};
  virtual void onError(int32_t errorCode) {};
  virtual void onProgress(uint32_t loaded, uint32_t total) {};
};

class ImageNode : public ArkUINode {
 protected:
  ArkUI_NodeHandle m_childArkUINodeHandle;
  ImageNodeDelegate* m_imageNodeDelegate;
  std::string m_uri;

 public:
  explicit ImageNode(const ArkUINode::Context::Shared& context = nullptr);
  ~ImageNode();
  ImageNode& setSources(std::string const& uri, std::string prefix = "");
  ImageNode& setResizeMode(facebook::react::ImageResizeMode const& mode);
  ImageNode& setTintColor(facebook::react::SharedColor const& sharedColor);
  ImageNode& setBlur(facebook::react::Float blur);
  ImageNode& setObjectRepeat(
      facebook::react::ImageResizeMode const& resizeMode);
  ImageNode& setCapInsets(facebook::react::EdgeInsets const& capInsets, float dpi);

  ImageNode& setInterpolation(int32_t interpolation);
  ImageNode& setDraggable(bool draggable);
  ImageNode& setFocusable(bool focusable);
  ImageNode& setResizeMethod(std::string const& resizeMethod);
  ImageNode& setAlt(std::string const& uri, std::string prefix = "");
  ImageNode& setFadeDuration(int32_t duration);
  ImageNode& setAccessibilityMode(
      facebook::react::ImportantForAccessibility importance) override;

  ImageNode& resetFocusable();
  ImageNode& resetResizeMethod();

  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;
  void setNodeDelegate(ImageNodeDelegate* imageNodeDelegate);

  std::string getUri();
};
} // namespace rnoh
