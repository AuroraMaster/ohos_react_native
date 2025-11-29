/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ModalHostViewComponentDescriptor.h"
#include <react/renderer/components/modal/ModalHostViewShadowNode.h>
#include <react/renderer/core/ConcreteComponentDescriptor.h>
#include "RNOH/ArkTSBridge.h"

namespace rnoh {

facebook::react::State::Shared ModalHostViewComponentDescriptor::createInitialState(
    facebook::react::ShadowNodeFragment const &fragment,
    const facebook::react::ShadowNodeFamily::Shared& family) const {
  facebook::react::Size screenSize = {0, 0};
  auto rnInstance = contextContainer_->at<std::weak_ptr<rnoh::RNInstance>>(
      "RNOH::RNInstance");
  auto maybeSurface =
      rnInstance.lock()->getSurfaceByRootTag(family->getSurfaceId());
  if (maybeSurface.has_value()) {
    auto surface = maybeSurface.value().lock();
    if (surface) {
      auto displayMetrics = surface->getDisplayMetrics();
      screenSize.height = displayMetrics.windowPhysicalPixels.height /
          displayMetrics.windowPhysicalPixels.scale;
      screenSize.width = displayMetrics.windowPhysicalPixels.width /
          displayMetrics.windowPhysicalPixels.scale;
    }
  }
  auto data =
      std::make_shared<const facebook::react::ModalHostViewState>(screenSize);
  return std::make_shared<
      facebook::react::ModalHostViewShadowNode::ConcreteState>(data, family);
}

} // namespace rnoh