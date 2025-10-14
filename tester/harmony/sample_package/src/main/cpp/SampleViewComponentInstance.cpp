/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "SampleViewComponentInstance.h"

namespace rnoh {

SampleViewComponentInstance::SampleViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)) {
  m_customNode.setCustomNodeDelegate(this);
  m_customNode.insertChild(m_textNode, 0);
  m_textNode.setTextContent(std::to_string(m_tag));
  m_textNode.setFontSize(m_fontSize);
  m_textNode.setFontColor(*facebook::react::whiteColor());
}

void SampleViewComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance,
    std::size_t index) {
  CppComponentInstance::onChildInserted(childComponentInstance, index);
  m_customNode.insertChild(
      childComponentInstance->getLocalRootArkUINode(), index + 1);
}

void SampleViewComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance) {
  CppComponentInstance::onChildRemoved(childComponentInstance);
  m_customNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}

void SampleViewComponentInstance::onPropsChanged(
    SharedConcreteProps const& props) {
  CppComponentInstance::onPropsChanged(props);
  if (props->textColor) {
    m_textNode.setFontColor(*props->textColor);
  } else {
    m_textNode.resetFontColor();
  }
  DLOG(INFO) << "SampleView props changed";
}

void SampleViewComponentInstance::onCommandReceived(
    std::string const& commandName,
    folly::dynamic const& args) {
  if (commandName == "toggleFontSize") {
    m_fontSize = m_fontSize == 12 ? 20 : 12;
    m_textNode.setFontSize(m_fontSize);
    return;
  }

  CppComponentInstance::onCommandReceived(commandName, args);
}

ArkUINode& SampleViewComponentInstance::getLocalRootArkUINode() {
  return m_customNode;
}

void SampleViewComponentInstance::onClick() {
  DLOG(INFO) << "SampleView clicked";
  m_eventEmitter->onSampleClick();
}

} // namespace rnoh
