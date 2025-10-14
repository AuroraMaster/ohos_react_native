/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <vector>
#include "RNOH/Package.h"

namespace rnoh {

class PackageProvider {
 public:
  std::vector<std::shared_ptr<Package>> getPackages(Package::Context ctx);
};

} // namespace rnoh