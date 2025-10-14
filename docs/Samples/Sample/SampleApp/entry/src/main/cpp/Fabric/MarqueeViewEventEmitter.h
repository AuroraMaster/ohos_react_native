/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/view/ViewEventEmitter.h>
#include <jsi/jsi.h>

namespace facebook {
namespace react {

class JSI_EXPORT MarqueeViewEventEmitter : public ViewEventEmitter {
  public:
    using ViewEventEmitter::ViewEventEmitter;
    struct OnStop {
      bool isStop;
      std::string type;
    };

    void onStop(OnStop value) const;
};

} // namespace react
} // namespace facebook
