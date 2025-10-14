/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOHCorePackage/TurboModules/AccessibilityInfoTurboModule.h"

namespace rnoh {
using namespace facebook;

AccessibilityInfoTurboModule::AccessibilityInfoTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_ASYNC_METHOD_METADATA(isScreenReaderEnabled, 0),
      ARK_ASYNC_METHOD_METADATA(isBoldTextEnabled, 0),
      ARK_ASYNC_METHOD_METADATA(announceForAccessibility, 1),
      ARK_ASYNC_METHOD_METADATA(setAccessibilityFocus, 1),
      ARK_ASYNC_METHOD_METADATA(isAccessibilityServiceEnabled, 0)};
}

} // namespace rnoh
