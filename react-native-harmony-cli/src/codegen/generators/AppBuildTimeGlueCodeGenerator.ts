/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import Case from 'case';
import { AbsolutePath } from '../../core';
import { CodeGenerator } from '../core';
import { IndexTSTemplate, RNOHGeneratedPackageHTemplate } from '../templates';
import { BasePackageHTemplate } from '../templates/BasePackageHTemplate';

export type GlueCodeComponentDataV1 = { name: string; eventNames: string[] };

export type GlueCodeComponentDataV2 = { name: string; libraryCppName: string };

export type GlueCodeTurboModuleData = { name: string };

export type GlueCodeDataV1 = {
  components: GlueCodeComponentDataV1[];
  turboModules: GlueCodeTurboModuleData[];
};

type LibraryName = string;
export type GlueCodeDataV2 = Map<
  LibraryName,
  {
    components: GlueCodeComponentDataV2[];
    turboModules: GlueCodeTurboModuleData[];
  }
>;

export type GlueCodeData = {
  v1: GlueCodeDataV1;
  v2: GlueCodeDataV2;
};

/**
 * Generates code based on UberGenerators results. If UberGenerators produce "leaves" (concrete turbo module or component classes),
 * this class generates "branches" (barrel files like index.ts) and "trunks" (package file or files).
 */
export class AppBuildTimeGlueCodeGenerator
  implements CodeGenerator<GlueCodeData>
{
  constructor(
    private cppOutputPath: AbsolutePath,
    private etsOutputPath: AbsolutePath,
    private codegenNoticeLines: string[]
  ) {}

  generate({ v1, v2 }: GlueCodeData): Map<AbsolutePath, string> {
    const fileContentByPath = new Map<AbsolutePath, string>();
    const generatedPackageH_v1 = new RNOHGeneratedPackageHTemplate();
    const components_indexTS = new IndexTSTemplate(this.codegenNoticeLines);
    const turboModules_indexTS = new IndexTSTemplate(this.codegenNoticeLines);
    const generated_indexETS = new IndexTSTemplate(this.codegenNoticeLines);

    generated_indexETS.addReexport({ from: './ts' });
    v1.components.forEach(({ name, eventNames }) => {
      components_indexTS.addReexport({ from: `./${name}` });
      generatedPackageH_v1.addComponent({
        name,
        supportedEventNames: eventNames,
        isArkTSComponent: true,
      });
    });

    v1.turboModules.forEach((turboModule) => {
      turboModules_indexTS.addReexport({ from: `./${turboModule.name}` });
      generatedPackageH_v1.addTurboModule(turboModule);
    });
    v2.forEach(({ components, turboModules }, libraryName) => {
      const basePackageClassName = `Base${Case.pascal(libraryName)}Package`;
      const generatedPackageH_v2 = new BasePackageHTemplate(
        basePackageClassName,
        this.codegenNoticeLines
      );
      components.forEach((component) => {
        generatedPackageH_v2.addComponent({
          ...component,
          isArkTSComponent: false,
        });
      });
      turboModules.forEach((turboModule) => {
        turboModules_indexTS.addReexport({ from: `./${turboModule.name}` });
        generatedPackageH_v2.addTurboModule(turboModule);
      });
      fileContentByPath.set(
        this.cppOutputPath.copyWithNewSegment(
          libraryName,
          'RNOH',
          'generated',
          `${basePackageClassName}.h`
        ),
        generatedPackageH_v2.build()
      );
    });
    fileContentByPath.set(
      this.cppOutputPath.copyWithNewSegment('RNOHGeneratedPackage.h'),
      generatedPackageH_v1.build()
    );
    fileContentByPath.set(
      this.etsOutputPath.copyWithNewSegment('ts.ts'),
      new IndexTSTemplate(this.codegenNoticeLines)
        .addReexport({ from: './components/ts', as: 'RNC' })
        .addReexport({ from: './turboModules/ts', as: 'TM' })
        .build()
    );
    fileContentByPath.set(
      this.etsOutputPath.copyWithNewSegment('components', 'ts.ts'),
      components_indexTS.build()
    );
    fileContentByPath.set(
      this.etsOutputPath.copyWithNewSegment('turboModules', 'ts.ts'),
      turboModules_indexTS.build()
    );
    fileContentByPath.set(
      this.etsOutputPath.copyWithNewSegment('index.ets'),
      generated_indexETS.build()
    );
    return fileContentByPath;
  }
}
