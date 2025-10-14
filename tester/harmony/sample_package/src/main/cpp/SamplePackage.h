/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/Package.h"
#include "RNOH/generated/BaseReactNativeHarmonySamplePackagePackage.h"

namespace rnoh {
class SamplePackage : public BaseReactNativeHarmonySamplePackagePackage {
  using Super = BaseReactNativeHarmonySamplePackagePackage;

 public:
  SamplePackage(Package::Context ctx)
      : BaseReactNativeHarmonySamplePackagePackage(ctx) {}

  std::vector<facebook::react::ComponentDescriptorProvider>
  createComponentDescriptorProviders() override;

  std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate()
      override;

  ComponentNapiBinderByString createComponentNapiBinderByName() override;

  ComponentJSIBinderByString createComponentJSIBinderByName() override;

  std::vector<ArkTSMessageHandler::Shared> createArkTSMessageHandlers()
      override;

  ComponentInstance::Shared createComponentInstance(
      const ComponentInstance::Context& ctx) override;

  ComponentInstanceFactoryDelegate::Shared
  createComponentInstanceFactoryDelegate() override;
};
} // namespace rnoh