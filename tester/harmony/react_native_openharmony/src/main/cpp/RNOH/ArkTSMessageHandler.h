/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <folly/dynamic.h>
#include <string>

namespace rnoh {
class RNInstance;

/**
 * Provides a mechanism to respond to messages sent from the ArkTS side.
 * Messages are sent within the context of an RNInstance.
 */
class ArkTSMessageHandler {
 public:
  using Shared = std::shared_ptr<ArkTSMessageHandler>;

  struct Context {
    std::string messageName;
    folly::dynamic messagePayload;
    std::weak_ptr<RNInstance> rnInstance;
  };

  /**
   * This method is executed on MAIN thread.
   */
  virtual void handleArkTSMessage(const Context& ctx) = 0;
};
} // namespace rnoh