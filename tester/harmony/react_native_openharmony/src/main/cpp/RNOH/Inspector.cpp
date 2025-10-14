/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "Inspector.h"
#include <glog/logging.h>
#include <memory>
#include <vector>
#include "ArkJS.h"
#include "TaskExecutor/NapiTaskRunner.h"

namespace rnoh {

static std::unique_ptr<NapiTaskRunner> mainThreadTaskRunner = nullptr;

// Constructor must always be called on the main thread
RemoteConnection::RemoteConnection(napi_env env, napi_value connection)
    : m_env(env) {
  napi_create_reference(env, connection, 1, &m_connectionRef);
}

RemoteConnection::~RemoteConnection() {
  mainThreadTaskRunner->runAsyncTask(
      [env = this->m_env, ref = this->m_connectionRef] {
        ArkJS arkJs(env);
        arkJs.deleteReference(ref);
      });
}

void RemoteConnection::onMessage(std::string message) {
  mainThreadTaskRunner->runAsyncTask([env = this->m_env,
                                      ref = this->m_connectionRef,
                                      message = std::move(message)] {
    ArkJS arkJs(env);
    arkJs.getObject(ref).call<1>("onMessage", {arkJs.createString(message)});
  });
}

void RemoteConnection::onDisconnect() {
  mainThreadTaskRunner->runAsyncTask(
      [env = this->m_env, ref = this->m_connectionRef] {
        ArkJS arkJs(env);
        arkJs.getObject(ref).call<0>("onDisconnect", {});
      });
}

static napi_value wrapLocalConnection(
    napi_env env,
    std::unique_ptr<facebook::react::ILocalConnection> localConnection) {
  ArkJS arkJs(env);
  auto connectionRawPtr = localConnection.release();

  try {
    auto sendMessage = arkJs.createFunction(
        "sendMessage",
        [](auto env, auto cbInfo) {
          ArkJS arkJs(env);
          facebook::react::ILocalConnection* connection;
          size_t argc = 1;
          napi_value args[1];
          napi_get_cb_info(
              env, cbInfo, &argc, args, nullptr, (void**)&connection);
          if (argc < 1) {
            LOG(ERROR) << "sendMessage requires 1 argument";
            return arkJs.getUndefined();
          }
          auto message = arkJs.getString(args[0]);
          connection->sendMessage(message);
          return arkJs.getUndefined();
        },
        connectionRawPtr);

    auto disconnect = arkJs.createFunction(
        "disconnect",
        [](auto env, auto cbInfo) {
          facebook::react::ILocalConnection* connection;
          napi_get_cb_info(
              env, cbInfo, 0, nullptr, nullptr, (void**)&connection);
          connection->disconnect();
          delete connection;
          return ArkJS(env).getUndefined();
        },
        connectionRawPtr);

    return arkJs.createObjectBuilder()
        .addProperty("sendMessage", sendMessage)
        .addProperty("disconnect", disconnect)
        .build();
  } catch (std::exception& e) {
    LOG(ERROR) << "Failed to wrap local connection: " << e.what();
    delete connectionRawPtr;
    return arkJs.getUndefined();
  }
}

static napi_value connect(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);

  size_t argc = 2;
  napi_value args[2];
  napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  if (argc < 2) {
    LOG(ERROR) << "connect requires 2 arguments";
    return arkJs.getUndefined();
  }
  auto pageId = static_cast<int>(arkJs.getDouble(args[0]));
  auto remoteConnection = std::make_unique<RemoteConnection>(env, args[1]);
  auto localConnection = ::facebook::react::getInspectorInstance().connect(
      pageId, std::move(remoteConnection));
  return wrapLocalConnection(env, std::move(localConnection));
}

static napi_value getPages(napi_env env, napi_callback_info info) {
  ArkJS arkJs(env);
  auto pages = ::facebook::react::getInspectorInstance().getPages();
  std::vector<napi_value> pageObjects(pages.size());
  for (size_t i = 0; i < pages.size(); i++) {
    auto page = pages[i];
    auto pageObject = arkJs.createObjectBuilder()
                          .addProperty("id", arkJs.createDouble(page.id))
                          .addProperty("title", arkJs.createString(page.title))
                          .addProperty("vm", arkJs.createString(page.vm))
                          .build();
    pageObjects[i] = pageObject;
  }
  return arkJs.createArray(pageObjects);
}

napi_value getInspectorWrapper(napi_env env, napi_callback_info info) {
  if (mainThreadTaskRunner == nullptr) {
    mainThreadTaskRunner =
        std::make_unique<NapiTaskRunner>("RNOH_MAIN_INSPECTOR", env);
  }

  ArkJS arkJs(env);
  napi_property_descriptor desc[] = {
      {"getPages",
       nullptr,
       getPages,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr},
      {"connect",
       nullptr,
       connect,
       nullptr,
       nullptr,
       nullptr,
       napi_default,
       nullptr}};
  auto obj = arkJs.createObjectBuilder().build();
  napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
  return obj;
}

} // namespace rnoh
