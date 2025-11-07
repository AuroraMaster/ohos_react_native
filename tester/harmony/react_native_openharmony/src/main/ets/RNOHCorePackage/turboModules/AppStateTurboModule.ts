/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AnyThreadTurboModuleContext } from "../../RNOH/RNOHContext";
import { AnyThreadTurboModule } from "../../RNOH/TurboModule";
import type EnvironmentCallback from '@ohos.app.ability.EnvironmentCallback';
import { AbilityConstant, ApplicationStateChangeCallback } from '@kit.AbilityKit';
import { emitter, Callback } from '@kit.BasicServicesKit';

export class AppStateTurboModule extends AnyThreadTurboModule {
  public static readonly NAME = 'AppState';
  private state: string = 'FOREGROUND';

  private cleanUpCallbacks: (() => void)[] = [];

  constructor(protected ctx: AnyThreadTurboModuleContext) {
    super(ctx);
    this.subscribeListeners();
  }

  private subscribeListeners() {
    const applicationContext = this.ctx.uiAbilityContext.getApplicationContext();
    const foregroundEvent = `FOREGROUND_${this.ctx.rnInstance.getId()}`;
    const backgroundEvent = `BACKGROUND_${this.ctx.rnInstance.getId()}`;
    const focusEvent = `APP_STATE_FOCUS_${this.ctx.rnInstance.getId()}`;
    const blurEvent = `APP_STATE_BLUR_${this.ctx.rnInstance.getId()}`;

    const applicationForegroundCallback: Callback<emitter.EventData> = (eventData: emitter.EventData) => {
      this.state = "FOREGROUND";
      this.ctx.rnInstance.emitDeviceEvent("appStateDidChange", {
        app_state: this.getAppState()
      });
    };

    const applicationBackgroundCallback: Callback<emitter.EventData> = (eventData: emitter.EventData) => {
      this.state = "BACKGROUND";
      this.ctx.rnInstance.emitDeviceEvent("appStateDidChange", {
        app_state: this.getAppState()
      });
    };

    const appStateFocusCallback: Callback<emitter.EventData> = (eventData: emitter.EventData) => {
      this.ctx.rnInstance.emitDeviceEvent("appStateFocusChange", true);
    };

    const appStateBlurCallback: Callback<emitter.EventData> = (eventData: emitter.EventData) => {
      this.ctx.rnInstance.emitDeviceEvent("appStateFocusChange", false);
    };

    emitter.on(foregroundEvent, applicationForegroundCallback);
    emitter.on(backgroundEvent, applicationBackgroundCallback);
    emitter.on(focusEvent, appStateFocusCallback);
    emitter.on(blurEvent, appStateBlurCallback);

    let envCallback: EnvironmentCallback = {
      onConfigurationUpdated: () => {
      },

      onMemoryLevel: (level: AbilityConstant.MemoryLevel) => {
        if (level == AbilityConstant.MemoryLevel.MEMORY_LEVEL_CRITICAL) {
          this.ctx.rnInstance.emitDeviceEvent("memoryWarning", null);
        }
      },
    };

    let envCallbackID: number;
    if (applicationContext != undefined) {
      envCallbackID = applicationContext.on('environment', envCallback);
    }
    this.cleanUpCallbacks.push(() => {
      emitter.off(foregroundEvent);
      emitter.off(backgroundEvent);
      emitter.off(focusEvent);
      emitter.off(blurEvent);
      applicationContext.off("environment", envCallbackID);
    });
  }

  private getAppState() {
    return this.state === "FOREGROUND" ? "active" : "background";
  }

  getConstants() {
    return { initialAppState: this.getAppState() };
  }

  getCurrentAppState(success: (appState: { app_state: string }) => void, error: (error: Error) => void) {
    success({ app_state: this.getAppState() });
  };

  __onDestroy__() {
    super.__onDestroy__();
    this.cleanUpCallbacks.forEach(cb => cb());
    this.cleanUpCallbacks = [];
  }
}