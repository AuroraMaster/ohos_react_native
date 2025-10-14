/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#ifndef SAMPLEAPP_SAMPLEWORKERTURBOMODULESPEC_H
#define SAMPLEAPP_SAMPLEWORKERTURBOMODULESPEC_H

#pragma once

#include <ReactCommon/TurboModule.h>
#include "RNOH/ArkTSTurboModule.h"

namespace rnoh {

    class JSI_EXPORT NativeSampleWorkerTurboModuleSpecJSI : public ArkTSTurboModule {
    public:
        NativeSampleWorkerTurboModuleSpecJSI(const ArkTSTurboModule::Context ctx, const std::string name);
    };

} // namespace rnoh

#endif //SAMPLEAPP_SAMPLEWORKERTURBOMODULESPEC_H
