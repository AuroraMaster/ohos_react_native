/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/PackageProvider.h"
#include "RNOHPackagesFactory.h"

using namespace rnoh;

std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(
    Package::Context ctx) {
  auto packages = createRNOHPackages(ctx); // <= autolinking
  return packages;
}