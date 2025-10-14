/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModule, TurboModuleRegistry } from "react-native";

// 使用自定义的TurboModules运行在worker线程
export interface SpecWorkerSampleTurboModule extends TurboModule {
  pushStringToHarmony(arg: string, id?: number): string;
  registerFunction(onComplete: (value: string) => void): void;
  doAsyncJob(shouldResolve: boolean): Promise<string>;
  rnLog(info: string): void;
}

export default TurboModuleRegistry.getEnforcing<SpecWorkerSampleTurboModule>('SampleWorkerTurboModule');