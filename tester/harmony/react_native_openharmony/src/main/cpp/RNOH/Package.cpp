/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/Package.h"
#include "Package.h"

using namespace rnoh;
using namespace facebook;

Package::Package(Context ctx) : m_ctx(ctx) {}

std::unique_ptr<TurboModuleFactoryDelegate>
Package::createTurboModuleFactoryDelegate() {
  return nullptr;
}

std::vector<react::ComponentDescriptorProvider>
Package::createComponentDescriptorProviders() {
  return {};
}

GlobalJSIBinders rnoh::Package::createGlobalJSIBinders() {
  return {};
}

ComponentJSIBinderByString rnoh::Package::createComponentJSIBinderByName() {
  return {};
}

ComponentNapiBinderByString rnoh::Package::createComponentNapiBinderByName() {
  return {};
}

EventEmitRequestHandlers rnoh::Package::createEventEmitRequestHandlers() {
  return {};
}

ComponentInstanceFactoryDelegate::Shared
rnoh::Package::createComponentInstanceFactoryDelegate() {
  return nullptr;
}
