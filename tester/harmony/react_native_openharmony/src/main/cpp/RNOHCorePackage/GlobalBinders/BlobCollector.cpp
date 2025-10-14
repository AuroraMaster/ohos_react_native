/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */


#include "RNOHCorePackage/GlobalBinders/BlobCollector.h"
#include <jsi/jsi.h>
#include "RNOH/ArkTSTurboModule.h"

#include "RNOH/TurboModule.h"
#include "RNOHCorePackage/TurboModules/BlobTurboModule.h"

namespace rnoh {

/**
 * This is a hack to get around the fact that there's no easy way to call a
 * method in a TurboModule at the destruction of an object. Because we add the
 * BlobCollector as a HostObject to the Blob in js, it's destructor will be
 * called when the Blob is garbage collected.
 */
BlobCollector::BlobCollector(
    std::weak_ptr<BlobTurboModule> weakBlobTurboModule,
    const std::string& blobId)
    : m_weakBlobTurboModule(std::move(weakBlobTurboModule)),
    m_blobId(std::move(blobId)){};

void BlobCollector::install(
    facebook::jsi::Runtime& rt,
    std::weak_ptr<BlobTurboModule> weakBlobTurboModule) {
  rt.global().setProperty(
      rt,
      "__blobCollectorProvider",
      facebook::jsi::Function::createFromHostFunction(
          rt,
          facebook::jsi::PropNameID::forAscii(rt, "__blobCollectorProvider"),
          1,
          [weakBlobTurboModule = std::move(weakBlobTurboModule)](
              facebook::jsi::Runtime& rt,
              const facebook::jsi::Value& thisVal,
              const facebook::jsi::Value* args,
              size_t count) {
            auto blobId = args[0].asString(rt).utf8(rt);
            auto blobCollector = std::make_shared<BlobCollector>(
                std::move(weakBlobTurboModule), blobId);
            return facebook::jsi::Object::createFromHostObject(
                rt, blobCollector);
          }));
}

BlobCollector::~BlobCollector() {
  auto blobTurboModule = m_weakBlobTurboModule.lock();
  if (blobTurboModule) {
    blobTurboModule->release(m_blobId);
  }
}

} // namespace rnoh