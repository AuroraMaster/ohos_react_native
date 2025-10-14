/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "RNOH/arkui/StackNode.h"
#include "RNOH/arkui/TextNode.h"
#include "RNOH/generated/components/BaseSimpleTextComponentInstance.h"

namespace rnoh {
class SimpleTextComponentInstance : public BaseSimpleTextComponentInstance {
  using Super = BaseSimpleTextComponentInstance;

  StackNode m_stackNode;
  TextNode m_textNode;

 public:
  SimpleTextComponentInstance(Context context) : Super(std::move(context)) {
    m_stackNode.insertChild(m_textNode, 0);
  }

  ArkUINode& getLocalRootArkUINode() {
    return m_stackNode;
  };

  void onPropsChanged(SharedConcreteProps const& props) {
    this->changeText(props->text);
  };

 private:
  void changeText(const std::string& text) {
    m_textNode.setTextContent(text);
    if (m_eventEmitter) {
      m_eventEmitter->onTextChange({.text = text});
    }
  }
};

} // namespace rnoh