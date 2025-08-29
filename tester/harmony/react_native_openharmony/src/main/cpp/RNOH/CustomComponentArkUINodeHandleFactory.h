/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <glog/logging.h>
#include <react/renderer/core/ReactPrimitives.h>
#include "RNOH/ArkJS.h"
#include "RNOH/ThreadGuard.h"
#include "arkui/NodeContentHandle.h"

#ifdef C_API_ARCH
#include <arkui/native_node.h>
#include <arkui/native_node_napi.h>
#endif

namespace rnoh {

struct CustomComponentArkUINodeHandle {
  ArkUI_NodeHandle nodeHandle;
  NodeContentHandle nodeContent;
  folly::Function<void()> deleter;
};

/**
 * @thread: MAIN
 * Used by the ComponentRegistry to create ComponentInstances written in ArkTS.
 */
class CustomComponentArkUINodeHandleFactory final {
 public:
  using Shared = std::shared_ptr<CustomComponentArkUINodeHandleFactory>;

  CustomComponentArkUINodeHandleFactory(
      napi_env env,
      NapiRef customRNComponentFrameNodeFactoryRef)
      : m_env(env),
        m_customRNComponentFrameNodeFactoryRef(
            std::move(customRNComponentFrameNodeFactoryRef)) {}

  std::optional<CustomComponentArkUINodeHandle> create(
      facebook::react::Tag tag,
      std::string componentName) {
    m_threadGuard.assertThread();
#ifdef C_API_ARCH
    ArkJS arkJs(m_env);
    auto frameNodeFactory =
        arkJs.getObject(m_customRNComponentFrameNodeFactoryRef)
            .getProperty("frameNodeFactory");
    auto n_result =
        arkJs.getObject(frameNodeFactory)
            .call(
                "create",
                {arkJs.createInt(tag), arkJs.createString(componentName)});
    auto n_arkTsNodeHandle = arkJs.getObjectProperty(n_result, "frameNode");
    auto n_disposeCallback = arkJs.getObjectProperty(n_result, "dispose");
    auto n_disposeRef = arkJs.createNapiRef(n_disposeCallback);
    ArkUI_NodeHandle arkTsNodeHandle = nullptr;
    auto errorCode = OH_ArkUI_GetNodeHandleFromNapiValue(
        m_env, n_arkTsNodeHandle, &arkTsNodeHandle);
    if (errorCode != 0) {
      LOG(ERROR) << "Couldn't get node handle. Error code: " << errorCode;
      return std::nullopt;
    }
    auto n_nodeContent = arkJs.getObjectProperty(n_result, "nodeContent");
    auto contentHandle = NodeContentHandle::fromNapiValue(m_env, n_nodeContent);
    return CustomComponentArkUINodeHandle{
        arkTsNodeHandle,
        contentHandle,
        [env = m_env, disposeRef = std::move(n_disposeRef)] {
          ArkJS arkJs(env);
          auto disposeCallback = arkJs.getReferenceValue(disposeRef);
          arkJs.call(disposeCallback, {});
        }};
#else
    return std::nullopt;
#endif
  }

 private:
  napi_env m_env;
  NapiRef m_customRNComponentFrameNodeFactoryRef;
  ThreadGuard m_threadGuard{};
};
} // namespace rnoh