/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  ComponentSchema,
  NativeModuleSchema,
  SchemaType as RawUberSchema,
} from '@react-native/codegen/lib/CodegenSchema';
import {
  AbsolutePath,
  ValueObject,
  PackageJSON,
  ProjectDependenciesManager,
  CodegenConfig,
  FS
} from '../../core';
// @ts-expect-error
import extractUberSchemaFromSpecFilePaths_ from '@react-native/codegen/lib/cli/combine/combine-js-to-schema.js';
import { CodegenError } from './CodegenError';

function createRawUberSchemaFromSpecFilePaths(
  projectSourceFilePaths: AbsolutePath[]
): RawUberSchema {
  return extractUberSchemaFromSpecFilePaths_(
    projectSourceFilePaths.map((p) => p.getValue())
  );
}

export type SpecSchema = ComponentSchema | NativeModuleSchema;
export type SpecSchemaType = SpecSchema['type'];
type FindSpecSchemaByType<
  TType extends SpecSchemaType,
  TSpecSchema = SpecSchema
> = TSpecSchema extends { type: TType } ? TSpecSchema : never;

/**
 * Contains component and turbo module (NativeModule) schemas. The "Uber" word is used here to highlight that SpecSchemas don't extend this class.
 */
export class UberSchema implements ValueObject {
  static fromSpecFilePaths(specPaths: AbsolutePath[]): UberSchema {
    return new UberSchema(createRawUberSchemaFromSpecFilePaths(specPaths));
  }

  static fromCodegenConfig(codegenConfig: CodegenConfig): UberSchema {
    try {
      return new UberSchema(
        createRawUberSchemaFromSpecFilePaths(codegenConfig.getSpecFilePaths())
      );
    } catch (err) {
      if (err instanceof Error) {
        throw new CodegenError({
          whatHappened: "Couldn't create the schema",
          whatCanUserDo: [
            `There's probably at least one spec file defined in your project or in a third-party package that breaks some code generation restrictions. Please check the message below. If it's ambiguous, debug the problem with divide and conquer strategy.\n\n${err.message}`,
          ],
        });
      }
      throw err;
    }
  }

  static async fromProject(
    fs: FS,
    projectRootPath: AbsolutePath,
    onShouldAcceptCodegenConfig?: (
      codegenVersion: number,
      packageName: string
    ) => boolean
  ): Promise<UberSchema> {
    const onShouldAcceptCodegenConfig_ =
      onShouldAcceptCodegenConfig ?? ((x: number) => true);
    const packageJSON = PackageJSON.fromProjectRootPath(
      fs,
      projectRootPath,
      projectRootPath
    );
    const acceptedCodegenConfigs: CodegenConfig[] = [];
    const appCodegenConfigs = packageJSON.getCodegenConfigs();
    for (const codegenConfig of appCodegenConfigs) {
      if (
        onShouldAcceptCodegenConfig_(
          codegenConfig.getVersion(),
          packageJSON.name
        )
      ) {
        acceptedCodegenConfigs.push(codegenConfig);
      }
    }
    await new ProjectDependenciesManager(fs, projectRootPath).forEachAsync(
      (dependency) => {
        const codegenConfigs = dependency.getCodegenConfigs();
        for (const codegenConfig of codegenConfigs) {
          if (
            onShouldAcceptCodegenConfig_(
              codegenConfig.getVersion(),
              dependency.readPackageJSON().name
            )
          ) {
            acceptedCodegenConfigs.push(codegenConfig);
          }
        }
      }
    );
    try {
      return new UberSchema(
        createRawUberSchemaFromSpecFilePaths(
          acceptedCodegenConfigs.flatMap((codegenConfig) =>
            codegenConfig.getSpecFilePaths()
          )
        )
      );
    } catch (err) {
      if (err instanceof Error) {
        throw new CodegenError({
          whatHappened: "Couldn't create the schema",
          whatCanUserDo: [
            `There's probably at least one spec file defined in your project or in a third-party package that breaks some code generation restrictions. Please check the message below. If it's ambiguous, debug the problem with divide and conquer strategy.\n\n${err.message}`,
          ],
        });
      }
      throw err;
    }
  }

  private constructor(private schemaValue: RawUberSchema) {}

  getValue(): RawUberSchema {
    return this.schemaValue;
  }

  findAllSpecSchemasByType<TSpecSchemaType extends SpecSchemaType>(
    schemaType: TSpecSchemaType
  ): FindSpecSchemaByType<TSpecSchemaType>[] {
    return Object.values(this.schemaValue.modules)
      .filter((module) => module.type === schemaType)
      .map((module) => module as FindSpecSchemaByType<TSpecSchemaType>);
  }

  getSpecSchemaByFilenameMap(): ReadonlyMap<string, SpecSchema> {
    return Object.entries(this.schemaValue.modules).reduce(
      (acc, [moduleName, moduleSchema]) => {
        acc.set(moduleName, moduleSchema);
        return acc;
      },
      new Map<string, SpecSchema>()
    );
  }
}
