/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
 
#pragma once

#include <cxxreact/JSExecutor.h>

namespace rnoh {
    std::shared_ptr<facebook::react::JSExecutorFactory> createHermesExecutorFactory(bool shouldEnableDebugger);
} // namespace rnoh