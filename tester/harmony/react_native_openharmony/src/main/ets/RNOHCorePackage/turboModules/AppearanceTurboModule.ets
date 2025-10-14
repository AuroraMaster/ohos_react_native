/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import ConfigurationConstant from '@ohos.app.ability.ConfigurationConstant';
import type EnvironmentCallback from '@ohos.app.ability.EnvironmentCallback';
import { EtsWorkerTurboModuleContext } from '../../RNOH';
import { AnyThreadTurboModuleContext } from '../../RNOH/RNOHContext';
import { AnyThreadTurboModule, RNOHLogger, UITurboModuleContext } from "../../RNOH/ts"
import { UITaskRunnable } from '../../RNOH/UITaskRunner';

@Sendable
class ChangeColorSchemeRunnableParams {
  public colorMode: number

  constructor(colorMode: ConfigurationConstant.ColorMode) {
    this.colorMode = colorMode
  }
}

@Sendable
class ChangeColorSchemeRunnable implements UITaskRunnable<ChangeColorSchemeRunnableParams> {
  run(uiTurboModuleContext: UITurboModuleContext, params: ChangeColorSchemeRunnableParams): void {
    uiTurboModuleContext.uiAbilityContext.getApplicationContext().setColorMode(params.colorMode);
  }
}

type JSColorScheme = 'light' | 'dark' | null;

export class AppearanceTurboModule extends AnyThreadTurboModule {
  public static readonly NAME = 'Appearance';
  private lastColorScheme: JSColorScheme = null;
  /**
   * This is a hack. Currently applicationContext.setColorMode(colorMode) doesn't seem to work for COLOR_MODE_NOT_SET.
   * We store the initialColorMode so when the JS side wants to set colorScheme to null, we can set the ColorMode to the
   * ColorMode which was used on app startup (which was set in system settings).  (colorScheme set to null means that
   * the colorMode which is set in the system settings should be used).
   * Refer to RN docs: https://reactnative.dev/docs/appearance
   */
  private initialColorMode: ConfigurationConstant.ColorMode = ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET;
  private logger: RNOHLogger

  constructor(ctx: AnyThreadTurboModuleContext) {
    super(ctx);
    this.logger = ctx.logger.clone("AppearanceTurboModule")
    this.logger.info("constructor")
    this.setup();
  }

  private colorModeToJSColorScheme(colorMode: ConfigurationConstant.ColorMode): JSColorScheme {
    if (colorMode === ConfigurationConstant.ColorMode.COLOR_MODE_LIGHT) {
      return 'light'
    } else if (colorMode === ConfigurationConstant.ColorMode.COLOR_MODE_DARK) {
      return 'dark';
    } else {
      return null;
    }
  }

  getColorScheme(): JSColorScheme {
    return this.lastColorScheme;
  }

  setup(): void {
    this.initialColorMode =
      this.ctx.uiAbilityContext.config.colorMode ?? ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET
    this.lastColorScheme = this.colorModeToJSColorScheme(this.initialColorMode);

    let envCallback: EnvironmentCallback = {
      onConfigurationUpdated: (config) => {
        const colorMode = config.colorMode ?? ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET;
        this.lastColorScheme = this.colorModeToJSColorScheme(colorMode);

        this.ctx.rnInstance.emitDeviceEvent("appearanceChanged", {
          colorScheme: this.lastColorScheme
        });
      },

      onMemoryLevel: () => {
        //we need this empty callback, otherwise it won't compile
      }
    };

    const applicationContext = this.ctx.uiAbilityContext.getApplicationContext();
    applicationContext.on('environment', envCallback);
  }

  setColorScheme(colorScheme: string): void {
    let colorMode: ConfigurationConstant.ColorMode;
    if (colorScheme === 'light') {
      colorMode = ConfigurationConstant.ColorMode.COLOR_MODE_LIGHT;
    } else if (colorScheme === 'dark') {
      colorMode = ConfigurationConstant.ColorMode.COLOR_MODE_DARK;
    } else {
      //for null we want to use the colorMode set in the system settings
      colorMode = this.initialColorMode;
    }
    if (this.ctx instanceof EtsWorkerTurboModuleContext) {
      this.ctx.runOnUIThread(new ChangeColorSchemeRunnable(), new ChangeColorSchemeRunnableParams(colorMode))
    } else if (this.ctx instanceof UITurboModuleContext) {
      this.ctx.uiAbilityContext.getApplicationContext().setColorMode(colorMode)
    }
  };
}
