/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import * as tmp from 'tmp';
import { copyMetroConfig, createFileStructure } from './fsUtils';
import * as pathUtils from 'path';
import * as colors from 'colors/safe';

let tmpDir = '';
let removeTmpDir = () => {};
let originalCwd = process.cwd();
let metroConfigPath = '';

beforeEach(async () => {
  const dir = tmp.dirSync();
  tmpDir = dir.name;
  removeTmpDir = dir.removeCallback;

  metroConfigPath = copyMetroConfig(tmpDir, 'metro.config.js', 'node_modules');
  process.chdir(originalCwd);
});

afterEach(removeTmpDir);

it('should redirect internal imports in harmony packages', async () => {
  createFileStructure(tmpDir, {
    node_modules: {
      'react-native-harmony-foo': {
        'package.json': `{
          "name": "react-native-harmony-foo",
          "harmony": {
            "alias": "react-native-foo",
            "redirectInternalImports": true
          }}`,
      },
    },
  });

  process.chdir(tmpDir);

  const { createHarmonyMetroConfig } = require(metroConfigPath);

  const resolutionContext = {
    resolveRequest: (context: any, moduleName: string, platform: string) => {
      return { moduleName: moduleName, platform: platform };
    },
    originModulePath: pathUtils.join(
      tmpDir,
      'node_modules',
      'react-native-foo',
      'src',
      'foo.ts'
    ),
  };
  const moduleName = './bar.ts';
  const platform = 'harmony';
  const resolveRequest = createHarmonyMetroConfig({
    reactNativeHarmonyPackageName: 'react-native-harmony',
  })?.resolver?.resolveRequest;

  const resolution = resolveRequest(resolutionContext, moduleName, platform);

  expect(resolution.moduleName).toEqual(
    pathUtils.posix.join('react-native-harmony-foo', 'src', moduleName)
  );
});

it('should redirect internal imports in scoped harmony packages', async () => {
  createFileStructure(tmpDir, {
    node_modules: {
      '@rnoh': {
        'react-native-foo': {
          'package.json': `{
          "name": "@rnoh/react-native-harmony-foo",
          "harmony": {
            "alias": "react-native-foo",
            "redirectInternalImports": true
          }}`,
        },
      },
    },
  });

  process.chdir(tmpDir);

  const { createHarmonyMetroConfig } = require(metroConfigPath);

  const resolutionContext = {
    resolveRequest: (context: any, moduleName: any, platform: any) => {
      return { moduleName: moduleName, platform: platform };
    },
    originModulePath: pathUtils.join(
      tmpDir,
      'node_modules',
      'react-native-foo',
      'src',
      'foo.ts'
    ),
  };
  const moduleName = './bar.ts';
  const platform = 'harmony';
  const resolveRequest = createHarmonyMetroConfig({
    reactNativeHarmonyPackageName: 'react-native-harmony',
  })?.resolver?.resolveRequest;

  const resolution = resolveRequest(resolutionContext, moduleName, platform);

  expect(resolution.moduleName).toEqual(
    pathUtils.posix.join('@rnoh/react-native-foo', 'src', moduleName)
  );
});

it('should redirect internal imports if alias is scoped', async () => {
  createFileStructure(tmpDir, {
    node_modules: {
      '@rnoh': {
        'react-native-harmony-foo': {
          'package.json': `{
          "name": "@rnoh/react-native-harmony-foo",
          "harmony": {
            "alias": "@org/react-native-foo",
            "redirectInternalImports": true
          }}`,
        },
      },
    },
  });

  process.chdir(tmpDir);

  const { createHarmonyMetroConfig } = require(metroConfigPath);

  const resolutionContext = {
    resolveRequest: (context: any, moduleName: any, platform: any) => {
      return { moduleName: moduleName, platform: platform };
    },
    originModulePath: pathUtils.join(
      tmpDir,
      'node_modules',
      '@org',
      'react-native-foo',
      'src',
      'foo.ts'
    ),
  };
  const moduleName = './bar.ts';
  const platform = 'harmony';
  const resolveRequest = createHarmonyMetroConfig({
    reactNativeHarmonyPackageName: 'react-native-harmony',
  })?.resolver?.resolveRequest;

  const resolution = resolveRequest(resolutionContext, moduleName, platform);

  expect(resolution.moduleName).toEqual(
    pathUtils.posix.join('@rnoh', 'react-native-harmony-foo', 'src', moduleName)
  );
});

it('should not redirect internal imports when flag not set', async () => {
  createFileStructure(tmpDir, {
    node_modules: {
      'react-native-harmony-foo': {
        'package.json': `{
          "name": "react-native-harmony-foo",
          "harmony": {
            "alias": "react-native-foo"
          }}`,
      },
    },
  });

  process.chdir(tmpDir);

  const { createHarmonyMetroConfig } = require(metroConfigPath);

  const resolutionContext = {
    resolveRequest: (context: any, moduleName: any, platform: any) => {
      return { moduleName: moduleName, platform: platform };
    },
    originModulePath: pathUtils.join(
      tmpDir,
      'node_modules',
      'react-native-foo',
      'src',
      'foo.ts'
    ),
  };
  const moduleName = './bar.ts';
  const platform = 'harmony';
  const resolveRequest = createHarmonyMetroConfig({
    reactNativeHarmonyPackageName: 'react-native-harmony',
  })?.resolver?.resolveRequest;

  const resolution = resolveRequest(resolutionContext, moduleName, platform);

  expect(resolution.moduleName).toEqual(moduleName);
});