/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "NodeContentHandle.h"

#include "RNOH/RNOHError.h"

namespace rnoh {

NodeContentHandle& NodeContentHandle::removeNode(ArkUINode& node) {
  maybeThrow(OH_ArkUI_NodeContent_RemoveNode(
      m_contentHandle, node.getArkUINodeHandle()));
  return *this;
}

NodeContentHandle& NodeContentHandle::addNode(ArkUINode& node) {
  maybeThrow(
      OH_ArkUI_NodeContent_AddNode(m_contentHandle, node.getArkUINodeHandle()));
  return *this;
}

NodeContentHandle NodeContentHandle::fromNapiValue(
    napi_env env,
    napi_value value) {
  ArkUI_NodeContentHandle content;
  maybeThrow(OH_ArkUI_GetNodeContentFromNapiValue(env, value, &content));
  return {content};
}

NodeContentHandle::NodeContentHandle(ArkUI_NodeContentHandle contentHandle)
    : m_contentHandle(contentHandle) {}
void NodeContentHandle::maybeThrow(int32_t status) {
  if (status != ARKUI_ERROR_CODE_NO_ERROR) {
    throw RNOHError(
        "NodeContentHandle operation failed with code: " +
        std::to_string(status));
  }
}

NodeContentHandle& NodeContentHandle::insertNode(
    ArkUINode& node,
    int32_t index) {
  maybeThrow(OH_ArkUI_NodeContent_InsertNode(
      m_contentHandle, node.getArkUINodeHandle(), index));
  return *this;
}
} // namespace rnoh
