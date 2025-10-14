/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModuleRegistry } from 'react-native';
import type { TurboModule } from 'react-native/Libraries/TurboModule/RCTExport';

export interface Spec extends TurboModule {
  runComputeIntensiveTask(): void;
  runAsyncTask(): Promise<number>;
  getRNVersionFromUIContext(prefix: string): Promise<string>;
  showAlert(): void;
  emitDeviceEvent(eventName: string, payload: number): void;
  testCommunicationBetweenWorkerAndCpp(): Promise<boolean>;
  getArchitecture(): string;
  testCommunicatingWithOtherWorkerTurboModules(): Promise<boolean>;
}

export default TurboModuleRegistry.get<Spec>('SampleWorkerTurboModule')!;
