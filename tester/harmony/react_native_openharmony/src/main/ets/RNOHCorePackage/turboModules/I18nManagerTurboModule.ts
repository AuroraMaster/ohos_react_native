/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { TurboModuleContext } from '../../RNOH/TurboModule';
import I18n from '@ohos.i18n';
import { TurboModule } from "../../RNOH/TurboModule";

export class I18nManagerTurboModule extends TurboModule {
  public static readonly NAME = 'I18nManager';

  private RTLAllowed = true;
  private RTLForced = false;
  private lastLanguage: string | null = null;
  private cleanUpCallbacks: (() => void)[] = [];

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    this.lastLanguage = this.ctx.uiAbilityContext.config.language;
    this.subscribeToLanguageChanges();
  }

  private subscribeToLanguageChanges() {
    // Monitor for configuration update events and detect language changes.
    const onConfigurationUpdate = (newConfig: any) => {
      const newLanguage = newConfig?.language || this.ctx.uiAbilityContext.config.language;

      if (this.lastLanguage !== newLanguage) {
        this.lastLanguage = newLanguage;
        const newIsRTL = this.RTLForced || (this.RTLAllowed && I18n.isRTL(newLanguage));
        const eventPayload = {
          localeIdentifier: newLanguage,
          isRTL: newIsRTL
        };
        this.ctx.rnInstance.emitDeviceEvent("i18nManagerLanguageChanged", eventPayload);
        this.ctx.rnInstance.updateRTL(newIsRTL);
      }
    };

    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.subscribeToLifecycleEvents("CONFIGURATION_UPDATE", onConfigurationUpdate)
    );
  }

  get isRTL() {
    const currentLanguage = this.lastLanguage || this.ctx.uiAbilityContext.config.language;
    return this.RTLForced || (this.RTLAllowed && I18n.isRTL(currentLanguage));
  }

  getConstants() {
    const currentLanguage = this.lastLanguage || this.ctx.uiAbilityContext.config.language;
    const currentIsRTL = this.RTLForced || (this.RTLAllowed && I18n.isRTL(currentLanguage));

    return {
      isRTL: currentIsRTL,
      doLeftAndRightSwapInRTL: true,
      localeIdentifier: currentLanguage
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

  __onDestroy__() {
    super.__onDestroy__();
    this.cleanUpCallbacks.forEach(cb => cb());
    this.cleanUpCallbacks = [];
  }
}

