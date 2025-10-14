/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/MutationsToNapiConverter.h"
#include "MutationsToNapiConverter.h"
#include "RNOH/ArkJS.h"
#include "RNOH/BaseComponentNapiBinder.h"

using namespace facebook;
using namespace rnoh;

MutationsToNapiConverter::MutationsToNapiConverter(
    ComponentNapiBinderByString componentNapiBinderByName)
    : m_componentNapiBinderByName(std::move(componentNapiBinderByName)) {}

napi_value MutationsToNapiConverter::convert(
    napi_env env,
    react::ShadowViewMutationList const& mutations) const {
  std::vector<napi_value> napiMutations;
  ArkJS arkJs(env);
  for (auto& mutation : mutations) {
    auto objBuilder =
        arkJs.createObjectBuilder().addProperty("type", mutation.type);
    switch (mutation.type) {
      case react::ShadowViewMutation::Type::Create: {
        objBuilder.addProperty(
            "descriptor",
            this->convertShadowView(env, mutation.newChildShadowView));
        break;
      }
      case react::ShadowViewMutation::Type::Remove: {
        objBuilder.addProperty("childTag", mutation.oldChildShadowView.tag)
            .addProperty("parentTag", mutation.parentShadowView.tag);
        break;
      }
      case react::ShadowViewMutation::Type::Update: {
        objBuilder.addProperty(
            "descriptor",
            this->convertShadowView(env, mutation.newChildShadowView));
        break;
      }
      case react::ShadowViewMutation::Type::Insert: {
        objBuilder.addProperty("childTag", mutation.newChildShadowView.tag)
            .addProperty("parentTag", mutation.parentShadowView.tag)
            .addProperty("index", mutation.index);
        break;
      }
      case react::ShadowViewMutation::Type::Delete: {
        objBuilder.addProperty("tag", mutation.oldChildShadowView.tag);
        break;
      }
    }
    napiMutations.push_back(objBuilder.build());
  }
  return arkJs.createArray(napiMutations);
}

void rnoh::MutationsToNapiConverter::updateState(
    napi_env env,
    std::string const& componentName,
    facebook::react::State::Shared const& state,
    napi_value newState) {
  if (auto it = m_componentNapiBinderByName.find(componentName);
      it != m_componentNapiBinderByName.end()) {
    it->second->updateState({env, state, newState});
  }
  return;
}

napi_value MutationsToNapiConverter::convertShadowView(
    napi_env env,
    react::ShadowView const shadowView) const {
  ArkJS arkJs(env);
  auto descriptorBuilder = arkJs.createObjectBuilder();
  if (m_componentNapiBinderByName.count(shadowView.componentName) > 0) {
    auto componentNapiBinder =
        m_componentNapiBinderByName.at(shadowView.componentName);
    descriptorBuilder.addProperty("isDynamicBinder", arkJs.createBoolean(false))
        .addProperty("props", componentNapiBinder->createProps(env, shadowView))
        .addProperty(
            "state", componentNapiBinder->createState(env, shadowView));
  } else {
    BaseComponentNapiBinder baseNapiBinder;
    descriptorBuilder.addProperty("isDynamicBinder", arkJs.createBoolean(true))
        .addProperty("props", baseNapiBinder.createProps(env, shadowView))
        .addProperty("state", arkJs.createObjectBuilder().build());
  }
  descriptorBuilder.addProperty(
      "layoutMetrics",
      arkJs.createObjectBuilder()
          .addProperty(
              "frame",
              arkJs.createObjectBuilder()
                  .addProperty(
                      "origin",
                      arkJs.createObjectBuilder()
                          .addProperty(
                              "x", shadowView.layoutMetrics.frame.origin.x)
                          .addProperty(
                              "y", shadowView.layoutMetrics.frame.origin.y)
                          .build())
                  .addProperty(
                      "size",
                      arkJs.createObjectBuilder()
                          .addProperty(
                              "width",
                              shadowView.layoutMetrics.frame.size.width)
                          .addProperty(
                              "height",
                              shadowView.layoutMetrics.frame.size.height)
                          .build())
                  .build())
          .addProperty(
              "layoutDirection",
              static_cast<int>(shadowView.layoutMetrics.layoutDirection))
          .build());

  return descriptorBuilder.addProperty("tag", shadowView.tag)
      .addProperty("type", shadowView.componentName)
      .addProperty("childrenTags", arkJs.createArray())
      .addProperty(
          "rawProps", arkJs.createFromDynamic(shadowView.props->rawProps))
      .build();
}
