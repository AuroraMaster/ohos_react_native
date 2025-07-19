/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Command } from './types';
import { getLoader, Loader } from '@react-native-community/cli-tools';
import chalk from 'chalk';
import { DescriptiveError, AbsolutePath } from '../core';
import { Logger } from '../io';
import { CheckResult, verifyRNOHPackage } from '../package-verifier';

export const commandVerifyPackageHarmony: Command = {
  name: 'verify-package-harmony',
  description: 'Validates that a package is compliant with guidelines',
  options: [
    {
      name: '--package-path <path>',
      description: 'Path to the npm package',
    },
    {
      name: '--no-harmony-code [boolean]',
      description: 'Disable har verification',
    },
    {
      name: '--skip-checks <string>',
      description: 'Disable certain checks by their names separated by ";"',
    },
  ],
  func: async (argv, config, rawArgs: any) => {
    const loader = getLoader();
    try {
      const args = validateArgs(rawArgs);
      loader.info(`Verifying RNOH package: ${args.packagePath.getValue()}`);
      const checkResults: CheckResult[] = [];
      for await (const checkResult of verifyRNOHPackage({
        npmPackagePath: args.packagePath,
        hasHarmonyCode: args.hasHarmonyCode,
        checkNamesToSkip: args.checkNamesToSkip,
      })) {
        if (checkResult.type === 'START_PROCESS') {
          loader.start(chalk.gray(checkResult.name));
        } else if (checkResult.type === 'FINISHED_CHECKING') {
          checkResults.push(checkResult);
          logCheckResult(loader, checkResult);
        }
      }
      if (
        checkResults.every(
          (checkResult) =>
            checkResult.status === 'pass' || checkResult.status === 'skip'
        )
      ) {
        loader.info(chalk.bold('OK'));
      } else {
        loader.info(
          chalk.red(
            'Package verification failed. Please fix those checks before publishing your package.'
          )
        );
        process.exit(1);
      }
    } catch (err) {
      if (err instanceof DescriptiveError) {
        new Logger().descriptiveError(err);
        process.exit(1);
      }
      throw err;
    }
  },
};

type Args = {
  packagePath: AbsolutePath;
  hasHarmonyCode: boolean;
  checkNamesToSkip: string[];
};

function validateArgs(args: any): Args {
  if (!args.packagePath) {
    throw new DescriptiveError({
      whatHappened: 'package-path is undefined',
      whatCanUserDo: ['Please provide --package-path'],
    });
  }
  return {
    packagePath: new AbsolutePath(args.packagePath),
    hasHarmonyCode: args.harmonyCode,
    checkNamesToSkip: ((args.skipChecks ?? '') as string).split(';'),
  };
}

function logCheckResult(loader: Loader, checkResult: CheckResult) {
  const prefix = (() => {
    switch (checkResult.status) {
      case 'pass':
        return `${chalk.green('PASS')}`;
      case 'fail':
        return `${chalk.red('FAIL')}`;
      case 'skip':
        return `${chalk.yellow('SKIP')}`;
    }
  })();
  loader.info(
    checkResult.message
      ? `[${chalk.bold(prefix)}] ${chalk.bold(checkResult.name)}\n${
          checkResult.message
        }`
      : `[${chalk.bold(prefix)}] ${chalk.bold(checkResult.name)}`
  );
}
