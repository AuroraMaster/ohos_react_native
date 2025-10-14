/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "JSVMExecutorFactory.h"
#include "RNOH/NativeLogger.h"
#include "RNOH/Performance/NativeTracing.h"
#include "jsvmExecutor.h"

namespace rnoh {

std::shared_ptr<facebook::react::JSExecutorFactory> createJSVMExecutorFactory(folly::dynamic initOptions) {
    return std::make_shared<rnjsvm::JSVMExecutorFactory>(
        [](facebook::jsi::Runtime& rt) {
            facebook::react::bindNativeLogger(rt, nativeLogger);
            rnoh::setupTracing(rt);
        },
        initOptions
    );
}

}