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
#include "RNOH/RNArkTSComponentDelegate.h"
#include "RNOH/ThreadGuard.h"
#include "arkui/NodeContentHandle.h"

#ifdef C_API_ARCH
#include <arkui/native_node.h>
#include <arkui/native_node_napi.h>
#endif

namespace rnoh {

using ArkTSComponentDelegateGetter =
 	     std::function<std::optional<std::shared_ptr<RNArkTSComponentDelegate>>()>;

struct CustomComponentArkUINodeHandle {
  ArkUI_NodeHandle nodeHandle;
  NodeContentHandle nodeContent;
  folly::Function<void()> deleter;
  ArkTSComponentDelegateGetter arkTSComponentDelegateGetter;
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
    ArkJS arkJS(m_env);
    auto frameNodeFactory =
        arkJS.getObject(m_customRNComponentFrameNodeFactoryRef)
            .getProperty("frameNodeFactory");
    auto n_result =
        arkJS.getObject(frameNodeFactory)
            .call(
                "create",
                {arkJS.createInt(tag), arkJS.createString(componentName)});
    auto n_arkTsNodeHandle = arkJS.getObjectProperty(n_result, "frameNode");
    auto n_disposeCallback = arkJS.getObjectProperty(n_result, "dispose");
    auto n_getDelegateFun = arkJS.getObjectProperty(n_result, "getDelegate");
    auto n_disposeRef = arkJS.createNapiRef(n_disposeCallback);
    auto n_delegateGetterRef = arkJS.createReference(n_getDelegateFun);
    ArkUI_NodeHandle arkTsNodeHandle = nullptr;
    auto errorCode = OH_ArkUI_GetNodeHandleFromNapiValue(
        m_env, n_arkTsNodeHandle, &arkTsNodeHandle);
    if (errorCode != 0) {
      LOG(ERROR) << "Couldn't get node handle. Error code: " << errorCode;
      return std::nullopt;
    }
    auto n_nodeContent = arkJS.getObjectProperty(n_result, "nodeContent");
    auto contentHandle = NodeContentHandle::fromNapiValue(m_env, n_nodeContent);
    return CustomComponentArkUINodeHandle{
        arkTsNodeHandle, contentHandle,
        [env = m_env, disposeRef = std::move(n_disposeRef), delegateGetterRef = std::move(n_delegateGetterRef)] {
          ArkJS arkJS(env);
          auto disposeCallback = arkJS.getReferenceValue(disposeRef);
          arkJS.call(disposeCallback, {});
          arkJS.deleteReference(delegateGetterRef);
        },
        [env = m_env, delegateGetterRef = std::move(
                          n_delegateGetterRef)]() -> std::optional<std::shared_ptr<RNArkTSComponentDelegate>> {
          ArkJS arkJS(env);
          auto getDelegateFun = arkJS.getReferenceValue(delegateGetterRef);
          auto n_result = arkJS.call(getDelegateFun, {});
          auto isUndefined = arkJS.isUndefined(n_result);
          if (isUndefined) {
            return std::nullopt;
          } else {
            return std::make_shared<RNArkTSComponentDelegate>(env, n_result);
          }
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