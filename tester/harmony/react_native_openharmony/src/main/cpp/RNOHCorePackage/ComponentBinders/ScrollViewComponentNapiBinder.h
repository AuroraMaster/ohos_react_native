/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/scrollview/ScrollViewProps.h>
#include <react/renderer/components/scrollview/ScrollViewState.h>
#include <react/renderer/core/ConcreteState.h>
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"

namespace rnoh {

class ScrollViewComponentNapiBinder : public ViewComponentNapiBinder {
 public:
  napi_value createProps(
      napi_env env,
      facebook::react::ShadowView const shadowView) override {
    napi_value napiViewProps =
        ViewComponentNapiBinder::createProps(env, shadowView);
    if (auto props =
            std::dynamic_pointer_cast<const facebook::react::ScrollViewProps>(
                shadowView.props)) {
      auto rawProps = shadowView.props->rawProps;

      // default values for properties from rawProps which can be missing
      auto persistentScrollbar = false;
      auto flexDirection = 1;

      if (rawProps.count("persistentScrollbar") > 0) {
        persistentScrollbar = rawProps["persistentScrollbar"].asBool();
      }
      if (rawProps.count("horizontal") > 0) {
        flexDirection = rawProps["horizontal"].asBool() ? 2 : 1;
      }

      ArkJS(env)
          .getObjectBuilder(napiViewProps)
          .addProperty("contentOffsetX", props->contentOffset.x)
          .addProperty("contentOffsetY", props->contentOffset.y)
          .addProperty("scrollEnabled", props->scrollEnabled)
          .addProperty("alwaysBounceHorizontal", props->alwaysBounceHorizontal)
          .addProperty("horizontal", props->horizontal)
          .addProperty("alwaysBounceVertical", props->alwaysBounceVertical)
          .addProperty("bounces", props->bounces)
          .addProperty(
              "showsHorizontalScrollIndicator",
              props->showsHorizontalScrollIndicator)
          .addProperty(
              "showsVerticalScrollIndicator",
              props->showsVerticalScrollIndicator)
          .addProperty("flexDirection", flexDirection)
          .addProperty("persistentScrollbar", persistentScrollbar)
          .addProperty(
              "indicatorStyle", static_cast<int>(props->indicatorStyle))
          .addProperty("decelerationRate", props->decelerationRate)
          .addProperty("scrollEventThrottle", props->scrollEventThrottle)
          .addProperty("removeClippedSubviews", props->removeClippedSubviews)
          .build();
    }
    return napiViewProps;
  };

  napi_value createState(
      napi_env env,
      facebook::react::ShadowView const shadowView) override {
    napi_value napiViewState =
        ViewComponentNapiBinder::createState(env, shadowView);
    if (auto state =
            std::dynamic_pointer_cast<const facebook::react::ConcreteState<
                facebook::react::ScrollViewState>>(shadowView.state)) {
      auto data = state->getData();
      ArkJS(env)
          .getObjectBuilder(napiViewState)
          .addProperty("contentOffsetX", data.contentOffset.x)
          .addProperty("contentOffsetY", data.contentOffset.y)
          .addProperty("contentSizeWidth", data.getContentSize().width)
          .addProperty("contentSizeHeight", data.getContentSize().height)
          .build();
    }
    return napiViewState;
  };

  void updateState(StateUpdateContext const& ctx) override {
    ArkJS arkJs(ctx.env);
    auto scrollState = std::dynamic_pointer_cast<
        facebook::react::ConcreteState<facebook::react::ScrollViewState> const>(
        ctx.state);
    if (scrollState == nullptr) {
      return;
    }

    auto contentOffsetX =
        arkJs.getObjectProperty(ctx.newState, "contentOffsetX");
    auto contentOffsetY =
        arkJs.getObjectProperty(ctx.newState, "contentOffsetY");
    auto contentOffset = facebook::react::Point{
        (facebook::react::Float)arkJs.getDouble(contentOffsetX),
        (facebook::react::Float)arkJs.getDouble(contentOffsetY)};

    // we want to keep content size so we use update function, similar to iOS
    // implementation
    auto updateCallback =
        [contentOffset](facebook::react::ConcreteState<
                        facebook::react::ScrollViewState>::Data const& data) {
          auto newData = data;
          newData.contentOffset = contentOffset;
          return std::make_shared<facebook::react::ConcreteState<
              facebook::react::ScrollViewState>::Data const>(newData);
        };
    scrollState->updateState(updateCallback);
    return;
  };
};

} // namespace rnoh
