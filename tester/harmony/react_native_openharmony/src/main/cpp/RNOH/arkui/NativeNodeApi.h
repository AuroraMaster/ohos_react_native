/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <arkui/native_interface.h>
#include <arkui/native_node.h>
#include <mutex>

namespace rnoh {

class NativeNodeApi {
 public:
  static ArkUI_NativeNodeAPI_1* getInstance();
  static void resetInstance();

  static ArkUI_NativeNodeAPI_1* INSTANCE;
  static std::mutex instanceMutex;

 private:
  NativeNodeApi() {}

  static ArkUI_NativeNodeAPI_1* api;
};

} // namespace rnoh
