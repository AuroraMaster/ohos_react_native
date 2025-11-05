/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "NativeNodeApi.h"
#include <glog/logging.h>
#include "RNOH/ParallelCheck.h"

namespace rnoh {

ArkUI_NativeNodeAPI_1* NativeNodeApi::getInstance() {
#ifdef C_API_ARCH
  static ArkUI_NativeNodeAPI_1* INSTANCE = nullptr;
  if (INSTANCE == nullptr) {
#ifdef PARALLELIZATION_ON
    if (IsParallelizationWorkable()) {
      OH_ArkUI_GetModuleInterface(
          ARKUI_MULTI_THREAD_NATIVE_NODE, ArkUI_NativeNodeAPI_1, INSTANCE);
    } else {
      OH_ArkUI_GetModuleInterface(
          ARKUI_NATIVE_NODE, ArkUI_NativeNodeAPI_1, INSTANCE);
    }
#else 
    OH_ArkUI_GetModuleInterface(
          ARKUI_NATIVE_NODE, ArkUI_NativeNodeAPI_1, INSTANCE);
#endif
    if (INSTANCE == nullptr) {
      LOG(FATAL) << "Failed to get native node API instance.";
    }
  }
  return INSTANCE;
#endif
  LOG(FATAL)
      << "This method should only by used when C-API architecture is enabled.";
}

} // namespace rnoh