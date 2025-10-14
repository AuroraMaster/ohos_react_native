/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { TurboModule } from 'react-native/Libraries/TurboModule/RCTExport';
import { TurboModuleRegistry } from 'react-native';

interface LogTurboModuleProtocol {
  Log(arg: string): void;
}

interface Spec extends TurboModule, LogTurboModuleProtocol { }
const LogTurboModule = TurboModuleRegistry.get<Spec>('RNCLog')!;
export function HMLog(arg: string) {
  LogTurboModule.Log(arg);
}
