/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/Package.h"

namespace rnoh {
class SampleTurboModulePackage : public Package {
    public:
        SampleTurboModulePackage(Package::Context ctx) : Package(ctx) {}
    
        std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override;

    };
} // namespace rnoh
