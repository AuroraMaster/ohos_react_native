/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import json5 from 'json5';
import fs from 'node:fs';
import { AbsolutePath } from './AbsolutePath';

export class OHPackageJSON5 {
  static fromPath(ohPackageJson5Path: AbsolutePath) {
    return new OHPackageJSON5(
      json5.parse(
        fs.readFileSync(ohPackageJson5Path.getValue(), { encoding: 'utf-8' })
      )
    );
  }

  private constructor(private rawData: Record<string, any>) {}

  get name(): string {
    return this.rawData['name'];
  }

  get version(): string {
    return this.rawData['version'];
  }
}
