/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import I18n from '@ohos.i18n';
import { AnyThreadTurboModule, UITurboModuleContext } from "../../RNOH/ts"
import { EtsWorkerTurboModuleContext } from '../../RNOH';
import { UITaskRunnable } from '../../RNOH/UITaskRunner';

@Sendable
class updateRTLRunnableParams {
  public isRTL: boolean

  constructor(isRTL: boolean) {
    this.isRTL = isRTL
  }
}

@Sendable
class updateRTLRunnable implements UITaskRunnable<updateRTLRunnableParams> {
  run(uiTurboModuleContext: UITurboModuleContext, params: updateRTLRunnableParams): void {
    uiTurboModuleContext.rnInstance.updateRTL(params.isRTL);
  }
}

interface I18nConstants {
  isRTL: boolean,
  doLeftAndRightSwapInRTL: boolean,
  localeIdentifier: string | undefined
}

export class I18nManagerTurboModule extends AnyThreadTurboModule {
  public static readonly NAME = 'I18nManager';

  private RTLAllowed = true;
  private RTLForced = false;

  get isRTL() {
    return this.RTLForced || (this.RTLAllowed && I18n.isRTL(this.ctx.uiAbilityContext.config.language));
  }

  getConstants(): Object {
    return {
      isRTL: this.isRTL,
      doLeftAndRightSwapInRTL: true,
      localeIdentifier: this.ctx.uiAbilityContext.config.language
    } as I18nConstants;
  }

  allowRTL(allow: boolean) {
    this.RTLAllowed = allow;
    if (this.ctx instanceof EtsWorkerTurboModuleContext) {
      this.ctx.runOnUIThread(new updateRTLRunnable(), new updateRTLRunnableParams(this.isRTL))
    } else if (this.ctx instanceof UITurboModuleContext) {
      this.ctx.rnInstance.updateRTL(this.isRTL);
    }
  }

  forceRTL(force: boolean) {
    this.RTLForced = force;
    if (this.ctx instanceof EtsWorkerTurboModuleContext) {
      this.ctx.runOnUIThread(new updateRTLRunnable(), new updateRTLRunnableParams(this.isRTL))
    } else if (this.ctx instanceof UITurboModuleContext) {
      this.ctx.rnInstance.updateRTL(this.isRTL);
    }
  }
}
