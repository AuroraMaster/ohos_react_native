/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include <jsinspector/InspectorInterfaces.h>
#include <napi/native_api.h>
#include <string>

namespace rnoh {

class RemoteConnection : public facebook::react::IRemoteConnection {
 public:
  RemoteConnection(napi_env env, napi_value connection);
  ~RemoteConnection() override;

  void onMessage(std::string message) override;
  void onDisconnect() override;

 private:
  napi_env m_env;
  napi_ref m_connectionRef;
};

napi_value getInspectorWrapper(napi_env env, napi_callback_info info);

} // namespace rnoh
