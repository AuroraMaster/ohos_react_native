/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { TurboModuleContext } from '../../RNOH/TurboModule';
import { TurboModule } from '../../RNOH/TurboModule';
import promptAction from '@ohos.promptAction';
import { BusinessError } from '@ohos.base';
import { RNOHLogger } from '../../RNOH/RNOHLogger';

const ToastAndroidConstants = {
  SHORT: 2000,
  LONG: 3500,
  TOP: 0,
  BOTTOM: 1,
  CENTER: 2,
}

export class ToastAndroidTurboModule extends TurboModule {
  public static readonly NAME = 'ToastAndroid';
  private logger: RNOHLogger;

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    this.logger = this.ctx.logger.clone("ToastAndroidTurboModule")

  }

  getConstants() {
    return ToastAndroidConstants;
  }

  show(message: string, duration: number) {
    try {
      promptAction.showToast({
        message,
        duration,
      })
    } catch (error) {
      const errorMessage = (error as BusinessError).message;
      const errorCode = (error as BusinessError).code;
      this.logger.error(`Show toast failed with code ${errorCode} and message ${errorMessage}`)
    }
  }

  showWithGravity(message: string, duration: number, _gravity: number) {
    this.show(message, duration);
  }

  showWithGravityAndOffset(message: string, duration: number, _gravity: number, _xOffset: number, _yOffset: number) {
    this.show(message, duration);
  }
}