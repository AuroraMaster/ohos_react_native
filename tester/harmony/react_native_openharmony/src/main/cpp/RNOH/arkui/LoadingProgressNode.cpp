/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "LoadingProgressNode.h"
#include "NativeNodeApi.h"

namespace rnoh {
LoadingProgressNode::LoadingProgressNode(const ArkUINode::Context::Shared& context)
    : ArkUINode(context, ArkUI_NodeType::ARKUI_NODE_LOADING_PROGRESS) {}

LoadingProgressNode& LoadingProgressNode::setColor(
    facebook::react::SharedColor const& color) {
  uint32_t loadingProgressColor = *color;
  std::array<ArkUI_NumberValue, 1> loadingProgressColorValue = {
      {{.u32 = loadingProgressColor}}};

  ArkUI_AttributeItem loadingProgressColorItem = {
      loadingProgressColorValue.data(), loadingProgressColorValue.size()};

  m_nodeApi->setAttribute(
      m_nodeHandle, NODE_LOADING_PROGRESS_COLOR, &loadingProgressColorItem);
  return *this;
}

LoadingProgressNode& LoadingProgressNode::setAnimating(const bool& enable) {
  uint32_t enableLoadingProgressNodeAnimation = enable ? 1 : 0;
  std::array<ArkUI_NumberValue, 1> enableLoadingProgressNodeAnimationValue = {
      {{.u32 = enableLoadingProgressNodeAnimation}}};

  ArkUI_AttributeItem enableLoadingProgressNodeAnimationItem = {
      enableLoadingProgressNodeAnimationValue.data(),
      enableLoadingProgressNodeAnimationValue.size()};

  m_nodeApi->setAttribute(
      m_nodeHandle,
      NODE_LOADING_PROGRESS_ENABLE_LOADING,
      &enableLoadingProgressNodeAnimationItem);
  return *this;
};

} // namespace rnoh
