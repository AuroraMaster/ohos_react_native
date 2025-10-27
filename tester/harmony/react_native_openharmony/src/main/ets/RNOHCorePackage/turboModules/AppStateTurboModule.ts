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
    let applicationStateChangeCallback: ApplicationStateChangeCallback = {
      onApplicationForeground: () => {
        this.state = "FOREGROUND"
        this.ctx.rnInstance.emitDeviceEvent("appStateDidChange", { app_state: this.getAppState() });
      },
      onApplicationBackground: () => {
        this.state = "BACKGROUND"
        this.ctx.rnInstance.emitDeviceEvent("appStateDidChange", { app_state: this.getAppState() });
      },
    }
    let appStateFocusCallback: Callback<emitter.EventData> = (eventData: emitter.EventData) => {
      this.ctx.rnInstance.emitDeviceEvent("appStateFocusChange", true);
    };
    let appStateBLURCallback: Callback<emitter.EventData> = (eventData: emitter.EventData) => {
      this.ctx.rnInstance.emitDeviceEvent("appStateFocusChange", false);
    };
    emitter.on("APP_STATE_FOCUS", appStateFocusCallback);
    emitter.on("APP_STATE_BLUR", appStateBLURCallback);

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
      applicationContext.on('applicationStateChange', applicationStateChangeCallback);
      envCallbackID = applicationContext.on('environment', envCallback);
    }
    this.cleanUpCallbacks.push(() => {
      emitter.off("APP_STATE_FOCUS", appStateFocusCallback);
      emitter.off("APP_STATE_BLUR", appStateBLURCallback);
      applicationContext.off("environment", envCallbackID);
      applicationContext.off('applicationStateChange', applicationStateChangeCallback);
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