/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import UIAbility from '@ohos.app.ability.UIAbility';
import type { RNOHLogger } from './RNOHLogger';
import { StandardRNOHLogger } from './RNOHLogger';
import { RNOHCoreContext } from "./RNOHContext"
import window from '@ohos.window';
import Want from '@ohos.app.ability.Want';
import { RNInstancesCoordinator } from "./RNInstancesCoordinator"
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import AbilityConfiguration from '@ohos.app.ability.Configuration';
import { HttpClient, DefaultHttpClient } from "../HttpClient"
import { EnvironmentCallback } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { EventEmitter } from './EventEmitter';

/**
 Application developers are expected to extend this class. RNAbility aims to move much of the code to the RNOH side in order to improve stability.
 RNInstancesCoordinator can be used if more control is needed.
 */
export abstract class RNAbility extends UIAbility {
  // region RNAbility methods that can be safely overridden.
  protected createLogger(): RNOHLogger {
    return new StandardRNOHLogger();
  }


  /**
   * Override this method if you want to create an RNInstance, and load bundle here, rather than in an ArkUI component.
   * RNAbility waits for the completion of this method before loading the content. This approach reduces a flash that
   * can occur after a splash screen is hidden, and a RN application is ready.
   */
  protected async onPrepareRNInstance(rnohCoreContext: RNOHCoreContext) {
  }

  /**
   * Override this method if you want to specify screen orientation, window dimensions etc.
   */
  protected async onWindowSetup(window: window.Window) {
    // NOTE: disabling full screen breaks the SafeAreaView component
    await window.setWindowLayoutFullScreen(true)
  }

  /**
   * Override this method if, for example, you want to customize loading behavior.
   */
  protected async onWindowStageSetup(windowStage: window.WindowStage) {
    windowStage.loadContent(this.getPagePath(), (err, data) => {
      const logger = this.logger.clone("onLoadContent")
      if (err.code) {
        logger.error("Failed to load the content", err.code)
        return;
      }
      logger.info("Succeeded in loading the content", JSON.stringify(data))
      this.rnInstancesCoordinator?.onUpdateDecorHeight();
    });
  }

  protected abstract getPagePath(): string

  /**
   * Override to specify the worker script URL and enable the RNOHWorker thread and WorkerTurboModules.
   * @example return "entry/ets/workers/RNOHWorker.ets"
   *
   * RNOHWorker.ets calls `setupRNOHWorker` which can be imported from RNOH in the following way:
   * import { setupRNOHWorker } from "@rnoh/react-native-openharmony/src/main/ets/setupRNOHWorker"
   *
   * You may need to import setupRNOHWorker directly from its file, because ETS seem to struggle when
   * importing this function from the package level.
   */
  protected getRNOHWorkerScriptUrl(): string | undefined {
    return undefined
  }

  /**
   * Invoked when the React application doesn't handle the device back press.
   */
  protected defaultBackPressHandler() {
    this.context.terminateSelf()
  }

  // endregion

  private mainWindow: window.Window | undefined = undefined
  private windowStage: window.WindowStage | undefined = undefined
  private rnInstancesCoordinator!: RNInstancesCoordinator
  private logger!: RNOHLogger

  private onWindowSizeChange = (windowSize: window.Size) => {
    this.rnInstancesCoordinator?.onWindowSizeChange(windowSize)
  }
  private onWindowRectChange = (data: window.RectChangeOptions) => {
    this.rnInstancesCoordinator?.onWindowRectChange(data)
  }
  private onWindowStageChange = (windowStageEvent: window.WindowStageEventType) => {
    this.rnInstancesCoordinator?.onWindowStageChange(windowStageEvent)
  }

  private subscribeWindowListeners() {
    this.mainWindow?.on('windowSizeChange', this.onWindowSizeChange)
    this.mainWindow?.on('windowRectChange', this.onWindowRectChange)
    this.windowStage?.on('windowStageEvent', this.onWindowStageChange)
  }

  private unsubscribeWindowListeners() {
    try {
      this.mainWindow?.off('windowSizeChange', this.onWindowSizeChange)
      this.mainWindow?.off('windowRectChange', this.onWindowRectChange)
      this.windowStage?.off('windowStageEvent', this.onWindowStageChange)
    } catch {
      this.logger?.error("Error when trying to unsubscribe from window changes")
    }
  }

  onCreate(want: Want, launchParam?: AbilityConstant.LaunchParam) {
    this.logger = this.createLogger().clone("RNAbility")
    this.rnInstancesCoordinator = RNInstancesCoordinator.create({
      logger: this.createLogger(),
      uiAbilityContext: this.context,
      defaultBackPressHandler: () => {
        this.defaultBackPressHandler()
      },
      rnohWorkerScriptUrl: this.getRNOHWorkerScriptUrl(),
    }, {
      disableCleaningRNInstances: this.onShouldDisableCleaningRNInstances(),
      launchURI: want.uri,
      onGetPackagerClientConfig: (buildMode) => buildMode === "DEBUG" ? {
        host: "localhost",
        port: 8081
      } : undefined,
      defaultHttpClient: this.onCreateDefaultHttpClient()
    })
    AppStorage.setOrCreate('RNOHCoreContext', this.rnInstancesCoordinator.getRNOHCoreContext())
  }

  /**
   * @returns HttpClient shared by RNInstances
   */
  protected onCreateDefaultHttpClient(): undefined | HttpClient {
    return new DefaultHttpClient({ logger: this.logger });
  }

  onDestroy() {
    // this method seems to be never called
    this.rnInstancesCoordinator?.onDestroy()
    this.unsubscribeWindowListeners()
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    const logger = this.logger.clone("onWindowStageCreate")
    const stopTracing = logger.startTracing()
    this.onAppSetup(windowStage).finally(stopTracing)
  }

  onWindowStageDestroy() {
    this.unsubscribeWindowListeners()
  }

  private async onAppSetup(windowStage: window.WindowStage) {
    const logger = this.logger.clone("onWindowSetup");
    const stopTracing = logger.startTracing()
    try {
      this.unsubscribeWindowListeners()
      this.windowStage = windowStage
      await this.onPrepareRNInstance(AppStorage.get<RNOHCoreContext>('RNOHCoreContext')!)
      const mainWindow = await windowStage.getMainWindow();
      this.mainWindow = mainWindow
      const windowID = mainWindow.getWindowProperties().id;
      this.rnInstancesCoordinator?.postMessageToCpp("WINDOW_ID", windowID);
      this.rnInstancesCoordinator?.postMessageToCpp("BUNDLE_CODE_DIR", this.context.bundleCodeDir);
      this.rnInstancesCoordinator?.setMainWindow(mainWindow);
      
      this.subscribeWindowListeners()

      await this.onWindowSetup(mainWindow)
      await this.onWindowStageSetup(windowStage)
      let windowSize = mainWindow.getWindowProperties().windowRect
      windowSize.top = 0
      windowSize.left = 0
      this.rnInstancesCoordinator?.onWindowSizeChange(windowSize)
    } catch (err) {
      logger.error("Failed to setup window", JSON.stringify(err))
    } finally {
      stopTracing()
    }
  }

  onMemoryLevel(level: AbilityConstant.MemoryLevel) {
    this.rnInstancesCoordinator?.onMemoryLevel(level)
  }

  onConfigurationUpdate(config: AbilityConfiguration.Configuration) {
    this.rnInstancesCoordinator?.onConfigurationUpdate(config)
  }

  onForeground() {
    this.rnInstancesCoordinator?.onForeground();
  }

  onBackground() {
    this.rnInstancesCoordinator?.onBackground();
  }

  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    this.rnInstancesCoordinator?.onNewWant(want, launchParam);
  }

  /**
   * Return `true`, if your app bundle provides multiple RNAbilities, for each RNAbility.
   */
  onShouldDisableCleaningRNInstances(): boolean {
    return false;
  }

  protected setParallelizationEnabled(enabled: boolean): void {
    this.rnInstancesCoordinator?.setParallelizationEnabled(enabled);
  }
}
