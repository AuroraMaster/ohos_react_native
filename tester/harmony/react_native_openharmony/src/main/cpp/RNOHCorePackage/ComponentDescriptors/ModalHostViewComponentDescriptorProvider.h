/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/componentregistry/ComponentDescriptorProvider.h>
#include "./ModalHostViewComponentDescriptor.h"

namespace rnoh {
facebook::react::ComponentDescriptor::Unique createModalHostViewComponentDescriptor(
    const facebook::react::ComponentDescriptorParameters& parameters) {
  return std::make_unique<const rnoh::ModalHostViewComponentDescriptor>(
      parameters);
}

facebook::react::ComponentDescriptorProvider createModalHostViewComponentDescriptorProvider() {
  return facebook::react::ComponentDescriptorProvider{
      facebook::react::ModalHostViewShadowNode::Handle(),
      facebook::react::ModalHostViewShadowNode::Name(),
      nullptr,
      &createModalHostViewComponentDescriptor,
  };
}
} // namespace rnoh