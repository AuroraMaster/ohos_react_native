/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "RNOH/MountingManagerArkTS.h"

#include <glog/logging.h>
#include "MountingManagerArkTS.h"

namespace rnoh {

using namespace facebook;

void MountingManagerArkTS::willMount(MutationList const& mutations) {}

void MountingManagerArkTS::doMount(MutationList const& mutations) {
  for (auto& mutation : mutations) {
    switch (mutation.type) {
      case react::ShadowViewMutation::Create: {
        auto newChild = mutation.newChildShadowView;

        shadowViewRegistry->setShadowView(newChild.tag, newChild);
        break;
      }
      case react::ShadowViewMutation::Delete: {
        auto oldChild = mutation.oldChildShadowView;

        shadowViewRegistry->clearShadowView(oldChild.tag);
        break;
      }
      case react::ShadowViewMutation::Insert: {
        auto newChild = mutation.newChildShadowView;
        auto parent = mutation.parentShadowView;
        break;
      }
      case react::ShadowViewMutation::Remove: {
        auto oldChild = mutation.oldChildShadowView;
        auto parent = mutation.parentShadowView;
        break;
      }
      case react::ShadowViewMutation::Update: {
        auto newChild = mutation.newChildShadowView;

        shadowViewRegistry->setShadowView(newChild.tag, newChild);
        break;
      }
    }
  }
}

void MountingManagerArkTS::didMount(MutationList const& mutations) {
  if (mutations.size() > 0) {
    this->triggerUICallback(mutations);
  }
}

void MountingManagerArkTS::finalizeMutationUpdates(MutationList const& mutations) {}

void MountingManagerArkTS::dispatchCommand(
    const facebook::react::ShadowView& shadowView,
    const std::string& commandName,
    folly::dynamic const& args) {
  this->commandDispatcher(shadowView.tag, commandName, args);
}

void MountingManagerArkTS::setIsJsResponder(
    const facebook::react::ShadowView& shadowView,
    bool isJsResponder,
    bool blockNativeResponder) {
  folly::dynamic payload = folly::dynamic::object;
  payload["tag"] = shadowView.tag;
  payload["isJSResponder"] = isJsResponder;
  payload["blockNativeResponder"] = blockNativeResponder;
  m_arkTsChannel->postMessage("SCHEDULER_DID_SET_IS_JS_RESPONDER", payload);
}

void MountingManagerArkTS::updateView(
    facebook::react::Tag tag,
    folly::dynamic props,
    facebook::react::ComponentDescriptor const& componentDescriptor) {
  throw RNOHError("updateView is not implemented in ArkTS architecture.");
}

void MountingManagerArkTS::clearPreallocatedViews() {
  throw RNOHError("Preallocation is not implemented in ArkTS architecture.");
}

void MountingManagerArkTS::clearPreallocatedViews(facebook::react::ShadowViewMutationList mutations) {
  throw RNOHError("Preallocation is not implemented in ArkTS architecture.");
}

void MountingManagerArkTS::clearPreallocationRequestQueue()
{
    throw RNOHError("Preallocation is not implemented in ArkTS architecture.");
}
} // namespace rnoh