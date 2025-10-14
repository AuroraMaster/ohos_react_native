/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/image/ImageProps.h>
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"

namespace rnoh {

class ImageComponentNapiBinder : public ViewComponentNapiBinder {
 public:
  napi_value createProps(
      napi_env env,
      facebook::react::ShadowView const shadowView) override {
    napi_value napiViewProps =
        ViewComponentNapiBinder::createProps(env, shadowView);
    if (auto props =
            std::dynamic_pointer_cast<const facebook::react::ImageProps>(
                shadowView.props)) {
      facebook::react::ImageSource imageSource;
      if (props->sources.size() > 0) {
        imageSource = props->sources[0];
        ArkJS arkJs(env);
        auto propsObjBuilder = arkJs.getObjectBuilder(napiViewProps);
        if (props->defaultSources.size() > 0) {
          facebook::react::ImageSource defaultSource = props->defaultSources[0];
          propsObjBuilder.addProperty("defaultSource", defaultSource.uri);
        }

        auto rawProps = props->rawProps;
        auto loadingIndicatorSrc = rawProps["loadingIndicatorSource"];

        return propsObjBuilder.addProperty("uri", imageSource.uri)
            .addProperty("resizeMode", static_cast<int>(props->resizeMode))
            .addProperty("tintColor", props->tintColor)
            .addProperty("blurRadius", props->blurRadius)
            .addProperty("loadingIndicatorSource", loadingIndicatorSrc)
            .build();
      }
    }
    return napiViewProps;
  };
};

} // namespace rnoh
