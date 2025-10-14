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

import {
  Descriptor as ComponentDescriptor,
  ViewBaseProps,
  ViewRawProps,
  ViewDescriptorWrapperBase,
  ColorValue,
  Color,
  RNInstance,
  Tag,
  RNComponentCommandReceiver,
  ViewPropsSelector,
} from '{{{rnohImport}}}';


export namespace {{name}} {
  export const NAME = "{{name}}" as const

  export interface DirectRawProps {
    {{#props}}
    {{name}}{{optionalityMarker}}: {{{type}}};
    {{/props}}
  }
  
  export interface Props extends ViewBaseProps {}
  
  export interface State {}
  
  export interface RawProps extends ViewRawProps, DirectRawProps {}
  
  export class PropsSelector extends ViewPropsSelector<Props, RawProps> {
    {{#simpleProps}}
    get {{name}}() {
      return this.rawProps.{{name}}{{{orDefaultValue}}};
    }
    
    {{/simpleProps}}
  
    {{#complexProps}}
    get {{name}}() {
{{{implementation}}}
    }
    
    {{/complexProps}}
  }

  export type Descriptor = ComponentDescriptor<
    typeof NAME,
    Props,
    State,
    RawProps
  >;
  
  export class DescriptorWrapper extends ViewDescriptorWrapperBase<
    typeof NAME,
    Props,
    State,
    RawProps,
    PropsSelector
  > {
    protected createPropsSelector() {
      return new PropsSelector(this.descriptor.props, this.descriptor.rawProps)
    }
  }
  
  export interface EventPayloadByName {
    {{#events}}
    "{{name}}": {{{payloadTSType}}}
    {{/events}}
  }
  
  export class EventEmitter {
    constructor(private rnInstance: RNInstance, private tag: Tag) {}
    
    emit<TEventName extends keyof EventPayloadByName>(eventName: TEventName, payload: EventPayloadByName[TEventName]) {
      this.rnInstance.emitComponentEvent(this.tag, eventName, payload)
    }
  }
  
  export interface CommandArgvByName {
    {{#commands}}
    "{{name}}": {{{argsTSType}}}
    {{/commands}}
  }
  
  export class CommandReceiver {
    private listenersByCommandName = new Map<string, Set<(...args: any[]) => void>>()
    private cleanUp: (() => void) | undefined = undefined
  
    constructor(private componentCommandReceiver: RNComponentCommandReceiver, private tag: Tag) {
    }
  
    subscribe<TCommandName extends keyof CommandArgvByName>(commandName: TCommandName, listener: (argv: CommandArgvByName[TCommandName]) => void) {
      if (!this.listenersByCommandName.has(commandName)) {
        this.listenersByCommandName.set(commandName, new Set())
      }
      this.listenersByCommandName.get(commandName)!.add(listener)
      const hasRegisteredCommandReceiver = !!this.cleanUp
      if (!hasRegisteredCommandReceiver) {
        this.cleanUp = this.componentCommandReceiver.registerCommandCallback(this.tag, (commandName: string, argv: any[]) => {
          if (this.listenersByCommandName.has(commandName)) {
            const listeners = this.listenersByCommandName.get(commandName)!
            listeners.forEach(listener => {
              listener(argv)
            })
          }
        })
      }
  
      return () => {
        this.listenersByCommandName.get(commandName)?.delete(listener)
        if (this.listenersByCommandName.get(commandName)?.size ?? 0 === 0) {
          this.listenersByCommandName.delete(commandName)
        }
        if (this.listenersByCommandName.size === 0) {
          this.cleanUp?.()
        }
      }
    }
  }

}
`;

const COLOR_VALUE_GETTER_TEMPLATE = `        if (this.rawProps.{{name}}) {
          return Color.fromColorValue(this.rawProps.{{name}})
        } else {
          return new Color({ r: 0, g: 0, b: 0, a: 255})
        }`;

type Prop = {
  name: string;
  tsType: string;
  defaultValue: string | null;
  isOptional: boolean;
};

type Event = {
  name: string;
  payloadTSType: string;
};

type Command = {
  name: string;
  argsTSType: string;
};

export class ComponentUtilsTSTemplate {
  private props: Prop[] = [];
  private events: Event[] = [];
  private commands: Command[] = [];
  private getterTemplateByTSType = new Map<string, string>();

  constructor(
    private name: string,
    private codegenNoticeLines: string[],
    private rnohImport: string
  ) {
    this.getterTemplateByTSType.set('ColorValue', COLOR_VALUE_GETTER_TEMPLATE);
  }

  addProp(prop: Prop) {
    this.props.push(prop);
  }

  addEvent(event: Event) {
    this.events.push(event);
  }

  addCommand(command: Command) {
    this.commands.push(command);
  }

  build(): string {
    return mustache.render(TEMPLATE.trimStart(), {
      codegenNoticeLines: this.codegenNoticeLines.map((line) => ({
        line,
      })),
      name: this.name,
      rnohImport: this.rnohImport,
      props: this.props.map((prop) => ({
        name: prop.name,
        type: prop.tsType,
        optionalityMarker: prop.isOptional ? '?' : '',
      })),
      events: this.events,
      commands: this.commands,
      simpleProps: this.props
        .filter((prop) => !this.getterTemplateByTSType.has(prop.tsType))
        .map((prop) => ({
          name: prop.name,
          type: prop.tsType,
          orDefaultValue: prop.defaultValue ? ` ?? ${prop.defaultValue}` : '',
        })),
      complexProps: this.props
        .filter((prop) => this.getterTemplateByTSType.has(prop.tsType))
        .map((prop) => ({
          name: prop.name,
          implementation: mustache.render(
            this.getterTemplateByTSType.get(prop.tsType)!,
            { name: prop.name },
            {}
          ),
        })),
    });
  }
}
