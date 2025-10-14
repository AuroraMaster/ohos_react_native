/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import fs from 'node:fs';
import tmp from 'tmp';
import {
  AbsolutePath,
  DescriptiveError,
  OHPackageJSON5,
  PackageJSON,
} from '../core';
import {
  createHarmonyDirContainsOnlyHarsCheck,
  createHarmonyDirExistsCheck,
  createOhPackageNameFollowsNamingConventionCheck,
  createNpmAndOhPackageVersionsAreEqualCheck,
} from './checks';
import { Check, CheckStatus } from './types';
import { RealFS, unpackTarGz } from '../io';

type StartProcessMessage = {
  type: 'START_PROCESS';
  name: string;
};

type FinishedCheckingMessage = {
  type: 'FINISHED_CHECKING';
  name: string;
  status: CheckStatus;
  message: string;
};

type VerificationMessage = StartProcessMessage | FinishedCheckingMessage;

export async function* verifyRNOHPackage({
  npmPackagePath,
  hasHarmonyCode,
  checkNamesToSkip,
}: {
  npmPackagePath: AbsolutePath;
  hasHarmonyCode: boolean;
  checkNamesToSkip: string[];
}): AsyncGenerator<VerificationMessage, void, unknown> {
  const npmPackageJson = PackageJSON.fromProjectRootPath(
    new RealFS(),
    npmPackagePath,
    npmPackagePath
  );
  const initialChecks: Check[] = [
    createHarmonyDirExistsCheck(npmPackagePath, hasHarmonyCode),
    createHarmonyDirContainsOnlyHarsCheck(npmPackagePath, hasHarmonyCode),
  ];
  yield* runChecks(initialChecks, checkNamesToSkip);
  if (hasHarmonyCode) {
    const harPath = findHarPath(npmPackagePath);
    if (!harPath) {
      throw new DescriptiveError({
        whatHappened: 'Attempted to verify har but no har was found',
        whatCanUserDo: [
          'Run this command without --no-harmony-code flag.',
          'Create har files by running DevEco Studio > Build > Make Module (MODULE_NAME). The har file can be later found in MODULE_NAME/build/default/outputs/default/MODULE_NAME.har. Copy that file to the PACKAGE_ROOT/harmony directory.',
        ],
      });
    }
    const tmpDirPath = new AbsolutePath(tmp.dirSync().name);
    yield {
      type: 'START_PROCESS',
      name: `Unpacking the har file to the tmpdir`,
    };
    await unpackTarGz(harPath, tmpDirPath);
    const ohPackageJson5 = OHPackageJSON5.fromPath(
      tmpDirPath.copyWithNewSegment('package', 'oh-package.json5')
    );
    const harChecks: Check[] = [
      createOhPackageNameFollowsNamingConventionCheck(
        npmPackageJson.name,
        ohPackageJson5.name
      ),
      createNpmAndOhPackageVersionsAreEqualCheck(
        npmPackageJson.version,
        ohPackageJson5.version
      ),
    ];
    yield* runChecks(harChecks, checkNamesToSkip);
  }
}

async function* runChecks(
  checks: Check[],
  checkNamesToSkip: string[]
): AsyncGenerator<VerificationMessage, void, unknown> {
  for (const { checkIf, run } of checks) {
    if (checkNamesToSkip.includes(checkIf)) {
      yield {
        type: 'FINISHED_CHECKING',
        name: checkIf,
        status: 'skip' as CheckStatus,
        message: '',
      };
      continue;
    }
    const result = await run();
    const status = typeof result === 'string' ? result : result.status;
    const message = typeof result === 'string' ? '' : result.message;
    yield { type: 'FINISHED_CHECKING', name: checkIf, status, message };
  }
}

function findHarPath(packagePath: AbsolutePath): AbsolutePath | null {
  const packageHarmonyPath = packagePath.copyWithNewSegment('harmony');
  const fileOrDirNames = fs.readdirSync(packageHarmonyPath.getValue());
  const harFilenames = fileOrDirNames.filter((fileOrDirName) =>
    fileOrDirName.endsWith('.har')
  );
  if (harFilenames.length != 1) {
    return null;
  }
  return packageHarmonyPath.copyWithNewSegment(harFilenames[0]);
}
