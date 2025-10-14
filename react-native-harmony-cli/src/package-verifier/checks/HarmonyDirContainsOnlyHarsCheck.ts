/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import fs from 'node:fs';
import { AbsolutePath } from '../../core';
import { Check } from '../types';

export function createHarmonyDirContainsOnlyHarsCheck(
  npmPackagePath: AbsolutePath,
  hasNpmPackageNativeCode: boolean
): Check {
  return {
    checkIf: 'harmony_directory_contains_only_one_har_file',
    run: async () => {
      if (!hasNpmPackageNativeCode) {
        return 'skip';
      }
      const harmonyPath = npmPackagePath
        .copyWithNewSegment('harmony')
        .getValue();
      const fileOrDirNames = fs.readdirSync(harmonyPath);
      if (fileOrDirNames.length !== 1) {
        return { status: 'fail', message: JSON.stringify(fileOrDirNames) };
      }
      if (!fileOrDirNames[0].endsWith('.har')) {
        return 'fail';
      }
      return 'pass';
    },
  };
}
