/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "DynamicArkUILoader.h"

namespace rnoh {

void* DynamicArkUILoader::aceNdkHandle = nullptr;
int32_t (*DynamicArkUILoader::OH_ArkUI_RunTaskInScopeFun)(
    ArkUI_ContextHandle,
    void*,
    void (*)(void*)) = nullptr;
ArkUI_TextChangeEvent* (
    *DynamicArkUILoader::OH_ArkUI_NodeEvent_GetTextChangeEventFun)(
    ArkUI_NodeEvent* event) = nullptr;

bool DynamicArkUILoader::initialized = false;
std::once_flag DynamicArkUILoader::initFlag;

bool DynamicArkUILoader::init() {
  std::call_once(initFlag, []() {
    aceNdkHandle = dlopen("libace_ndk.z.so", RTLD_LAZY);
    if (!aceNdkHandle) {
      LOG(ERROR) << "Failed to load libace_ndk.z.so: " << dlerror();
      return;
    }
    auto func = reinterpret_cast<decltype(OH_ArkUI_RunTaskInScopeFun)>(
        dlsym(aceNdkHandle, "OH_ArkUI_RunTaskInScope"));
    auto func1 =
        reinterpret_cast<decltype(OH_ArkUI_NodeEvent_GetTextChangeEventFun)>(
            dlsym(aceNdkHandle, "OH_ArkUI_NodeEvent_GetTextChangeEvent"));
    if (!func) {
      LOG(ERROR) << "Failed to get OH_ArkUI_RunTaskInScope: " << dlerror();
      dlclose(aceNdkHandle);
      aceNdkHandle = nullptr;
      return;
    }
    if (!func1) {
      LOG(ERROR) << "Failed to get OH_ArkUI_RunTaskInScope: " << dlerror();
      dlclose(aceNdkHandle);
      aceNdkHandle = nullptr;
      return;
    }
    OH_ArkUI_RunTaskInScopeFun = func;
    OH_ArkUI_NodeEvent_GetTextChangeEventFun = func1;
    initialized = true;
  });
  return initialized;
}

void DynamicArkUILoader::cleanup() {
  if (aceNdkHandle) {
    dlclose(aceNdkHandle);
    aceNdkHandle = nullptr;
    OH_ArkUI_RunTaskInScopeFun = nullptr;
    OH_ArkUI_NodeEvent_GetTextChangeEventFun = nullptr;
    initialized = false;
  }
}

bool DynamicArkUILoader::isAvailable() {
  return initialized && OH_ArkUI_RunTaskInScopeFun != nullptr;
}

int32_t (*DynamicArkUILoader::getRunTaskFunction())(
    ArkUI_ContextHandle,
    void*,
    void (*)(void*)) {
  return OH_ArkUI_RunTaskInScopeFun;
}

ArkUI_TextChangeEvent* (*DynamicArkUILoader::getTextChangeEventFun())(
    ArkUI_NodeEvent* event) {
  return OH_ArkUI_NodeEvent_GetTextChangeEventFun;
}

} // namespace rnoh