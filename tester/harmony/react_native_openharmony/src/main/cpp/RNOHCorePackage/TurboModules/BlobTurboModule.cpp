/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOHCorePackage/TurboModules/BlobTurboModule.h"

namespace rnoh {
using namespace facebook;

BlobTurboModule::BlobTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      ARK_METHOD_METADATA(getConstants, 0),
      ARK_METHOD_METADATA(release, 1),
      ARK_METHOD_METADATA(sendOverSocket, 2),
      ARK_METHOD_METADATA(addWebSocketHandler, 1),
      ARK_METHOD_METADATA(addNetworkingHandler, 0),
      ARK_METHOD_METADATA(removeWebSocketHandler, 1),
      ARK_METHOD_METADATA(createFromParts, 2)};
}

/**
 * This method is used because calling the generated method 'release' is
 * cumbersome.
 */
void BlobTurboModule::release(std::string blobId) {
  m_ctx.taskExecutor->runTask(
      m_ctx.turboModuleThread, 
      [env = m_ctx.env,
        arkTSTurboModuleRef = m_ctx.arkTSTurboModuleInstanceRef,
        name = name_,
        blobId]() {
          std::string methodName = "release";
          try {
            ArkJS arkJS(env);
            if (!arkTSTurboModuleRef) {
              return;
            }
            std::vector<napi_value> napiArgs;
            napiArgs.push_back(arkJS.createString(blobId));
            auto napiTurboModuleObject =
                arkJS.getObject(arkTSTurboModuleRef);
            napiTurboModuleObject.call(methodName, napiArgs);
          } catch (const std::exception& e) {
            LOG(ERROR) << "Exception thrown while calling " << name
                      << " TurboModule method " << methodName << ": "
                      << e.what();
          }
      });
}

} // namespace rnoh