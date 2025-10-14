/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "ArkUINode.h"

namespace rnoh {

class LoadingProgressNode : public ArkUINode {
 public:
  explicit LoadingProgressNode(const ArkUINode::Context::Shared& context = nullptr);
  LoadingProgressNode& setColor(facebook::react::SharedColor const& color);
  LoadingProgressNode& setAnimating(const bool& enable);
};

} // namespace rnoh