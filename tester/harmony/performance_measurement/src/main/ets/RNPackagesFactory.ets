/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type {RNPackageContext, RNPackage} from 'rnoh/ts';
/**
 * Importing ets files from top level barrel file doesn't work.
 */
import SamplePackage from 'rnoh-sample-package/src/main/ets/SamplePackage';

import { FsPackage } from '@react-native-oh-tpl/react-native-fs/ts';
import { SafeAreaViewPackage } from '@react-native-oh-tpl/react-native-safe-area-context/src/main/ets/SafeAreaViewPackage'

export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [new SamplePackage(ctx), new FsPackage(ctx), new SafeAreaViewPackage(ctx)];
}
