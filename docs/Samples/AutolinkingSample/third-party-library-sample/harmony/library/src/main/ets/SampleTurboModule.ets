/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { EtsUITurboModule } from '@rnoh/react-native-openharmony/ets';
import { TM } from "./generated/ts"

export class SampleTurboModule extends EtsUITurboModule implements TM.SampleTurboModule.Spec {
  public static readonly NAME = TM.SampleTurboModule.NAME;

  add(a: number, b: number): number {
    this.ctx.logger.info("AutoLinkSample.SampleTurboModule.add");
    return a + b;
  }
}