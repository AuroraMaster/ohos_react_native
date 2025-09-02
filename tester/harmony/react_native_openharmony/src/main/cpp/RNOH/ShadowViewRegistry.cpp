/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/ShadowViewRegistry.h"

namespace rnoh {

void ShadowViewRegistry::setShadowView(
    facebook::react::Tag tag,
    facebook::react::ShadowView const& shadowView) {
  m_shadowViewEntryByTag.insert_or_assign(
      tag,
      ShadowViewEntry{
          shadowView.eventEmitter, shadowView.state, shadowView.componentName});
}

void ShadowViewRegistry::clearShadowView(facebook::react::Tag tag) {
  m_shadowViewEntryByTag.erase(tag);
}

std::string ShadowViewRegistry::getComponentName(facebook::react::Tag tag) {
  auto it = m_shadowViewEntryByTag.find(tag);
  if (it != m_shadowViewEntryByTag.end()) {
    return it->second.componentName;
  }
  return "";
}

} // namespace rnoh
