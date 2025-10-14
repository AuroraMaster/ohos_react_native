/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "RNOH/generated/BaseReactNativeHarmonySamplePackage2Package.h"

namespace rnoh {

class CodegenLibSamplePackage
    : public BaseReactNativeHarmonySamplePackage2Package {
  using Super = BaseReactNativeHarmonySamplePackage2Package;

 public:
  CodegenLibSamplePackage(Package::Context ctx) : Super(ctx) {}

  ComponentInstance::Shared createComponentInstance(
      const ComponentInstance::Context& ctx) {
    return nullptr;
  };
};
} // namespace rnoh
