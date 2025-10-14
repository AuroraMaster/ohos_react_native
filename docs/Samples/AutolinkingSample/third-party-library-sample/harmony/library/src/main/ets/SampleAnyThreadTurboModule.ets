/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AnyThreadTurboModule } from "@rnoh/react-native-openharmony";
import { TM } from "./generated/ts"

export class SampleAnyThreadTurboModule extends AnyThreadTurboModule implements TM.SampleAnyThreadTurboModule.Spec {
  public static readonly NAME = TM.SampleAnyThreadTurboModule.NAME;

  power(a: number, b: number): Promise<number> {
    this.ctx.logger.info("AutoLinkSample.AnyThreadTurboModule.power");
    return new Promise(resolve => {
      setTimeout(() => resolve(Math.pow(a, b)), 3000)
    })
  }
}