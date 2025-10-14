/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "hermes/executor/HermesExecutorFactory.h"
#include "RNOH/NativeLogger.h"
#include "RNOH/Performance/NativeTracing.h"
#include "hermesExecutor.h"

namespace rnoh {

std::shared_ptr<facebook::react::JSExecutorFactory> createHermesExecutorFactory(bool shouldEnableDebugger) {
    auto jsExecutorFactory = std::make_shared<facebook::react::HermesExecutorFactory>(
        // runtime installer, which is run when the runtime
        // is first initialized and provides access to the runtime
        // before the JS code is executed
        [](facebook::jsi::Runtime& rt) {
            // install `console.log` (etc.) implementation
            facebook::react::bindNativeLogger(rt, nativeLogger);
            // install tracing functions
            rnoh::setupTracing(rt);
        }
    );
    jsExecutorFactory->setEnableDebugger(shouldEnableDebugger);
    return jsExecutorFactory;
}

}