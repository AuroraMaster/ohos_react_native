/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Check } from '../types';

export function createNpmAndOhPackageVersionsAreEqualCheck(
  npmPackageVersion: string,
  ohPackageVersion: string
): Check {
  return {
    checkIf: 'npm_and_oh_package_versions_are_equal',
    run: async () => {
      return npmPackageVersion === ohPackageVersion
        ? 'pass'
        : {
            status: 'fail',
            message: `expected OH package version (${ohPackageVersion}) to match NPM version (${npmPackageVersion})`,
          };
    },
  };
}
