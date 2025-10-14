/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>
#include <react/renderer/core/ConcreteComponentDescriptor.h>
#include <react/renderer/core/propsConversions.h>

namespace facebook {
namespace react {

inline const char SampleViewComponentName[] = "SampleView";

class SampleViewProps : public ViewProps {
 public:
  SampleViewProps() = default;

  SampleViewProps(
      const PropsParserContext& context,
      const SampleViewProps& sourceProps,
      const RawProps& rawProps)
      : ViewProps(context, sourceProps, rawProps),
        textColor(
            CoreFeatures::enablePropIteratorSetter ? sourceProps.textColor
                                                   : convertRawProp(
                                                         context,
                                                         rawProps,
                                                         "textColor",
                                                         sourceProps.textColor,
                                                         {})) {}

  SharedColor textColor{};
};

class SampleViewEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  void onSampleClick() const {
    dispatchEvent("sampleClick", [](jsi::Runtime& runtime) {
      auto payload = jsi::Object(runtime);
      return payload;
    });
  }
};

using SampleViewShadowNode = ConcreteViewShadowNode<
    SampleViewComponentName,
    SampleViewProps,
    SampleViewEventEmitter>;

class SampleViewComponentDescriptor final
    : public ConcreteComponentDescriptor<SampleViewShadowNode> {
 public:
  SampleViewComponentDescriptor(ComponentDescriptorParameters const& parameters)
      : ConcreteComponentDescriptor(parameters) {}
};

} // namespace react
} // namespace facebook
