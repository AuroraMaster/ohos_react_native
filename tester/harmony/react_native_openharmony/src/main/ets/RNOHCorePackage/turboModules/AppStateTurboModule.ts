/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { TurboModuleContext } from "../../RNOH/TurboModule";
import { TurboModule } from "../../RNOH/TurboModule";
import type EnvironmentCallback from '@ohos.app.ability.EnvironmentCallback';
import { AbilityConstant } from '@kit.AbilityKit';

export class AppStateTurboModule extends TurboModule {
  public static readonly NAME = 'AppState';

  private cleanUpCallbacks: (() => void)[] = [];

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    this.subscribeListeners();
  }

  private subscribeListeners() {
    this.ctx.rnInstance.subscribeToLifecycleEvents("FOREGROUND", () => {
      this.ctx.rnInstance.emitDeviceEvent("appStateDidChange", { app_state: this.getAppState() });
    })
    this.ctx.rnInstance.subscribeToLifecycleEvents("BACKGROUND", () => {
      this.ctx.rnInstance.emitDeviceEvent("appStateDidChange", { app_state: this.getAppState() });
    })
    this.ctx.rnInstance.subscribeToStageChangeEvents("APP_STATE_FOCUS", () => {
      this.ctx.rnInstance.emitDeviceEvent("appStateFocusChange",  true);
    })
    this.ctx.rnInstance.subscribeToStageChangeEvents("APP_STATE_BLUR", () => {
      this.ctx.rnInstance.emitDeviceEvent("appStateFocusChange", false);
    })
    let envCallback: EnvironmentCallback = {
      onConfigurationUpdated: () => {
      },

      onMemoryLevel: (level: AbilityConstant.MemoryLevel) => {
        if (level == AbilityConstant.MemoryLevel.MEMORY_LEVEL_CRITICAL) {
          this.ctx.rnInstance.emitDeviceEvent("memoryWarning", null);
        }
      }
    };

    const applicationContext = this.ctx.uiAbilityContext.getApplicationContext();
    const envCallbackID = applicationContext.on('environment', envCallback);
    this.cleanUpCallbacks.push(() => {
      applicationContext.off("environment", envCallbackID);
    });
  }

  private getAppState() {
    const isActive = this.ctx.getUIAbilityState();

    return isActive === "FOREGROUND" ? 'active' : 'background';
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

