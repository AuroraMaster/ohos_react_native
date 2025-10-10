/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import Case from 'case';
import { execaCommandSync, execa, execaSync } from 'execa';
import pathUtils from 'path';
import { AbsolutePath } from '../src/core';
export class ReactNativeFixture {
  private cwd: string;
  constructor(cwd: string | AbsolutePath) {
    if (cwd instanceof AbsolutePath) {
      this.cwd = cwd.getValue();
    } else {
      this.cwd = cwd;
    }
  }

  help() {
    return execaCommandSync(`react-native --help`).stdout;
  }

  packHarmony(args: {
    harmonyDirPath: string;
    ohModulePath: string;
    packageJSONPath: string;
  }) {
    return execaCommandSync(
      `react-native pack-harmony --harmony-dir-path ${this.useCwd(
        args.harmonyDirPath
      )} --oh-module-path ${this.useCwd(
        args.ohModulePath
      )} --package-json-path ${this.useCwd(args.packageJSONPath)}`
    ).stdout;
  }
  unpackHarmony(args: { projectRootPath: string; outputDir: string }) {
    return execaSync(
      'react-native',
      [
        'unpack-harmony',
        '--project-root-path',
        this.useCwd(args.projectRootPath),
        '--output-dir',
        this.useCwd(args.outputDir),
      ],
      {
        env: { FORCE_COLOR: '0' },
        shell: true,
      }
    ).stdout;
  }

  createMetroHostConstantsHarmony(args: { output: string }) {
    return execaCommandSync(
      `react-native create-metro-host-constants-harmony --output ${this.useCwd(
        args.output
      )}`
    ).stdout;
  }

  codegenHarmony(args: {
    cppOutputPath: string;
    projectRootPath: string;
    rnohModulePath?: string;
  }) {
    return execaCommandSync(
      `react-native codegen-harmony --cpp-output-path ${this.useCwd(
        args.cppOutputPath
      )} --project-root-path ${this.useCwd(
        args.projectRootPath
      )} --rnoh-module-path ${this.useCwd(
        args.rnohModulePath ?? './harmony/rnoh'
      )} --no-safety-check`
    ).stdout;
  }

  codegenLibHarmony(
    args:
      | { help: true }
      | {
          npmPackageName: string;
          cppOutputPath: string;
          etsOutputPath: string;
          cppComponentsSpecPaths?: string;
          arktsComponentsSpecPaths?: string;
          turboModulesSpecPaths?: string;
          noSafetyCheck?: boolean;
        }
  ) {
    return execaCommandSync(
      `react-native codegen-lib-harmony ${this.createCliArgs(args)}`
    ).stdout;
  }

  private createCliArgs(
    args: Record<string, string | number | boolean>
  ): string {
    return Object.entries(args)
      .filter(([_, value]) => {
        if (typeof value === 'boolean') {
          return value;
        }
        return value !== undefined && value !== null;
      })
      .map(([key, value]) => {
        const formattedKey = Case.kebab(key);

        if (typeof value === 'boolean') {
          return `--${formattedKey}`;
        }

        if (
          (key.endsWith('Path') || key.endsWith('Paths')) &&
          typeof value === 'string'
        ) {
          return `--${formattedKey} ${this.useCwd(value)}`;
        }

        return `--${formattedKey} ${value}`;
      })
      .join(' ');
  }

  private useCwd(relPath: string) {
    return pathUtils.join(this.cwd, relPath);
  }
}
