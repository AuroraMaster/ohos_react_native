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

it('should generate MetroHostConfig.ts', async () => {
  createFileStructure(tmpDir, {
    foo: {},
  });

  new ReactNativeFixture(tmpDir).createMetroHostConstantsHarmony({
    output: 'foo/MetroHostConfig.ts',
  });

  expect(
    fs
      .readFileSync(pathUtils.join(tmpDir, 'foo', 'MetroHostConfig.ts'))
      .includes('METRO_SERVER_IP')
  ).toBeTruthy();
});
