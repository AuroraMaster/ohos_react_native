/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
#include "RNOH/ParallelCheck.h"
#include "RNOH/RNInstanceCAPI.h"
#include "RNOH/RNInstance.h"
#include "RNOH/arkui/NativeNodeApi.h"
#include "RNOH/arkui/ArkUINodeRegistry.h"
#include <mutex>

namespace rnoh {

bool PARALLEL_RUNTIME_SWITCH = false;

bool IS_SPECIAL_EQUIPMENT = false;

void SetParallelizationEnabled(bool enabled) {
  bool oldValue = PARALLEL_RUNTIME_SWITCH;
  PARALLEL_RUNTIME_SWITCH = enabled;
  
  // If the switch value changed and NativeNodeApi has been initialized,
  // reset the instance so it will be re-initialized with the new switch value
  if (oldValue != enabled) {
    NativeNodeApi::resetInstance();
    
    // Re-register the node event receiver if ArkUINodeRegistry has been initialized
    // This is necessary because the API instance has changed
    if (ArkUINodeRegistry::isInitialized()) {
      ArkUINodeRegistry::getInstance().reinitializeNodeEventReceiver();
    } else {
      LOG(INFO) << "SetParallelizationEnabled: ArkUINodeRegistry not initialized yet, skipping re-registration";
    }
  }
}

void setDeviceInfo(bool isSpecialEquipment) {
  IS_SPECIAL_EQUIPMENT = isSpecialEquipment;
}

} // namespace rnoh

