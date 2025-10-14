/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "NativeLogger.h"
#include <hilog/log.h>

#define LOG_DOMAIN 0xBEEF
#define LOG_TAG "#RNOH_JS"
#define LOG_PATTERN "__█__ %{public}s"

void rnoh::nativeLogger(const std::string& message, unsigned int logLevel) {
  switch (logLevel) {
    case 0:
      OH_LOG_DEBUG(LOG_APP, LOG_PATTERN, message.c_str());
      break;
    case 1:
      OH_LOG_INFO(LOG_APP, LOG_PATTERN, message.c_str());
      break;
    case 2:
      OH_LOG_WARN(LOG_APP, LOG_PATTERN, message.c_str());
      break;
    case 3:
      OH_LOG_ERROR(LOG_APP, LOG_PATTERN, message.c_str());
      break;
    default:
      OH_LOG_INFO(LOG_APP, LOG_PATTERN, message.c_str());
  }
}
