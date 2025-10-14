/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ButtonViewComponentInstance.h"
#include "Props.h"
#include <folly/dynamic.h>

using namespace rnoh;

ButtonViewComponentInstance::ButtonViewComponentInstance(
    Context context)
    : CppComponentInstance(std::move(context)) {
  m_buttonNode.setButtonNodeDelegate(this);
}

void ButtonViewComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance,
    std::size_t index) {
  CppComponentInstance::onChildInserted(childComponentInstance, index);
  m_buttonNode.insertChild(
      childComponentInstance->getLocalRootArkUINode(), index);
}

ButtonNode& ButtonViewComponentInstance::getLocalRootArkUINode() {
  return m_buttonNode;
}

void ButtonViewComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance) {
  CppComponentInstance::onChildRemoved(childComponentInstance);
  m_buttonNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}

void ButtonViewComponentInstance::onPropsChanged(
    SharedConcreteProps const& props) {
  CppComponentInstance::onPropsChanged(props);
  if (!m_props || props->buttonText != m_props->buttonText) {
    m_buttonNode.setLabel(props->buttonText);
  }
}

void ButtonViewComponentInstance::onButtonClick() {
  facebook::react::ButtonViewEventEmitter::OnButtonClick m_onButtonClick;
  m_onButtonClick.isButtonClick = true;
  m_onButtonClick.type = "custom";
  m_eventEmitter->onButtonClick(m_onButtonClick);
}

void ButtonViewComponentInstance::handleCommand(std::string const &commandName, folly::dynamic const &args) {
    if (commandName == "changeButtonText") {
        m_buttonNode.setLabel(args[0].asString());
    }
}
