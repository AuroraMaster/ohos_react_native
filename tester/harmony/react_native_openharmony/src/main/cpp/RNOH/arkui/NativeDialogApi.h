/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <arkui/native_dialog.h>
#include <arkui/native_interface.h>

namespace rnoh {

class NativeDialogApi {
 public:
  static ArkUI_NativeDialogAPI_1* getInstance();
  static ArkUI_NativeDialogAPI_2* getInstance2();

 private:
  NativeDialogApi() {}
};

} // namespace rnoh
