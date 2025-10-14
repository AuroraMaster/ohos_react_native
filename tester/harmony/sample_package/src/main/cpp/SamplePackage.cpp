/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "SamplePackage.h"
#include "CodegenLibCppSampleComponentInstance.h"
#include "GeneratedSampleViewComponentInstance.h"
#include "NativeCxxModuleExampleCxxSpec.h"
#include "PropsDisplayerComponentDescriptor.h"
#include "RNOH/RNInstanceCAPI.h"
#include "RNOH/generated/BaseReactNativeHarmonySamplePackagePackage.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"
#include "SampleTurboModuleSpec.h"
#include "SampleViewComponentDescriptor.h"
#include "SampleViewComponentInstance.h"
#include "SimpleTextComponentInstance.h"

using namespace facebook;

class SampleViewJSIBinder : public ViewComponentJSIBinder {
 protected:
  facebook::jsi::Object createNativeProps(facebook::jsi::Runtime& rt) override {
    auto nativeProps = ViewComponentJSIBinder::createNativeProps(rt);
    nativeProps.setProperty(rt, "size", "true");
    nativeProps.setProperty(rt, "textColor", "true");
    return nativeProps;
  }

  facebook::jsi::Object createDirectEventTypes(
      facebook::jsi::Runtime& rt) override {
    facebook::jsi::Object events =
        ViewComponentJSIBinder::createDirectEventTypes(rt);
    events.setProperty(
        rt, "topSampleClick", createDirectEvent(rt, "onSampleClick"));
    return events;
  }
};

class SampleTurboModuleFactoryDelegate
    : public BaseReactNativeHarmonySamplePackagePackageTurboModuleFactoryDelegate {
  using Super =
      BaseReactNativeHarmonySamplePackagePackageTurboModuleFactoryDelegate;

 public:
  SharedTurboModule createTurboModule(Context ctx, const std::string& name)
      const override {
    auto tm = Super::createTurboModule(ctx, name);
    if (tm != nullptr) {
      return tm;
    }
    if (name == "SampleTurboModule") {
      return std::make_shared<NativeSampleTurboModuleSpecJSI>(ctx, name);
    }
    if (name == "NativeCxxModuleExampleCxx") {
      return std::make_shared<NativeCxxModuleExampleCxxSpecJSI>(ctx, name);
    }
    return nullptr;
  };
};

std::unique_ptr<TurboModuleFactoryDelegate>
SamplePackage::createTurboModuleFactoryDelegate() {
  return std::make_unique<SampleTurboModuleFactoryDelegate>();
}

std::vector<react::ComponentDescriptorProvider>
SamplePackage::createComponentDescriptorProviders() {
  auto componentDescriptorProviders =
      Super::createComponentDescriptorProviders();
  componentDescriptorProviders.push_back(
      react::concreteComponentDescriptorProvider<
          react::SampleViewComponentDescriptor>());
  componentDescriptorProviders.push_back(
      react::concreteComponentDescriptorProvider<
          react::PropsDisplayerComponentDescriptor>());
  return componentDescriptorProviders;
}

ComponentNapiBinderByString SamplePackage::createComponentNapiBinderByName() {
  return {
      {"PropsDisplayer", std::make_shared<ViewComponentNapiBinder>()},
  };
};

ComponentJSIBinderByString SamplePackage::createComponentJSIBinderByName() {
  auto result = Super::createComponentJSIBinderByName();
  result["SampleView"] = std::make_shared<SampleViewJSIBinder>();
  return result;
}

class SampleArkTSMessageHandler : public ArkTSMessageHandler {
  void handleArkTSMessage(const Context& ctx) override {
    auto rnInstance = ctx.rnInstance.lock();
    if (rnInstance) {
      if (ctx.messageName == "SAMPLE_MESSAGE") {
        rnInstance->postMessageToArkTS(ctx.messageName, ctx.messagePayload);
      } else if (ctx.messageName == "BLOCK_NATIVE_RESPONDER") {
        auto rnInstanceCAPI =
            std::dynamic_pointer_cast<RNInstanceCAPI>(rnInstance);
        auto maybeTag = rnInstanceCAPI->findComponentInstanceTagById(
            ctx.messagePayload["componentInstanceId"].asString());
        if (maybeTag.has_value()) {
          auto componentInstance =
              rnInstanceCAPI->findComponentInstanceByTag(maybeTag.value());
          componentInstance->setNativeResponderBlocked(
              ctx.messagePayload["isBlocked"].asBool(),
              ctx.messagePayload["origin"].asString());
        }
      } else if (ctx.messageName == "SAMPLE_WORKER_MESSAGE") {
        rnInstance
            ->getTurboModule<SampleWorkerTurboModule>("SampleWorkerTurboModule")
            ->callSync("onInvokeFromCpp", {ctx.messagePayload});
      }
    }
  };
};

std::vector<ArkTSMessageHandler::Shared>
SamplePackage::createArkTSMessageHandlers() {
  return {std::make_shared<SampleArkTSMessageHandler>()};
}

/**
 * Deprecated approach. Use `createComponentInstance`. We keep this approach
 * here for testing purposes.
 */
ComponentInstanceFactoryDelegate::Shared
SamplePackage::createComponentInstanceFactoryDelegate() {
  class SampleComponentInstanceFactoryDelegate
      : public ComponentInstanceFactoryDelegate {
   public:
    ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
      if (ctx.componentName == "SampleView") {
        return std::make_shared<SampleViewComponentInstance>(ctx);
      }
      return nullptr;
    }
  };
  return std::make_shared<SampleComponentInstanceFactoryDelegate>();
};

ComponentInstance::Shared SamplePackage::createComponentInstance(
    const ComponentInstance::Context& ctx) {
  if (ctx.componentName == "GeneratedSampleView") {
    return std::make_shared<GeneratedSampleViewComponentInstance>(ctx);
  } else if (ctx.componentName == "SimpleText") {
    return std::make_shared<SimpleTextComponentInstance>(ctx);
  } else if (ctx.componentName == "CodegenLibCppSampleComponent") {
    return std::make_shared<CodegenLibCppSampleComponentInstance>(ctx);
  }
  return nullptr;
};