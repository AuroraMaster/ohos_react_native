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
ButtonViewProps::ButtonViewProps(
    const PropsParserContext &context, 
    const ButtonViewProps &sourceProps, 
    const RawProps &rawProps): ViewProps(context, sourceProps, rawProps),

    buttonText(convertRawProp(context, rawProps, "buttonText", sourceProps.buttonText, {""}))
      {}
} // namespace react
} // namespace facebook