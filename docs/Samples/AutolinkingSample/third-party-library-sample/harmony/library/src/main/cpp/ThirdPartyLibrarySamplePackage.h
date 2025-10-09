/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "RNOH/Package.h"
#include "RNOH/generated/BaseThirdPartyLibrarySamplePackage.h"

namespace rnoh {
class ThirdPartyLibrarySamplePackage
    : public BaseThirdPartyLibrarySamplePackage {
  using Super = BaseThirdPartyLibrarySamplePackage;

 public:
  explicit ThirdPartyLibrarySamplePackage(Package::Context ctx)
      : BaseThirdPartyLibrarySamplePackage(ctx) {}

  ComponentInstance::Shared createComponentInstance(
      const ComponentInstance::Context& ctx) override;
};
} // namespace rnoh
