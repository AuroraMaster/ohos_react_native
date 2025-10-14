/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/Package.h"
#include "RNOH/generated/BaseFabricComponentSamplePackagePackage.h"

namespace rnoh {
class FabricComponentSamplePackagePackage : public BaseFabricComponentSamplePackagePackage {
  using Super = BaseFabricComponentSamplePackagePackage;

 public:
  FabricComponentSamplePackagePackage(Package::Context ctx)
      : BaseFabricComponentSamplePackagePackage(ctx) {}

  ComponentInstance::Shared createComponentInstance(
      const ComponentInstance::Context& ctx) override;
};
} // namespace rnoh
