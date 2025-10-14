/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/rncore/Props.h>
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"

namespace rnoh {

class PullToRefreshViewNapiBinder : public ViewComponentNapiBinder {
 public:
  napi_value createProps(
      napi_env env,
      facebook::react::ShadowView const shadowView) override {
    ArkJS arkJs(env);
    auto pullToRefreshViewProps = std::dynamic_pointer_cast<
        const facebook::react::PullToRefreshViewProps>(shadowView.props);
    if (!pullToRefreshViewProps) {
      LOG(ERROR)
          << "PullToRefreshViewNapiBinder::createProps: props is not PullToRefreshViewProps";
      return arkJs.getUndefined();
    }

    napi_value napiBaseProps =
        ViewComponentNapiBinder::createProps(env, shadowView);
    auto propsBuilder = arkJs.getObjectBuilder(napiBaseProps);

    return propsBuilder.build();
  };
};

} // namespace rnoh
