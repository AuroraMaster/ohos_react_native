/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <arkui/native_node_napi.h>
#include <arkui/native_type.h>
#include <js_native_api.h>
#include <js_native_api_types.h>

#include "ArkUINode.h"

namespace rnoh {

class NodeContentHandle final {
 public:
  static NodeContentHandle fromNapiValue(napi_env env, napi_value value);

  NodeContentHandle& addNode(ArkUINode& node);

  NodeContentHandle& insertNode(ArkUINode& node, int32_t index);

  NodeContentHandle& removeNode(ArkUINode& node);

 private:
  NodeContentHandle(ArkUI_NodeContentHandle contentHandle);

  static void maybeThrow(int32_t status);

  ArkUI_NodeContentHandle m_contentHandle;
};

} // namespace rnoh
