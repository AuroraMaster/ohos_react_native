/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ThirdPartyLibrarySamplePackage.h"
#include "QDGestureFloatComponentInstance.h"
#include "RNOH/RNInstanceCAPI.h"
#include "RNOH/generated/BaseThirdPartyLibrarySamplePackage.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"

using namespace facebook;

namespace rnoh {
ComponentInstance::Shared ThirdPartyLibrarySamplePackage::createComponentInstance(
    const ComponentInstance::Context& ctx) {
  if (ctx.componentName == "QDGestureFloat") {
    return std::make_shared<QDGestureFloatComponentInstance>(std::move(ctx));
  }
  return nullptr;
};
} // namespace rnoh
