/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <arkui/native_interface.h>
#include <arkui/native_node.h>
#include <dlfcn.h>
#include <glog/logging.h>

namespace rnoh {

class DynamicArkUILoader {
 public:
  static bool init();
  static void cleanup();
  static bool isAvailable();
  static int32_t (
      *getRunTaskFunction())(ArkUI_ContextHandle, void*, void (*)(void*));
  static ArkUI_TextChangeEvent* (*getTextChangeEventFun())(
      ArkUI_NodeEvent* event);

 private:
  static void* aceNdkHandle;
  static int32_t (
      *OH_ArkUI_RunTaskInScopeFun)(ArkUI_ContextHandle, void*, void (*)(void*));
  static ArkUI_TextChangeEvent* (*OH_ArkUI_NodeEvent_GetTextChangeEventFun)(
      ArkUI_NodeEvent* event);
  static bool initialized;
  static std::once_flag initFlag;
};

} // namespace rnoh