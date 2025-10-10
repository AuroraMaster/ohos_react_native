/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import tmp from 'tmp';
import { ReactNativeFixture } from './ReactNativeFixture';
import { buildDirTree } from './fsUtils';
import { AbsolutePath, maybeMakeDirectories } from '../src/core';
import fs from 'node:fs';
import path from 'node:path';

let tmpDir: AbsolutePath;

beforeEach(async () => {
  const dir = tmp.dirSync();
  tmpDir = new AbsolutePath(dir.name);
});

afterEach(() => {
  if (expect.getState().assertionCalls != expect.getState().numPassingAsserts) {
    console.log(buildDirTree(tmpDir.getValue()));
  }
});

function createSampleSpecFiles(
  specDirPath: AbsolutePath,
  componentNames: string[],
  moduleNames: string[]
) {
  maybeMakeDirectories(specDirPath);
  componentNames.forEach((componentName) => {
    fs.writeFileSync(
      specDirPath
        .copyWithNewSegment(`${componentName}NativeComponent.ts`)
        .getValue(),
      createViewComponentSpec(componentName)
    );
  });
  moduleNames.forEach((moduleName) => {
    fs.writeFileSync(
      specDirPath.copyWithNewSegment(`Native${moduleName}.ts`).getValue(),
      createTurboModuleSpec(moduleName)
    );
  });
}

function createViewComponentSpec(name: string) {
  return `
import { ViewProps, HostComponent } from 'react-native';
import codegenNativeComponent from 'react-native/Libraries/Utilities/codegenNativeComponent';
import type { Float } from 'react-native/Libraries/Types/CodegenTypes';

export interface ${name}NativeProps extends ViewProps {
  size: Float;
}

export default codegenNativeComponent<${name}NativeProps>(
  '${name}'
) as HostComponent<${name}NativeProps>;
  `;
}

function createTurboModuleSpec(name: string) {
  return `
import type { TurboModule } from 'react-native/Libraries/TurboModule/RCTExport';
import { TurboModuleRegistry } from 'react-native';

interface Spec extends TurboModule {
  runProcedure(): void;
  getBool(arg: boolean): boolean;
  getString(arg: string): string;
  getObject(arg: { x: { y: number } }): Object;
  getArrayOfNumbersAsync(): Promise<number[]>;
}

export default TurboModuleRegistry.get<Spec>("${name}")!;
`;
}

function getFileNamesFromDirPath(dirPath: AbsolutePath): string[] {
  try {
    if (!fs.existsSync(dirPath.getValue())) {
      return [];
    }
    const files: string[] = fs.readdirSync(dirPath.getValue());
    const fileNames: string[] = files.filter((file) => {
      const filePath: string = path.join(dirPath.getValue(), file);
      return fs.statSync(filePath).isFile();
    });
    return fileNames;
  } catch (err) {
    console.error('Error reading directory:', err);
    throw err;
  }
}

it('should export codegen-lib-harmony command', () => {
  const helpDescription = new ReactNativeFixture(tmpDir).codegenLibHarmony({
    help: true,
  });
  expect(helpDescription.includes('codegen-lib-harmony')).toBeTruthy();
});

it('should generate components and Turbo Module code to the desired output directory based on provided paths to specs', () => {
  const specsDirPath = tmpDir.copyWithNewSegment('specs');
  createSampleSpecFiles(specsDirPath, ['MyView', 'MyView2'], ['MyModule']);
  const cppOutputPath = tmpDir.copyWithNewSegment('codegen', 'cpp');
  const etsOutputPath = tmpDir.copyWithNewSegment('codegen', 'ets');

  new ReactNativeFixture(tmpDir).codegenLibHarmony({
    npmPackageName: 'react-native-harmony-foobar',
    cppOutputPath: cppOutputPath.relativeTo(tmpDir).getValue(),
    etsOutputPath: etsOutputPath.relativeTo(tmpDir).getValue(),
    cppComponentsSpecPaths: specsDirPath.relativeTo(tmpDir).getValue(),
    arktsComponentsSpecPaths: specsDirPath.relativeTo(tmpDir).getValue(),
    turboModulesSpecPaths: specsDirPath.relativeTo(tmpDir).getValue(),
    noSafetyCheck: true,
  });

  expect(
    getFileNamesFromDirPath(
      cppOutputPath.copyWithNewSegment('RNOH', 'generated', 'components')
    ).length
  ).toBeGreaterThanOrEqual(4);
  expect(
    getFileNamesFromDirPath(
      cppOutputPath.copyWithNewSegment('RNOH', 'generated', 'turbo_modules')
    ).length
  ).toBeGreaterThan(0);
  expect(
    getFileNamesFromDirPath(
      cppOutputPath.copyWithNewSegment(
        'react',
        'renderer',
        'components',
        'react_native_harmony_foobar'
      )
    ).length
  ).toBeGreaterThan(0);
  expect(
    getFileNamesFromDirPath(etsOutputPath.copyWithNewSegment('components'))
      .length
  ).toBeGreaterThan(0);
  expect(
    getFileNamesFromDirPath(etsOutputPath.copyWithNewSegment('turboModules'))
      .length
  ).toBeGreaterThan(0);
});
