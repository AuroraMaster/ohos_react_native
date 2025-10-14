/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import tmp from 'tmp';
import { ProjectDependenciesManager } from '../src/core/ProjectDependenciesManager';
import { createFileStructure } from './fsUtils';
import { AbsolutePath } from '../src/core';
import { RealFS } from '../src/io';

let tmpDir: string = '';

beforeEach(async () => {
  const dir = tmp.dirSync();
  tmpDir = dir.name;
});

it('should not recognize a dependency if package.json is not defined', async () => {
  createFileStructure(tmpDir, {
    node_modules: {
      '@types': {
        node: {
          'ts.4.8': {},
        },
      },
      'some-package': {
        'package.json': '...',
      },
    },
  });
  const projectDependenciesManager = new ProjectDependenciesManager(
    new RealFS(),
    new AbsolutePath(tmpDir)
  );

  const dependencyRootPaths: AbsolutePath[] = [];
  await projectDependenciesManager.forEachAsync((dependency) => {
    dependencyRootPaths.push(dependency.getRootPath());
  });

  expect(dependencyRootPaths.length).toBe(1);
});
