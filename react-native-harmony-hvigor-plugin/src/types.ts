/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import { IFs } from 'memfs';

/**
 * @actor RNOH_APP
 */
export type CodegenConfig = {
  rnohModulePath?: string;
  etsOutputPath?: string;
  cppOutputPath?: string;
  projectRootPath?: string;
  debug?: boolean;
  noSafetyCheck?: boolean;
};

/**
 * @actor RNOH_APP
 */
export type MetroConfig = {
  port?: number;
};

/**
 * @actor RNOH_APP
 */
export type AutolinkingConfig = {
  ohPackagePath?: string;
  etsRNOHPackagesFactoryPath?: string;
  cppRNOHPackagesFactoryPath?: string;
  cmakeAutolinkPath?: string;
  excludeNpmPackages?: string[];
  includeNpmPackages?: string[];
};

/**
 * @actor RNOH_APP
 */
export type RNOHModulePluginOptions = {
  nodeModulesPath?: string;
  metro?: MetroConfig | null;
  codegen: CodegenConfig | null;
  autolinking?: AutolinkingConfig | null;
};

/**
 * @actor RNOH_APP
 */
export class RNOHModulePluginError extends Error {}

export type FS = Pick<IFs, 'existsSync'>;

export interface ILogger {
  info(message: string): void;

  warn(message: string): void;

  error(message: string): void;
}

export interface Subtask {
  run(): void;
}
