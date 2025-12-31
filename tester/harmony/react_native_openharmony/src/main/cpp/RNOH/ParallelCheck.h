/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
#pragma once
#include "ApiVersionCheck.h"
#include <atomic>
#include <glog/logging.h>

namespace rnoh {

class RNInstanceCAPI;

extern bool PARALLEL_RUNTIME_SWITCH;
     
inline bool GetParallelizationEnabled() {
  return PARALLEL_RUNTIME_SWITCH;
}

void SetParallelizationEnabled(bool enabled);

inline bool IsParallelizationWorkable() {
  #ifdef PARALLELIZATION_ON
    return IsAtLeastApi22() && GetParallelizationEnabled();
  #else
    return false;
  #endif
}

} // namespace rnoh
