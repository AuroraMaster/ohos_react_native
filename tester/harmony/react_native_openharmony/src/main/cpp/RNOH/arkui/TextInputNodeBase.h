/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "ArkUINode.h"
#include "react/renderer/components/textinput/TextInputProps.h"
#include "react/renderer/graphics/Color.h"

namespace rnoh {

class TextInputNodeBase : public ArkUINode {
 protected:
  TextInputNodeBase(const ArkUINode::Context::Shared& context, ArkUI_NodeType nodeType);
  void setCommonFontAttributes(
      facebook::react::TextAttributes const& textAttributes);
    const float defaultFontSize = 16.0;

 public:
  void setPadding(
      facebook::react::RectangleEdges<facebook::react::Float> padding);

  void setFocusable(bool const& focusable);

  void setResponseRegion(
      facebook::react::Point const& position,
      facebook::react::Size const& size);

  void setTextContent(std::string const& textContent);

  void setTextSelection(int32_t start, int32_t end);

  void setFontColor(facebook::react::SharedColor const& color);

  virtual void setFont(
      facebook::react::TextAttributes const& textAttributes) = 0;

  virtual void setCaretColor(facebook::react::SharedColor const& color) = 0;

  virtual void setMaxLength(int32_t maxLength) = 0;

  virtual void setPlaceholder(std::string const& placeholder) = 0;

  virtual void setPlaceholderColor(
      facebook::react::SharedColor const& color) = 0;

  void setTextAlign(
      std::optional<facebook::react::TextAlignment> const& textAlign);

  void setTextInputLineHeight(
        facebook::react::TextAttributes const& textAttributes);

  std::string virtual getTextContent() = 0;
  
  virtual facebook::react::Rect getTextContentRect() const = 0;

  void resetMaxLength();
};

} // namespace rnoh