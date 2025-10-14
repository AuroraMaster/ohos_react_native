/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include <react/renderer/components/rncore/Props.h>
#include <react/renderer/core/PropsParserContext.h>
#include <react/renderer/core/propsConversions.h>
#include "Props.h"

namespace facebook {
namespace react {
MarqueeViewProps::MarqueeViewProps(
    const PropsParserContext &context, 
    const MarqueeViewProps &sourceProps, 
    const RawProps &rawProps): ViewProps(context, sourceProps, rawProps),

    src(convertRawProp(context, rawProps, "src", sourceProps.src, {""}))
      {}
} // namespace react
} // namespace facebook