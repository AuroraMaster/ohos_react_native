/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import { ValidationError } from '../src/tasks/sync/SyncTask';
import { memfs, NestedDirectoryJSON } from 'memfs';
import pathUtils from 'node:path';
import { StubCommandExecutor, FakeLogger, FakeSyncTask } from './__fixtures__';

const ORIGINAL_DEVECO_SDK_HOME = process.env.DEVECO_SDK_HOME;

beforeEach(() => {
  process.env.DEVECO_SDK_HOME = '/Applications/DevEco-Studio.app/Contents/sdk';
});

afterEach(() => {
  process.env.DEVECO_SDK_HOME = ORIGINAL_DEVECO_SDK_HOME;
});

function createSyncTask(options: {
  onCodegenRun?: (command: string) => string;
  fsSetup?: NestedDirectoryJSON;
}) {
  const fakeCliExecutor = new StubCommandExecutor(
    options.onCodegenRun ?? (() => '')
  );
  const fakeLogger = new FakeLogger();

  const SyncTask = new FakeSyncTask(
    fakeCliExecutor,
    fakeLogger,
    memfs(options.fsSetup, './').fs
  );
  return { SyncTask, fakeCliExecutor, fakeLogger };
}

it('should handle port forwarding, call codegen-harmony, and log progress', () => {
  const { SyncTask, fakeCliExecutor, fakeLogger } = createSyncTask({
    onCodegenRun: () => '__GENERATED_FILE__',
    fsSetup: {
      node_modules: {
        '.bin': {
          'react-native': '',
        },
      },
    },
  });

  SyncTask.run({
    nodeModulesPath: './node_modules',
    codegen: { rnohModulePath: '_' },
  });

  const executedCommands = fakeCliExecutor.getCommands();
  expect(executedCommands[0]).toContain(
    '"/test/path" rport tcp:8081 tcp:8081'
  );
  expect(executedCommands[1]).toBe(
    pathUtils.join('node_modules', '.bin', 'react-native') +
      ` codegen-harmony --project-root-path .. --cpp-output-path entry${pathUtils.sep}src${pathUtils.sep}main${pathUtils.sep}cpp${pathUtils.sep}generated --ets-output-path _`
  );
  expect(
    fakeLogger
      .getLogs()
      .map((log) => log.msg)
      .includes('[codegen]\n__GENERATED_FILE__')
  ).toBeTruthy();
});

it("should fail if node_modules dir doesn't exist", () => {
  const { SyncTask } = createSyncTask({});

  expect(() => {
    SyncTask.run({
      nodeModulesPath: './NOT_EXISTING_DIR',
      codegen: { rnohModulePath: '_' },
    });
  }).toThrow(ValidationError);
});

it('should allow skipping codegen process', () => {
  const { SyncTask, fakeCliExecutor, fakeLogger } = createSyncTask({
    fsSetup: {
      node_modules: {
        '.bin': {
          'react-native': '',
        },
      },
    },
  });

  SyncTask.run({
    nodeModulesPath: './node_modules',
    codegen: null,
  });

  expect(
    fakeCliExecutor
      .getCommands()
      .some((command) => command.includes('codegen-harmony'))
  ).toBeFalsy();
  expect(
    fakeLogger.getLogs().some((log) => log.msg.includes('[codegen] skipped'))
  ).toBeTruthy();
});

it('should allow skipping Metro setup', () => {
  const { SyncTask, fakeCliExecutor, fakeLogger } = createSyncTask({
    fsSetup: {
      node_modules: {
        '.bin': {
          'react-native': '',
        },
      },
    },
  });

  SyncTask.run({
    nodeModulesPath: './node_modules',
    metro: null,
    codegen: null,
  });

  expect(
    fakeCliExecutor.getCommands().some((command) => command.includes('rport'))
  ).toBeFalsy();
  expect(
    fakeLogger.getLogs().some((log) => log.msg.includes('[metro] skipped'))
  ).toBeTruthy();
});

it('should call autolinking', () => {
  const { SyncTask, fakeCliExecutor } = createSyncTask({
    fsSetup: {
      node_modules: {
        '.bin': {
          'react-native': '',
        },
      },
    },
  });

  SyncTask.run({
    nodeModulesPath: './node_modules',
    codegen: null,
    metro: null,
  });

  expect(
    fakeCliExecutor
      .getCommands()
      .some((command) => command.includes('link-harmony'))
  ).toBeTruthy();
});

it('should skip autolinking', () => {
  const { SyncTask, fakeCliExecutor } = createSyncTask({
    fsSetup: {
      node_modules: {
        '.bin': {
          'react-native': '',
        },
      },
    },
  });

  SyncTask.run({
    nodeModulesPath: './node_modules',
    codegen: null,
    metro: null,
    autolinking: null,
  });

  expect(
    fakeCliExecutor
      .getCommands()
      .every((command) => !command.includes('link-harmony'))
  ).toBeTruthy();
});
