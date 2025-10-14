/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import UIAbility from '@ohos.app.ability.UIAbility';
import type { RNOHLogger } from '@rnoh/react-native-openharmony/src/main/ets/RNOH/RNOHLogger';
import { StandardRNOHLogger } from '@rnoh/react-native-openharmony/src/main/ets/RNOH/RNOHLogger';
import Want from '@ohos.app.ability.Want';
import { RNInstancesCoordinator } from "@rnoh/react-native-openharmony/src/main/ets/RNOH/RNInstancesCoordinator";
import { HttpClient, DefaultHttpClient } from "@rnoh/react-native-openharmony/src/main/ets/HttpClient";
import { EnvironmentCallback } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import common from '@ohos.app.ability.common';
import window from '@ohos.window';

export class UPRNSDKImpl {
  private context: common.UIAbilityContext | null = null;
  protected isDebugModeEnabled_: boolean = true;
  private rnInstancesCoordinator!: RNInstancesCoordinator;
  private logger!: RNOHLogger;

  constructor(context: common.UIAbilityContext, want: Want) {
    this.context = context;
    let envCallback: EnvironmentCallback = {
      onConfigurationUpdated(config) {
        let fontSizeScale = config.fontSizeScale;
        AppStorage.setOrCreate('fontSizeScale', fontSizeScale);
      },
      onMemoryLevel(level) {
        console.log('onMemoryLevel level: ${level}');
      }
    };
    try {
      let applicationContext = this.context.getApplicationContext();
      let callbackId = applicationContext.on('environment', envCallback);
      console.log(`callbackId: ${callbackId}`);
    } catch (paramError) {
      console.error(`error: ${(paramError as BusinessError).code}, ${(paramError as BusinessError).message}`);
    }
    this.logger = this.createLogger().clone("RNAbility");
    this.rnInstancesCoordinator = RNInstancesCoordinator.create({
      fontSizeScale: this.context.config.fontSizeScale,
      logger: this.createLogger(),
      uiAbilityContext: this.context,
      defaultBackPressHandler: () => {
        this.defaultBackPressHandler();
      },
    }, {
      launchURI: want.uri, onGetPackagerClientConfig: (buildMode) => buildMode === "DEBUG" ? {
        host: "localhost",
        port: 8081
      } : undefined,
      defaultHttpClient: this.onCreateDefaultHttpClient()
    })
    AppStorage.setOrCreate('RNOHCoreContext', this.rnInstancesCoordinator.getRNOHCoreContext())
  }

  public async onAppSetup(windowStage: window.WindowStage) {
    const mainWindow = await windowStage.getMainWindow();
    const onWindowSizeChange = (windowSize: window.Size) => {
      this.rnInstancesCoordinator?.onWindowSizeChange(windowSize);
    }
    mainWindow.on('windowSizeChange', onWindowSizeChange);
  }

  protected createLogger(): RNOHLogger {
    return new StandardRNOHLogger();
  }

  onBackPress() {
    const stopTracing = this.logger.clone("onBackPress").startTracing();
    stopTracing();
    return true;
  }

  /**
   * Invoked when the React application doesn't handle the device back press.
   */
  protected defaultBackPressHandler() {
    this.context?.terminateSelf();
  }

  /**
   * @returns HttpClient shared by RNInstances
   */
  protected onCreateDefaultHttpClient(): undefined | HttpClient {
    return new DefaultHttpClient({ logger: this.logger });
  }
}
