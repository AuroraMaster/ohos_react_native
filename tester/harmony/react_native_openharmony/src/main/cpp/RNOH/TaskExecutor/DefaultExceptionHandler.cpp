/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "DefaultExceptionHandler.h"
#include <glog/logging.h>

void rnoh::defaultExceptionHandler(std::exception_ptr e) {
  try {
    std::rethrow_exception(e);
  } catch (std::exception const& e) {
    LOG(ERROR) << "Exception thrown in task";
    LOG(ERROR) << e.what();
    try {
      std::rethrow_if_nested(e);
    } catch (const std::exception& nested) {
      LOG(ERROR) << nested.what();
    }
  }
};