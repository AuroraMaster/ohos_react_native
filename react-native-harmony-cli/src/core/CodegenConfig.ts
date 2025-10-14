/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import fs from 'node:fs';
import { AbsolutePath } from './AbsolutePath';
import { findFilePathsWithExtensions } from '../io';
import { DescriptiveError } from './DescriptiveError';

export type RawCodegenConfig = {
  specPaths: string[];
  version?: number;
};

export class CodegenConfig {
  static fromRawCodegenConfig(
    rawCodegenConfig: RawCodegenConfig,
    packageRootPath: AbsolutePath,
    projectRootPath: AbsolutePath,
    packageName: string
  ) {
    const SUPPORTED_SPEC_EXTENSIONS = ['js', 'jsx', 'ts', 'tsx'];
    const specPaths = (rawCodegenConfig.specPaths ?? []).map((rawSpecPath) => {
      if (rawSpecPath.startsWith('~')) {
        return projectRootPath.copyWithNewSegment(rawSpecPath.replace('~', ''));
      } else {
        return packageRootPath.copyWithNewSegment(rawSpecPath);
      }
    });
    const specFilePaths = specPaths.flatMap((specPath) => {
      if (!fs.existsSync(specPath.getValue())) {
        throw new DescriptiveError({
          whatHappened: `No such file or directory: ${specPath.getValue()}`,
          whatCanUserDo: [
            `Please verify "${packageName}/package.json::harmony::codegenConfig". If you are not the maintainer of this package, please report this problem to the package maintainer.`,
          ],
        });
      }
      if (fs.lstatSync(specPath.getValue()).isDirectory()) {
        return findFilePathsWithExtensions(specPath, SUPPORTED_SPEC_EXTENSIONS);
      } else {
        const ext = specPath.getExtension();
        if (ext && SUPPORTED_SPEC_EXTENSIONS.includes(ext)) {
          return [specPath];
        }
        return [];
      }
    });
    return new CodegenConfig(specFilePaths, rawCodegenConfig.version ?? 1);
  }

  private constructor(
    private specFilePaths: AbsolutePath[],
    private version: number
  ) {}

  getSpecFilePaths(): AbsolutePath[] {
    return this.specFilePaths;
  }

  getVersion(): number {
    return this.version;
  }
}
