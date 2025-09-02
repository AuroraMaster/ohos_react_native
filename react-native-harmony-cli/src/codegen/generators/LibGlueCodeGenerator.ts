/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import Case from 'case';
import { AbsolutePath } from '../../core';
import { CodeGenerator } from '../core';
import { BasePackageHTemplate } from '../templates/BasePackageHTemplate';
import { IndexTSTemplate } from '../templates';
import { GlueCodeTurboModuleData } from './AppBuildTimeGlueCodeGenerator';

export type ArkTSComponentLibGlueCodeData = {
  componentName: string;
  eventNames: string[];
};
export type CppComponentLibGlueCodeData = { componentName: string };

type LibGlueCodeData = {
  libraryCppName: string;
  turboModules: GlueCodeTurboModuleData[];
  arkTSComponents: ArkTSComponentLibGlueCodeData[];
  cppComponents: CppComponentLibGlueCodeData[];
};

/**
 * NOTE: This class is coupled with the generated code structure defined in the codegen-harmony-lib.
 */
export class LibGlueCodeGenerator implements CodeGenerator<LibGlueCodeData> {
  constructor(
    private cppOutputPath: AbsolutePath,
    private etsOutputPath: AbsolutePath,
    private codegenNoticeLines: string[]
  ) {}

  generate({
    libraryCppName,
    turboModules,
    arkTSComponents,
    cppComponents,
  }: LibGlueCodeData): Map<AbsolutePath, string> {
    const fileContentByPath = new Map<AbsolutePath, string>();
    const basePackageClassName = `Base${Case.pascal(libraryCppName)}Package`;
    const turboModules_indexTS = new IndexTSTemplate(this.codegenNoticeLines);
    const components_indexTS = new IndexTSTemplate(this.codegenNoticeLines);
    const generated_indexETS = new IndexTSTemplate(this.codegenNoticeLines);

    const basePackageTemplate = new BasePackageHTemplate(
      basePackageClassName,
      this.codegenNoticeLines
    );
    turboModules.forEach((turboModule) => {
      basePackageTemplate.addTurboModule(turboModule);
      turboModules_indexTS.addReexport({ from: `./${turboModule.name}` });
    });
    arkTSComponents.forEach(({ componentName, eventNames }) => {
      components_indexTS.addReexport({ from: `./${componentName}` });
      basePackageTemplate.addComponent({ name: componentName, libraryCppName, isArkTSComponent: true });
      eventNames.forEach((eventName) =>
        basePackageTemplate.addArkTSEvent({ name: eventName })
      );
    });
    cppComponents.forEach(({ componentName }) => {
      basePackageTemplate.addComponent({ name: componentName, libraryCppName, isArkTSComponent: false });
    });
    fileContentByPath.set(
      this.cppOutputPath.copyWithNewSegment(
        'RNOH',
        'generated',
        `${basePackageClassName}.h`
      ),
      basePackageTemplate.build()
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
      this.etsOutputPath.copyWithNewSegment('ts.ts'),
      new IndexTSTemplate(this.codegenNoticeLines)
        .addReexport({ from: './components/ts', as: 'RNC' })
        .addReexport({ from: './turboModules/ts', as: 'TM' })
        .build()
    );
    generated_indexETS.addReexport({ from: './ts' });
    fileContentByPath.set(
      this.etsOutputPath.copyWithNewSegment('index.ets'),
      generated_indexETS.build()
    );
    return fileContentByPath;
  }
}
