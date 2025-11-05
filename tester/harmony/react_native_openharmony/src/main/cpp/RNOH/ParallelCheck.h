/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
#pragma once
#include "ApiVersionCheck.h"

namespace rnoh {

/**
 * @ThreadSafe
 * @internal
 * @brief Check if parallelization optimization is enabled by user and API level >= 21
 *
 * @return true if API level >= 21 && user set PARALLEL_OPTIMIZATION_ENABLE, false otherwise.
 */
inline bool IsParallelizationWorkable() {
  #ifdef PARALLELIZATION_ON
    return IsAtLeastApi21();
  #else
    return false;
  #endif
}

} // namespace rnoh
