/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "SampleTurboModulePackage.h"
#include "./Fabric/MarqueeViewEventEmitRequestHandler.h"
#include "./Fabric/MarqueeViewNapiBinder.h"
#include "./Fabric/MarqueeViewJSIBinder.h"
#include "./Fabric/MarqueeViewComponentDescriptor.h"
#include "./ButtonView/ButtonViewComponentInstance.h"
#include "./ButtonView/ButtonViewJSIBinder.h"
#include "./TurboModules/SampleTurboModuleSpec.h"
#include "./TurboModules/SampleTurboModule2Spec.h"

using namespace rnoh;
using namespace facebook;

class SampleTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
    
    public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "SampleTurboModule") {
            return std::make_shared<NativeSampleTurboModuleSpecJSI>(ctx, name);
        } else if (name == "SampleTurboModule2") {
            return std::make_shared<NativeSampleTurboModule2SpecJSI>(ctx, name);    
        }
        return nullptr;
    };
};

class ButtonViewPackageComponentInstanceFactoryDelegate : public ComponentInstanceFactoryDelegate {
    public:
        using ComponentInstanceFactoryDelegate::ComponentInstanceFactoryDelegate;
    
        ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
            if (ctx.componentName == "CustomButtonView") {
                return std::make_shared<ButtonViewComponentInstance>(std::move(ctx));
            }
            return nullptr;
        }
};

EventEmitRequestHandlers SampleTurboModulePackage::createEventEmitRequestHandlers() {
    return {std::make_shared<MarqueeViewEventEmitRequestHandler>()};
}

ComponentNapiBinderByString SampleTurboModulePackage::createComponentNapiBinderByName() {
    return {
        {"PropsDisplayer", std::make_shared<ViewComponentNapiBinder>()}
    };
};

ComponentJSIBinderByString SampleTurboModulePackage::createComponentJSIBinderByName() {
    return {
        {"MarqueeView", std::make_shared<MarqueeViewJSIBinder>(),},
        {"CustomButtonView", std::make_shared<ButtonViewJSIBinder>()}
    };
};

ComponentInstanceFactoryDelegate::Shared SampleTurboModulePackage::createComponentInstanceFactoryDelegate() {
    return std::make_shared<ButtonViewPackageComponentInstanceFactoryDelegate>();
}

std::unique_ptr<TurboModuleFactoryDelegate> SampleTurboModulePackage::createTurboModuleFactoryDelegate() {
    return std::make_unique<SampleTurboModuleFactoryDelegate>();
}

std::vector<react::ComponentDescriptorProvider> SampleTurboModulePackage::createComponentDescriptorProviders() {
    return {
        react::concreteComponentDescriptorProvider<react::MarqueeViewComponentDescriptor>(),
        react::concreteComponentDescriptorProvider<react::ButtonViewComponentDescriptor>(),
    };
}
