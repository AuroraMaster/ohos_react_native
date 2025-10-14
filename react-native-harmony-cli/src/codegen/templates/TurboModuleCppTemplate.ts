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

#include "{{className}}.h"

namespace rnoh {
using namespace facebook;

{{className}}::{{className}}(const ArkTSTurboModule::Context ctx, const std::string name) : ArkTSTurboModule(ctx, name) {
    methodMap_ = {
        {{#methods}}
        ARK_{{asyncMarker}}METHOD_METADATA({{name}}, {{argsCount}}),
        {{/methods}}
    };
}

} // namespace rnoh
`;

export type TurboModuleCppTemplateMethod = {
  name: string;
  isAsync: boolean;
  argsCount: number;
};

export class TurboModuleCppTemplate {
  private methods: TurboModuleCppTemplateMethod[] = [];

  constructor(private name: string, private codegenNoticeLines: string[]) {}

  addMethod(method: TurboModuleCppTemplateMethod) {
    this.methods.push(method);
  }

  build(): string {
    return mustache.render(TEMPLATE.trimStart(), {
      codegenNoticeLines: this.codegenNoticeLines.map((line) => ({
        line,
      })),
      className: this.name,
      methods: this.methods.map((method) => ({
        name: method.name,
        argsCount: method.argsCount,
        asyncMarker: method.isAsync ? 'ASYNC_' : undefined,
      })),
    });
  }
}
