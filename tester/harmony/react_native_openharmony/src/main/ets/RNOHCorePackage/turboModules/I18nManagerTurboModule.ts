/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import I18n from '@ohos.i18n';
import { TurboModule } from "../../RNOH/TurboModule";

export class I18nManagerTurboModule extends TurboModule {
  public static readonly NAME = 'I18nManager';

  private RTLAllowed = true;
  private RTLForced = false;

  get isRTL() {
    return this.RTLForced || (this.RTLAllowed && I18n.isRTL(this.ctx.uiAbilityContext.config.language));
  }

  getConstants() {
    return {
      isRTL: this.isRTL,
      doLeftAndRightSwapInRTL: true,
      localeIdentifier: this.ctx.uiAbilityContext.config.language
    };
  }

  allowRTL(allow: boolean) {
    this.RTLAllowed = allow;
    this.ctx.rnInstance.updateRTL(this.isRTL);
  }

  forceRTL(force: boolean) {
    this.RTLForced = force;
    this.ctx.rnInstance.updateRTL(this.isRTL);
  }
}
