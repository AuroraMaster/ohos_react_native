/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/componentregistry/ComponentDescriptorProvider.h>
#include "RNOH/ArkTSMessageHandler.h"
#include "RNOH/ComponentInstanceFactory.h"
#include "RNOH/EventEmitRequestHandler.h"
#include "RNOH/GlobalJSIBinder.h"
#include "RNOH/MutationsToNapiConverter.h"
#include "RNOH/ShadowViewRegistry.h"
#include "RNOH/TurboModule.h"
#include "RNOH/TurboModuleFactory.h"
#include "RNOH/UIManagerModule.h"

namespace rnoh {
class Package {
 public:
  using Shared = std::shared_ptr<Package>;

  struct Context : ComponentInstance::Context {
    ShadowViewRegistry::Shared shadowViewRegistry;
  };

  Package(Context ctx);
  virtual ~Package(){};

  virtual std::unique_ptr<TurboModuleFactoryDelegate>
  createTurboModuleFactoryDelegate();

  virtual GlobalJSIBinders createGlobalJSIBinders();

  virtual std::vector<facebook::react::ComponentDescriptorProvider>
  createComponentDescriptorProviders();

  virtual ComponentJSIBinderByString createComponentJSIBinderByName();

  virtual ComponentNapiBinderByString createComponentNapiBinderByName();

  virtual EventEmitRequestHandlers createEventEmitRequestHandlers();

  virtual std::vector<ArkTSMessageHandler::Shared>
  createArkTSMessageHandlers() {
    return {};
  };

  /**
   * @architecture: C-API
   * @deprecated: Override `createComponentInstance` instead (latestRNOHVersion:
   * 0.72.27)
   */
  virtual ComponentInstanceFactoryDelegate::Shared
  createComponentInstanceFactoryDelegate();

  /**
   * @architecture: C-API
   */
  virtual ComponentInstance::Shared createComponentInstance(
      const ComponentInstance::Context& ctx) {
    return nullptr;
  };

 protected:
  Context m_ctx;
};
} // namespace rnoh