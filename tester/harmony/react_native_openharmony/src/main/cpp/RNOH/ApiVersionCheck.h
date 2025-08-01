/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
#pragma once

#include <deviceinfo.h>
#include <mutex>

namespace rnoh {

constexpr int API_LEVEL_20 = 20;

/**
 * @brief Lazily check if API level is at least the given level.
 *
 * @tparam ApiLevel Minimum API level to check.
 * @return true if OH_GetSdkApiVersion() >= ApiLevel, false otherwise.
 */
template <int ApiLevel>
inline bool IsAtLeastApi() {
  static std::once_flag flag;
  static bool cachedResult = false;
  std::call_once(
      flag, [] { cachedResult = OH_GetSdkApiVersion() >= ApiLevel; });
  return cachedResult;
}

/**
 * @ThreadSafe
 *
 * Check if current API level is at least 20.
 * @return true if API level >= 20, false otherwise.
 */
inline bool IsAtLeastApi20() {
  return IsAtLeastApi<API_LEVEL_20>();
}
} // namespace rnoh
