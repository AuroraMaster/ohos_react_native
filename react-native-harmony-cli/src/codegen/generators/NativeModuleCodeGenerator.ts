/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AbsolutePath } from '../../core';
import type { SpecCodeGenerator } from '../core';
import {
  TurboModuleCppTemplate,
  TurboModuleHTemplate,
  TurboModuleInterfaceTS,
} from '../templates';
import {
  TypeAnnotationToTS,
  wrapWithQuotes,
  SpecSchema,
  CodegenError,
} from '../core';
import { GlueCodeTurboModuleData } from './AppBuildTimeGlueCodeGenerator';

/**
 * Generates boilerplate code for Turbo Modules.
 */
export class NativeModuleCodeGenerator implements SpecCodeGenerator {
  private glueCodeTurboModulesData: GlueCodeTurboModuleData[] = [];
  constructor(
    private cppOutputPath: AbsolutePath,
    private etsOutputPath: AbsolutePath,
    private codegenNoticeLines: string[],
    private rnohImport: string
  ) {}

  getGlueCodeData() {
    return this.glueCodeTurboModulesData;
  }

  generate(schema: SpecSchema): Map<AbsolutePath, string> {
    const result = new Map<AbsolutePath, string>();
    if (schema.type !== 'NativeModule') {
      throw new CodegenError({
        whatHappened: `NativeModuleCodeGenerator can't generate code for module type: ${schema.type}`,
        unexpected: true,
      });
    }
    const typeAnnotationToTS = new TypeAnnotationToTS();
    const turboModuleInterfaceTS = new TurboModuleInterfaceTS(
      schema.moduleName,
      this.codegenNoticeLines,
      this.rnohImport
    );
    const turboModuleH = new TurboModuleHTemplate(
      schema.moduleName,
      this.codegenNoticeLines
    ).build();
    result.set(
      this.cppOutputPath.copyWithNewSegment(`${schema.moduleName}.h`),
      turboModuleH
    );

    const turboModuleCpp = new TurboModuleCppTemplate(
      schema.moduleName,
      this.codegenNoticeLines
    );

    Object.entries(schema.aliasMap).map(([name, typeAnnotation]) => {
      turboModuleInterfaceTS.addAlias({
        name: name,
        type: typeAnnotationToTS.convert(typeAnnotation),
      });
    });

    Object.entries(schema.enumMap).map(([name, enumSpec]) => {
      turboModuleInterfaceTS.addEnum({
        name,
        members: enumSpec.members.map((member) => ({
          name: member.name,
          value:
            typeof member.value === 'string'
              ? wrapWithQuotes(member.value)
              : member.value.toString(),
        })),
      });
    });

    schema.spec.properties.map((prop) => {
      if (prop.typeAnnotation.type === 'FunctionTypeAnnotation') {
        turboModuleCpp.addMethod({
          name: prop.name,
          argsCount: prop.typeAnnotation.params.length,
          isAsync:
            prop.typeAnnotation.returnTypeAnnotation.type ===
            'PromiseTypeAnnotation',
        });

        turboModuleInterfaceTS.addMethod({
          name: prop.name,
          returnType: typeAnnotationToTS.convert(
            prop.typeAnnotation.returnTypeAnnotation
          ),
          stringifiedArgs: prop.typeAnnotation.params
            .map(
              (param) =>
                `${param.name}: ${typeAnnotationToTS.convert(
                  param.typeAnnotation
                )}`
            )
            .join(', '),
        });
      }
    });
    result.set(
      this.cppOutputPath.copyWithNewSegment(`${schema.moduleName}.cpp`),
      turboModuleCpp.build()
    );
    result.set(
      this.etsOutputPath.copyWithNewSegment(`${schema.moduleName}.ts`),
      turboModuleInterfaceTS.build()
    );
    this.glueCodeTurboModulesData.push({ name: schema.moduleName });
    return result;
  }
}
