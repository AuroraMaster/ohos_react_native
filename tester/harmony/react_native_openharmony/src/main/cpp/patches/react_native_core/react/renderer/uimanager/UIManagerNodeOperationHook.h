/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
// RNOH patch add file

#include <react/renderer/core/ShadowNode.h>

namespace facebook::react {

/*
 * Implementing a node operation hook allows to observe nodes operations in the
 * host platform.
 */
class UIManagerNodeOperationHook {
 public:
  /*
   * Called right after a new shadow node was created.
   */
  virtual void uiManagerDidCreateShadowNode(
      const ShadowNode& shadowNode) noexcept = 0;

  /*
   * Called right after a shadow node was cloned.
   */
  virtual void uiManagerDidCloneShadowNode(
      const ShadowNode& shadowNode,
      const ShadowNode& clonedShadowNode) noexcept = 0;

  /*
   * Called right after a new child node was inserted to a parent shadow node.
   */
  virtual void uiManagerDidAppendChildNode(
      const ShadowNode& parentShadowNode,
      const ShadowNode& childShadowNode) noexcept = 0;

  virtual ~UIManagerNodeOperationHook() noexcept = default;
};

} // namespace facebook::react
