/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/mounting/ShadowViewMutation.h>

#include "RNOH/ArkJS.h"

namespace rnoh {

class ComponentNapiBinder {
 public:
  struct StateUpdateContext {
    napi_env env;
    facebook::react::State::Shared state;
    napi_value newState;
  };

  virtual napi_value createProps(
      napi_env env,
      facebook::react::ShadowView const shadowView) {
    return ArkJS(env).createObjectBuilder().build();
  };
  virtual napi_value createState(
      napi_env env,
      facebook::react::ShadowView const shadowView) {
    return ArkJS(env).createObjectBuilder().build();
  };
  virtual void updateState(ComponentNapiBinder::StateUpdateContext const& ctx) {
    return;
  };
};

using ComponentNapiBinderByString =
    std::unordered_map<std::string, std::shared_ptr<ComponentNapiBinder>>;

class MutationsToNapiConverter {
 public:
  using Shared = std::shared_ptr<MutationsToNapiConverter>;

  MutationsToNapiConverter(
      ComponentNapiBinderByString componentNapiBinderByName);

  napi_value convert(
      napi_env env,
      facebook::react::ShadowViewMutationList const& mutations) const;

  void updateState(
      napi_env env,
      std::string const& componentName,
      facebook::react::State::Shared const& state,
      napi_value newState);

 private:
  napi_value convertShadowView(
      napi_env env,
      facebook::react::ShadowView const shadowView) const;

  ComponentNapiBinderByString m_componentNapiBinderByName;
};

} // namespace rnoh