/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * @architecture: C-API
 */
#pragma once
#include <time.h>
#include <folly/dynamic.h>
#include <react/renderer/core/ComponentDescriptor.h>
#include <react/renderer/scheduler/SchedulerDelegate.h>
#include "RNOH/ComponentInstanceFactory.h"
#include "RNOH/ComponentInstanceRegistry.h"
#include "RNOH/ComponentInstanceProvider.h"
#include "RNOH/FeatureFlagRegistry.h"
#include "RNOH/MountingManager.h"
#include "RNOH/ShadowViewRegistry.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "RNOH/FeatureFlagRegistry.h"
#include "ArkTSChannel.h"
#include "RNOH/ArkTSChannel.h"

namespace facebook {
namespace react {
class Scheduler;
}
} // namespace facebook

namespace rnoh {

/**
 * @Thread: MAIN
 */
class MountingManagerCAPI final : public MountingManager {
  using Mutation = facebook::react::ShadowViewMutation;
  using MutationList = facebook::react::ShadowViewMutationList;

 public:
  MountingManagerCAPI(
      ComponentInstanceRegistry::Shared componentInstanceRegistry,
      ComponentInstanceFactory::Shared componentInstanceFactory,
      ComponentInstanceProvider::Shared componentInstanceProvider,
      MountingManager::Shared arkTsMountingManager,
      std::unordered_set<std::string> arkTsComponentNames,
      FeatureFlagRegistry::Shared featureFlagRegistry,
      ArkTSChannel::Shared arkTSChannel)
      : m_componentInstanceRegistry(std::move(componentInstanceRegistry)),
        m_componentInstanceFactory(std::move(componentInstanceFactory)),
        m_componentInstanceProvider(std::move(componentInstanceProvider)),
        m_arkTsMountingManager(std::move(arkTsMountingManager)),
        m_arkTsComponentNames(std::move(arkTsComponentNames)) ,
        m_featureFlagRegistry(std::move(featureFlagRegistry)),
        m_arkTSChannel(std::move(arkTSChannel)){};
    
    ~MountingManagerCAPI() {
    DLOG(INFO) << "~MountingManagerCAPI";
  }
    
  void willMount(MutationList const& mutations) override;

  void doMount(MutationList const& mutations) override;

  void didMount(MutationList const& mutations) override;
    
  void handleMutation(Mutation const& mutation);
    
  facebook::react::ShadowViewMutationList getValidMutations(
      facebook::react::ShadowViewMutationList const& mutations);
  bool isCAPIComponent(facebook::react::ShadowView const& shadowView);
    
  void dispatchCommand(
      const facebook::react::ShadowView& shadowView,
      const std::string& commandName,
      folly::dynamic const& args) override;

  void setIsJsResponder(
      const facebook::react::ShadowView& shadowView,
      bool isJsResponder,
      bool blockNativeResponder) override;

  void updateView(
      facebook::react::Tag tag,
      folly::dynamic props,
      facebook::react::ComponentDescriptor const& componentDescriptor) override;
    
  void schedulerDidSendAccessibilityEvent(
      const facebook::react::ShadowView& shadowView,
      std::string const& eventType) override;

    void clearPreallocatedViews();
    void clearPreallocatedViews(facebook::react::ShadowViewMutationList mutations);
    void clearPreallocationRequestQueue();

 private:
  void updateComponentWithShadowView(
      ComponentInstance::Shared const& componentInstance,
      facebook::react::ShadowView const& shadowView);

  void finalizeMutationUpdates(MutationList const& mutations);

  ComponentInstanceRegistry::Shared m_componentInstanceRegistry;
  ComponentInstanceFactory::Shared m_componentInstanceFactory;
  ComponentInstanceProvider::Shared m_componentInstanceProvider;
  facebook::react::ContextContainer::Shared m_contextContainer;
  MountingManager::Shared m_arkTsMountingManager;
  std::unordered_set<std::string> m_arkTsComponentNames;
  FeatureFlagRegistry::Shared m_featureFlagRegistry;
  std::unordered_set<std::string> m_cApiComponentNames;
  ArkTSChannel::Shared m_arkTSChannel;
};
} // namespace rnoh
