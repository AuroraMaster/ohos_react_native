/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "ArkTSMessageHandler.h"
#include "Subject.h"

namespace rnoh {

struct ArkTSMessage {
  std::string name;
  folly::dynamic payload;
};

class ArkTSMessageHub final : public ArkTSMessageHandler,
                              public Subject<ArkTSMessage> {
 public:
  using Shared = std::shared_ptr<ArkTSMessageHub>;
  using Weak = std::weak_ptr<ArkTSMessageHub>;

  class Observer : private Subject::Observer {
   public:
    Observer(ArkTSMessageHub::Shared const& subject)
        : Subject::Observer(std::static_pointer_cast<Subject>(subject)) {}

    virtual void onMessageReceived(ArkTSMessage const& payload) = 0;

   protected:
    void onUpdate(ArkTSMessage const& payload) final {
      onMessageReceived(payload);
    }
  };

  void handleArkTSMessage(const Context& ctx) override {
    notifyObservers({ctx.messageName, ctx.messagePayload});
  }
};

} // namespace rnoh