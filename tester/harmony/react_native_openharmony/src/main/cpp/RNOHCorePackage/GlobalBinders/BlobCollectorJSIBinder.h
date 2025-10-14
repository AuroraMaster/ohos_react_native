/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "RNOH/Assert.h"
#include "RNOH/GlobalJSIBinder.h"
#include "RNOH/TurboModuleProvider.h"
#include "RNOHCorePackage/GlobalBinders/BlobCollector.h"
#include "RNOHCorePackage/TurboModules/BlobTurboModule.h"

namespace rnoh {
class BlobCollectorJSIBinder : public GlobalJSIBinder {
 public:
  BlobCollectorJSIBinder(GlobalJSIBinder::Context ctx) : GlobalJSIBinder(ctx) {}
  void createBindings(
      facebook::jsi::Runtime& rt,
      std::shared_ptr<TurboModuleProvider> tmProvider) override {
        std::weak_ptr<BlobTurboModule> weakBlobTM =
            std::dynamic_pointer_cast<BlobTurboModule>(
                tmProvider->getTurboModule("BlobModule"));
        BlobCollector::install(rt, std::move(weakBlobTM));
  }
};
} // namespace rnoh