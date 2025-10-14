/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "SampleTurboModulePackage.h"
#include "./TurboModules/SampleTurboModuleSpec.h"
#include "./TurboModules/SampleTurboModule2Spec.h"
#include "./TurboModules/SampleWorkerTurboModuleSpec.h"

using namespace rnoh;
using namespace facebook;

class SampleTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
    
    public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "SampleTurboModule") {
            return std::make_shared<NativeSampleTurboModuleSpecJSI>(ctx, name);
        } else if (name == "SampleTurboModule2") {
            return std::make_shared<NativeSampleTurboModule2SpecJSI>(ctx, name);    
        }else if (name == "SampleWorkerTurboModule"){
            return std::make_shared<NativeSampleWorkerTurboModuleSpecJSI>(ctx, name);
        }
        return nullptr;
    };
};

class ButtonViewPackageComponentInstanceFactoryDelegate : public ComponentInstanceFactoryDelegate {
    public:
        using ComponentInstanceFactoryDelegate::ComponentInstanceFactoryDelegate;
    
        ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
            return nullptr;
        }
};

std::unique_ptr<TurboModuleFactoryDelegate> SampleTurboModulePackage::createTurboModuleFactoryDelegate() {
    return std::make_unique<SampleTurboModuleFactoryDelegate>();
}
