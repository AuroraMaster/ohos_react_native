/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModuleRegistry } from 'react-native';
import { Spec } from './specs/v2/NativeGeneratedSampleTurboModule';

// Those types are not supported by codegen: null, any, never
interface SampleTurboModule extends Spec {
  getNull(arg: null): null;
  getArray(args: any[]): any[];
  displayRNOHError(data: {
    whatHappened: string;
    howCanItBeFixed: string[];
    extraData?: any;
    customStack?: string;
  }): void;
  throwExceptionCpp(): never;
  throwExceptionArk(): never;
}

export default TurboModuleRegistry.get<SampleTurboModule>('SampleTurboModule')!;
