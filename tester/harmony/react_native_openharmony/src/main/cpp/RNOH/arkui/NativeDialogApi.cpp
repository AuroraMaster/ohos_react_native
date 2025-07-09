/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "NativeDialogApi.h"
#include <glog/logging.h>

namespace rnoh {

ArkUI_NativeDialogAPI_1* NativeDialogApi::getInstance() {
#ifdef C_API_ARCH
  static ArkUI_NativeDialogAPI_1* INSTANCE = nullptr;
  if (INSTANCE == nullptr) {
    OH_ArkUI_GetModuleInterface(
        ARKUI_NATIVE_DIALOG, ArkUI_NativeDialogAPI_1, INSTANCE);
    if (INSTANCE == nullptr) {
      LOG(FATAL) << "Failed to get ArkUI_NativeDialogAPI_1 instance.";
    }
  }
  return INSTANCE;
#endif
  LOG(FATAL)
      << "This method should only by used when C-API architecture is enabled.";
}

ArkUI_NativeDialogAPI_2* NativeDialogApi::getInstance2() {
#ifdef C_API_ARCH
  static ArkUI_NativeDialogAPI_2* INSTANCE = nullptr;
  if (INSTANCE == nullptr) {
    OH_ArkUI_GetModuleInterface(
        ARKUI_NATIVE_DIALOG, ArkUI_NativeDialogAPI_2, INSTANCE);
    if (INSTANCE == nullptr) {
      LOG(FATAL)
          << "Failed to get ArkUI_NativeDialogAPI_2 instance. "
          << "Please check whether the ROM and SDK have been upgraded to API level 15 or above.";
    }
  }
  return INSTANCE;
#endif
  LOG(FATAL)
      << "This method should only by used when C-API architecture is enabled.";
}

} // namespace rnoh