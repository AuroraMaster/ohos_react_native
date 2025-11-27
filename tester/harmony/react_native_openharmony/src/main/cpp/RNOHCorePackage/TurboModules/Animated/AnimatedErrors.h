/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/core/ReactPrimitives.h>
#include <stdexcept>
#include <string>

namespace rnoh {

class AnimatedNodeNotFoundError : public std::runtime_error {
 public:
  explicit AnimatedNodeNotFoundError(facebook::react::Tag tag)
      : std::runtime_error(
            "Animated node with tag " + std::to_string(tag) +
            " does not exist") {}
};

} // namespace rnoh
