/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Config, Command } from '@react-native-community/cli-types';
import {
  commandBundleHarmony,
  commandPackHarmony,
  commandUnpackHarmony,
  commandCreateMetroConstantsHarmony,
  commandCodegenHarmony,
  commandVerifyPackageHarmony,
  commandCodegenLibHarmony,
  commandLinkHarmony,
  commandRunHarmony,
} from './commands';

export const config = {
  commands: [
    commandBundleHarmony,
    commandPackHarmony,
    commandUnpackHarmony,
    commandCreateMetroConstantsHarmony,
    commandCodegenHarmony,
    commandVerifyPackageHarmony,
    commandCodegenLibHarmony,
    commandLinkHarmony,
    commandRunHarmony,
  ] as Command[],
} satisfies Partial<Config>;
