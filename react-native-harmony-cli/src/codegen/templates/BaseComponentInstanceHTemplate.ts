/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import mustache from 'mustache';
import Case from 'case';
import { TypeAnnotation } from '../core';

const TEMPLATE = `
/**
{{#codegenNoticeLines}}
 * {{{line}}}
{{/codegenNoticeLines}}
 */

#pragma once

#include "RNOH/CppComponentInstance.h"
#include <react/renderer/components/{{libraryName}}/ShadowNodes.h>

namespace rnoh {
  class Base{{name}}ComponentInstance
      : public CppComponentInstance<facebook::react::{{name}}ShadowNode> {
  public:
    using CppComponentInstance::CppComponentInstance;

    void handleCommand(std::string const &commandName, folly::dynamic const &args) override {
      {{#commands}}
      if (commandName == "{{commandName}}") {
        this->on{{CommandName}}Command({{argsFromDynamic}});
      }
      {{/commands}}
    }
    
    {{#commands}}
    virtual void on{{CommandName}}Command({{argsDeclaration}}) = 0;
    {{/commands}}
  };
} // namespace rnoh
`;

type BaseComponentInstanceHCommand = {
  name: string;
  args: { name: string; typeAnnotation: TypeAnnotation }[];
};

export class BaseComponentInstanceHTemplate {
  private commands: BaseComponentInstanceHCommand[] = [];

  constructor(
    private componentName: string,
    private libraryName: string,
    private codegenNoticeLines: string[]
  ) {}

  addCommand(command: BaseComponentInstanceHCommand) {
    this.commands.push(command);
  }

  build(): string {
    return mustache.render(TEMPLATE, {
      codegenNoticeLines: this.codegenNoticeLines.map((line) => ({
        line,
      })),
      name: this.componentName,
      libraryName: this.libraryName,
      commands: this.commands.map(({ name, args }) => {
        return {
          commandName: name,
          CommandName: Case.pascal(name),
          argsDeclaration: args
            .map(
              ({ name, typeAnnotation }) =>
                `${cppTypeFromTypeAnnotation(typeAnnotation)} ${name}`
            )
            .join(', '),
          argsFromDynamic: args
            .map(
              ({ typeAnnotation }, index) =>
                `args[${index}]${dynamicExtractorFromTypeAnnotation(
                  typeAnnotation
                )}`
            )
            .join(', '),
        };
      }),
    });
  }
}

function cppTypeFromTypeAnnotation(typeAnnotation: TypeAnnotation): string {
  switch (typeAnnotation.type) {
    case 'BooleanTypeAnnotation':
      return 'bool';
    case 'Int32TypeAnnotation':
      return 'int';
    case 'FloatTypeAnnotation':
      return 'float';
    case 'DoubleTypeAnnotation':
      return 'double';
    case 'StringTypeAnnotation':
      return 'std::string';
    default:
      return 'folly::dynamic';
  }
}

function dynamicExtractorFromTypeAnnotation(
  typeAnnotation: TypeAnnotation
): string {
  switch (typeAnnotation.type) {
    case 'BooleanTypeAnnotation':
      return '.asBool()';
    case 'Int32TypeAnnotation':
      return '.asInt()';
    case 'FloatTypeAnnotation':
      return '.asDouble()';
    case 'DoubleTypeAnnotation':
      return '.asDouble()';
    case 'StringTypeAnnotation':
      return '.asString()';
    default:
      return '';
  }
}
