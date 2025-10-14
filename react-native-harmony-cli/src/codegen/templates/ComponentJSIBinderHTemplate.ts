/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import mustache from 'mustache';

const TEMPLATE = `
/**
{{#codegenNoticeLines}}
 * {{{line}}}
{{/codegenNoticeLines}}
 */

#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh {
class {{name}}JSIBinder : public ViewComponentJSIBinder {
  protected:
    facebook::jsi::Object createNativeProps(facebook::jsi::Runtime &rt) override {
        auto object = ViewComponentJSIBinder::createNativeProps(rt);
        {{#props}}
        object.setProperty(rt, "{{name}}", {{{type}}});
        {{/props}}
        return object;
    }

    facebook::jsi::Object createCommands(facebook::jsi::Runtime &rt) override {
        auto commands = ViewComponentJSIBinder::createCommands(rt);
        {{#commands}}
        commands.setProperty(rt, "{{name}}", "{{name}}");
        {{/commands}}
        return commands;
    }

    facebook::jsi::Object createBubblingEventTypes(facebook::jsi::Runtime &rt) override {
        facebook::jsi::Object events(rt);
        {{#bubblingEvents}}
        events.setProperty(rt, "{{TOPname}}", createBubblingCapturedEvent(rt, "{{name}}"));
        {{/bubblingEvents}}
        return events;
    }

    facebook::jsi::Object createDirectEventTypes(facebook::jsi::Runtime &rt) override {
        facebook::jsi::Object events(rt);
        {{#directEvents}}
        events.setProperty(rt, "{{TOPname}}", createDirectEvent(rt, "{{name}}"));
        {{/directEvents}}
        return events;
    }
};
} // namespace rnoh
`;

type ComponentJSIBinderHProp = {
  name: string;
  isObject: boolean;
};

type ComponentJSIBinderHCommand = {
  name: string;
};

type ComponentJSIBinderHEvent = {
  type: 'direct' | 'bubble';
  eventHandlerName: string;
};

export class ComponentJSIBinderHTemplate {
  private props: ComponentJSIBinderHProp[] = [];
  private commands: ComponentJSIBinderHCommand[] = [];
  private events: ComponentJSIBinderHEvent[] = [];

  constructor(
    private name: string,
    private codegenNoticeLines: string[]
  ) {}

  addProp(prop: ComponentJSIBinderHProp) {
    this.props.push(prop);
  }

  addCommand(command: ComponentJSIBinderHCommand) {
    this.commands.push(command);
  }

  addEvent(event: ComponentJSIBinderHEvent) {
    this.events.push(event);
  }

  build(): string {
    return mustache.render(TEMPLATE.trimStart(), {
      codegenNoticeLines: this.codegenNoticeLines.map((line) => ({
        line,
      })),
      name: this.name,
      props: this.props.map((prop) => ({
        name: prop.name,
        type: prop.isObject ? '"Object"' : 'true',
      })),
      commands: this.commands.map((command) => ({
        name: command.name,
      })),
      bubblingEvents: this.events
        .filter((event) => event.type === 'bubble')
        .map((event) => ({
          TOPname: event.eventHandlerName.replace('on', 'top'),
          name: event.eventHandlerName,
        })),
      directEvents: this.events
        .filter((event) => event.type === 'direct')
        .map((event) => ({
          TOPname: event.eventHandlerName.replace('on', 'top'),
          name: event.eventHandlerName,
        })),
    });
  }
}
