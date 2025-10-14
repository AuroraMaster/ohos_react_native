/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/LoadingProgressNode.h"
#include "react/renderer/components/rncore/ShadowNodes.h"

namespace rnoh {
class ActivityIndicatorComponentInstance
    : public CppComponentInstance<
          facebook::react::ActivityIndicatorViewShadowNode> {
 private:
  LoadingProgressNode m_loadingProgressNode;

 public:
  ActivityIndicatorComponentInstance(Context context);

  void onPropsChanged(SharedConcreteProps const& props) override;

  LoadingProgressNode& getLocalRootArkUINode() override;
};
} // namespace rnoh