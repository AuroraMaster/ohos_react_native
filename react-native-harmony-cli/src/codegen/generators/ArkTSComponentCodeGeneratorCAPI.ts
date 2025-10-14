/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AbsolutePath } from '../../core';
import {
  TypeAnnotationToTS,
  createEventNameFromEventHandlerName,
  getStringifiedTSDefaultValueFromPropTypeAnnotation,
} from '../core';
import type { CodeGenerator, UberSchema } from '../core';
import {
  ComponentJSIBinderHTemplate,
  ComponentUtilsTSTemplate,
} from '../templates';
import { ArkTSComponentLibGlueCodeData } from './LibGlueCodeGenerator';

/**
 * Generates component code for ArkTS components used in C-API architecture.
 */
export class ArkTSComponentCodeGeneratorCAPI
  implements CodeGenerator<UberSchema>
{
  private glueCodeDataItems: ArkTSComponentLibGlueCodeData[] = [];

  constructor(
    private cppOutputPath: AbsolutePath,
    private tsOutputPath: AbsolutePath,
    private codegenNoticeLines: string[],
    private rnohImport: string
  ) {}

  getLibGlueCodeData(): ArkTSComponentLibGlueCodeData[] {
    return this.glueCodeDataItems;
  }

  generate(uberSchema: UberSchema): Map<AbsolutePath, string> {
    const typeAnnotationToTS = new TypeAnnotationToTS();
    const result = new Map<AbsolutePath, string>();
    const componentSpecSchemas =
      uberSchema.findAllSpecSchemasByType('Component');

    for (const componentSpecSchema of componentSpecSchemas) {
      Object.entries(componentSpecSchema.components).map(([name, shape]) => {
        const componentJSIBinderHTemplate = new ComponentJSIBinderHTemplate(
          name,
          this.codegenNoticeLines
        );
        const componentUtilsTSTemplate = new ComponentUtilsTSTemplate(
          name,
          this.codegenNoticeLines,
          this.rnohImport
        );
        shape.props.forEach((prop) => {
          componentJSIBinderHTemplate.addProp({
            name: prop.name,
            isObject: prop.typeAnnotation.type === 'ObjectTypeAnnotation',
          });

          componentUtilsTSTemplate.addProp({
            name: prop.name,
            tsType: typeAnnotationToTS.convert(prop.typeAnnotation),
            defaultValue: getStringifiedTSDefaultValueFromPropTypeAnnotation(
              prop.typeAnnotation
            ),
            isOptional: prop.optional,
          });
        });
        shape.commands.forEach((command) => {
          componentJSIBinderHTemplate.addCommand({
            name: command.name,
          });
          componentUtilsTSTemplate.addCommand({
            name: command.name,
            argsTSType:
              '[' +
              command.typeAnnotation.params
                .map(
                  (param) =>
                    typeAnnotationToTS.convert(param.typeAnnotation) +
                    (param.optional ? '?' : '')
                )
                .join(', ') +
              ']',
          });
        });

        shape.events.forEach((event) => {
          componentUtilsTSTemplate.addEvent({
            name: createEventNameFromEventHandlerName(event.name),
            payloadTSType: event.typeAnnotation.argument
              ? typeAnnotationToTS.convert(event.typeAnnotation.argument)
              : 'undefined',
          });
          componentJSIBinderHTemplate.addEvent({
            type: event.bubblingType,
            eventHandlerName: event.name,
          });
        });
        result.set(
          this.cppOutputPath.copyWithNewSegment(`${name}JSIBinder.h`),
          componentJSIBinderHTemplate.build()
        );
        result.set(
          this.tsOutputPath.copyWithNewSegment(`${name}.ts`),
          componentUtilsTSTemplate.build()
        );
        this.glueCodeDataItems.push({
          componentName: name,
          eventNames: shape.events.map((event) =>
            createEventNameFromEventHandlerName(event.name)
          ),
        });
      });
    }

    return result;
  }
}
