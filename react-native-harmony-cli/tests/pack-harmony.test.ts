/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import tmp from 'tmp';
import { ReactNativeFixture } from './ReactNativeFixture';
import pathUtils from 'path';
import fs from 'fs';
import { createFileStructure } from './fsUtils';

let tmpDir: string = '';
let removeTmpDir = () => {};

beforeEach(async () => {
  const dir = tmp.dirSync();
  tmpDir = dir.name;
  removeTmpDir = dir.removeCallback;
});

afterEach(removeTmpDir);

it('should create harmony.tar.gz', async () => {
  createFileStructure(tmpDir, {
    tester: {
      harmony: {
        'package-name': {
          'foo.txt': 'foo',
        },
      },
    },
    'react-native-harmony-package-name': {
      'package.json': `{"version": "0.0.1"}`,
      harmony: {},
    },
  });

  new ReactNativeFixture(tmpDir).packHarmony({
    ohModulePath: 'tester/harmony/package-name',
    harmonyDirPath: 'react-native-harmony-package-name/harmony',
    packageJSONPath: 'react-native-harmony-package-name/package.json',
  });

  expect(
    fs.existsSync(
      pathUtils.join(
        tmpDir,
        'react-native-harmony-package-name',
        'harmony.tar.gz'
      )
    )
  ).toBeTruthy();
});

it("should create harmony dir if it doesn't exist", () => {
  createFileStructure(tmpDir, {
    tester: {
      harmony: {
        'package-name': {
          'foo.txt': 'foo',
        },
      },
    },
    'react-native-harmony-package-name': {
      'package.json': `{"version": "0.0.1"}`,
    },
  });

  new ReactNativeFixture(tmpDir).packHarmony({
    ohModulePath: 'tester/harmony/package-name',
    harmonyDirPath: 'react-native-harmony-package-name/harmony',
    packageJSONPath: 'react-native-harmony-package-name/package.json',
  });

  expect(
    fs.existsSync(
      pathUtils.join(
        tmpDir,
        'react-native-harmony-package-name',
        'harmony.tar.gz'
      )
    )
  ).toBeTruthy();
});
