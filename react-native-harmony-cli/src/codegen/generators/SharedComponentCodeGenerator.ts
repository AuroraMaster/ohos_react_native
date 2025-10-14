/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import RNCodegen from '@react-native/codegen/lib/generators/RNCodegen';
import { AbsolutePath } from '../../core';
import { CodeGenerator, UberSchema } from '../core';

type LibraryData = {
  libraryCppName: string;
  uberSchema: UberSchema;
};

export class SharedComponentCodeGenerator
  implements CodeGenerator<LibraryData>
{
  constructor(
    private onGetSharedOutputPath: (filename: string) => AbsolutePath
  ) {}

  generate({
    libraryCppName,
    uberSchema,
  }: LibraryData): Map<AbsolutePath, string> {
    const fileContentByPath = new Map<AbsolutePath, string>();

    const generatorFns = [
      RNCodegen.allGenerators.generatePropsH,
      RNCodegen.allGenerators.generatePropsCpp,
      RNCodegen.allGenerators.generateEventEmitterH,
      RNCodegen.allGenerators.generateEventEmitterCpp,
      RNCodegen.allGenerators.generateComponentDescriptorH,
      RNCodegen.allGenerators.generateStateH,
      RNCodegen.allGenerators.generateStateCpp,
      RNCodegen.allGenerators.generateShadowNodeH,
      RNCodegen.allGenerators.generateShadowNodeCpp,
    ];
    generatorFns.forEach((generate) => {
      const filesOutput = generate(
        libraryCppName,
        uberSchema.getValue(),
        'UNUSED_packageName',
        true
      );
      filesOutput.forEach((fileContent, filename) => {
        fileContentByPath.set(
          this.onGetSharedOutputPath(filename),
          fileContent
        );
      });
    });
    return fileContentByPath;
  }
}
