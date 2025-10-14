/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/core/ConcreteComponentDescriptor.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>
#include "MarqueeViewEventEmitter.h"
#include "Props.h"

namespace facebook {
namespace react {

extern const char MarqueeViewComponentName[] = "MarqueeView";

  using MarqueeViewShadowNode = ConcreteViewShadowNode<MarqueeViewComponentName, MarqueeViewProps, MarqueeViewEventEmitter>;
  using MarqueeViewComponentDescriptor = ConcreteComponentDescriptor<MarqueeViewShadowNode>;

} // namespace react
} // namespace facebook
