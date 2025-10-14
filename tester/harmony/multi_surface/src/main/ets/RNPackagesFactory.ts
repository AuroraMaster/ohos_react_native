/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type {RNPackageContext, RNPackage} from 'rnoh/ts';
import {SamplePackage} from 'rnoh-sample-package/ts';

export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [new SamplePackage(ctx)];
}
