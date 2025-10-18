/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <functional>

#include <react/renderer/components/modal/ModalHostViewState.h>
#include <react/renderer/components/root/RootShadowNode.h>
#include <react/renderer/core/LayoutableShadowNode.h>
#include <react/renderer/core/RawProps.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <react/renderer/mounting/MountingCoordinator.h>
#include <react/renderer/mounting/TelemetryController.h>

#include "RNOH/MountingManager.h"
#include "RNOH/MutationsToNapiConverter.h"
#include "RNOH/ShadowViewRegistry.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"

namespace rnoh {

/**
 * @Thread: MAIN
 *
 * MountingManagerArkTS is responsible for relaying the UI mutations to ArkTS
 * side
 *
 */
class MountingManagerArkTS final : public MountingManager {
 public:
  using TriggerUICallback = std::function<void(
      facebook::react::ShadowViewMutationList const& mutations)>;
  using CommandDispatcher = std::function<void(
      facebook::react::Tag tag,
      std::string const& commandName,
      folly::dynamic const args)>;
  using Shared = std::shared_ptr<MountingManagerArkTS>;

  MountingManagerArkTS(
      ShadowViewRegistry::Shared shadowViewRegistry,
      TriggerUICallback triggerUICallback,
      CommandDispatcher commandDispatcher,
      ArkTSChannel::Shared arkTsChannel)
      : shadowViewRegistry(std::move(shadowViewRegistry)),
        triggerUICallback(std::move(triggerUICallback)),
        commandDispatcher(std::move(commandDispatcher)),
        m_arkTsChannel(std::move(arkTsChannel)) {}
  void willMount(MutationList const& mutations) override;

  void doMount(MutationList const& mutations) override;

  void didMount(MutationList const& mutations) override;

  void finalizeMutationUpdates(MutationList const& mutations) override;

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

    void clearPreallocatedViews();
    void clearPreallocatedViews(facebook::react::ShadowViewMutationList mutations);
    void clearPreallocationRequestQueue();

 private:
  ShadowViewRegistry::Shared shadowViewRegistry;
  TriggerUICallback triggerUICallback;
  CommandDispatcher commandDispatcher;
  ArkTSChannel::Shared m_arkTsChannel;
};

} // namespace rnoh