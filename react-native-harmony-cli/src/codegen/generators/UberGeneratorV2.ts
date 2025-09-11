/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import pathUtils from 'path';
import { AbsolutePath } from '../../core';
import { CodeGenerator } from '../core';
import {
  CppComponentCodeGenerator,
  LibraryData,
} from './CppComponentCodeGenerator';
import { NativeModuleCodeGenerator } from './NativeModuleCodeGenerator';
import { GlueCodeDataV2 } from './AppBuildTimeGlueCodeGenerator';
import { SharedComponentCodeGenerator } from './SharedComponentCodeGenerator';

// The name of rnoh OHOS module
const RNOH_OHOS_NAME = '@rnoh/react-native-openharmony';

/**
 * Generates code for libraries built on top of RNOH's C-API architecture.
 */
export class UberGeneratorV2 implements CodeGenerator<LibraryData[]> {
  private glueCodeData: GlueCodeDataV2 = new Map();

  constructor(
    private cppOutputPath: AbsolutePath,
    private etsOutputPath: AbsolutePath,
    private codegenNoticeLines: string[]
  ) {}

  getGlueCodeData(): GlueCodeDataV2 {
    return this.glueCodeData;
  }

  generate(npmPackagesInfo: LibraryData[]) {
    const fileContentByPath: Map<AbsolutePath, string> = new Map();

    npmPackagesInfo.forEach(({ libraryCppName, uberSchema }) => {
      this.glueCodeData.set(libraryCppName, {
        components: [],
        turboModules: [],
      });
      const libraryGlueCodeData = this.glueCodeData.get(libraryCppName)!;
      const libraryCppOutput =
        this.cppOutputPath.copyWithNewSegment(libraryCppName);
      const sharedComponentCodeGenerator = new SharedComponentCodeGenerator(
        (filename) => {
          return libraryCppOutput.copyWithNewSegment(
            'react',
            'renderer',
            'components',
            libraryCppName,
            filename
          );
        }
      );
      const cppComponentCodeGenerator = new CppComponentCodeGenerator(
        (filename) => {
          return libraryCppOutput.copyWithNewSegment(
            'RNOH',
            'generated',
            'components',
            filename
          );
        },
        this.codegenNoticeLines
      );
      cppComponentCodeGenerator
        .generate({ libraryCppName, uberSchema })
        .forEach((fileContent, path) => {
          fileContentByPath.set(path, fileContent);
        });
      cppComponentCodeGenerator
        .getGlueCodeData()
        .forEach((componentGlueCodeData) => {
          libraryGlueCodeData.components.push(componentGlueCodeData);
        });

      sharedComponentCodeGenerator
        .generate({ libraryCppName, uberSchema })
        .forEach((fileContent, path) => {
          fileContentByPath.set(path, fileContent);
        });

      for (const [_fileName, specSchema] of uberSchema
        .getSpecSchemaByFilenameMap()
        .entries()) {
        if (specSchema.type === 'NativeModule') {
          const isRNOHModulePath = this.etsOutputPath.getValue().endsWith(`${RNOH_OHOS_NAME}/generated`.replace(/\//g, pathUtils.sep));
          const nativeModuleCodeGenerator = new NativeModuleCodeGenerator(
            libraryCppOutput.copyWithNewSegment(
              'RNOH',
              'generated',
              'turbo_modules'
            ),
            this.etsOutputPath.copyWithNewSegment('turboModules'),
            this.codegenNoticeLines,
            isRNOHModulePath ? '../../ts' : `${RNOH_OHOS_NAME}/ts`
          );
          nativeModuleCodeGenerator
            .generate(specSchema)
            .forEach((fileContent, filePath) => {
              fileContentByPath.set(filePath, fileContent);
            });
          nativeModuleCodeGenerator
            .getGlueCodeData()
            .forEach((turboModuleGlueCodeData) => {
              libraryGlueCodeData.turboModules.push(turboModuleGlueCodeData);
            });
        }
      }
    });

    return fileContentByPath;
  }
}
