/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import font from '@ohos.font';
import type { RNInstance, RNInstanceOptions } from './RNInstance';
import { RNInstanceImpl } from './RNInstance';
import type { NapiBridge } from './NapiBridge';
import type { UITurboModuleContext } from './TurboModule'
import type { RNOHLogger } from './RNOHLogger';
import type { DevToolsController } from './DevToolsController';
import { HttpClientProvider } from './HttpClientProvider';
import { HttpClient } from '../HttpClient/ts';
import { DisplayMetricsManager } from './DisplayMetricsManager'
import resourceManager from '@ohos.resourceManager';
import { WorkerThread } from './WorkerThread'
import common from '@ohos.app.ability.common'
import { RNOHCorePackage } from '../RNOHCorePackage/Package'
import { RNOHErrorEventEmitter } from './RNOHError';

const DEFAULT_ASSETS_DEST: string = "assets/"; // assets destination path "assets/subpath/"

interface CreateWorkerRNInstanceAckPayload {
  rnInstanceId: number
}

interface DestroyWorkerRNInstanceAckPayload {
  rnInstanceId: number
}

export class RNInstanceRegistry {
  private instanceMap: Map<number, RNInstanceImpl> = new Map();
  private unregisterMessageListeners: (() => void)[] = []
  private uiAbilityContext: common.UIAbilityContext
  private envId: number
  private logger: RNOHLogger
  private globalRNOHErrorEventEmitter: RNOHErrorEventEmitter;
  private napiBridge: NapiBridge
  private devToolsController: DevToolsController
  private createUITurboModuleContext: (rnInstance: RNInstanceImpl) => UITurboModuleContext
  private httpClientProvider: HttpClientProvider
  private defaultHttpClient: HttpClient | undefined
  private resourceManager: resourceManager.ResourceManager
  private displayMetricsManager: DisplayMetricsManager
  private workerThreadPromise: Promise<WorkerThread> | undefined
  private launchUri?: string
  private cacheDir?: string
  private onInstanceCreated?: (rnInstance: RNInstanceImpl) => void

  constructor(
    uiAbilityContext: common.UIAbilityContext,
    envId: number,
    logger: RNOHLogger,
    rnohErrorEventEmitter: RNOHErrorEventEmitter,
    napiBridge: NapiBridge,
    devToolsController: DevToolsController,
    createUITurboModuleContext: (rnInstance: RNInstanceImpl) => UITurboModuleContext,
    httpClientProvider: HttpClientProvider,
    defaultHttpClient: HttpClient | undefined, // TODO: remove "undefined" when HttpClientProvider is removed
    resourceManager: resourceManager.ResourceManager,
    displayMetricsManager: DisplayMetricsManager,
    workerThreadPromise: Promise<WorkerThread> | undefined,
    launchUri: string | undefined,
    cacheDir: string | undefined,
    onInstanceCreated?: (rnInstance: RNInstanceImpl) => void,
  ) {
    this.uiAbilityContext = uiAbilityContext
    this.envId = envId
    this.logger = logger
    this.globalRNOHErrorEventEmitter = rnohErrorEventEmitter
    this.napiBridge = napiBridge
    this.devToolsController = devToolsController
    this.createUITurboModuleContext = createUITurboModuleContext
    this.httpClientProvider = httpClientProvider
    this.defaultHttpClient = defaultHttpClient
    this.resourceManager = resourceManager
    this.displayMetricsManager = displayMetricsManager
    this.workerThreadPromise = workerThreadPromise
    this.launchUri = launchUri
    this.cacheDir = cacheDir
    this.onInstanceCreated = onInstanceCreated
  }

  public async createInstance(options: RNInstanceOptions): Promise<RNInstance> {
    const id = this.napiBridge.getNextRNInstanceId();
    if (options.enableBackgroundExecutor) {
      this.logger.warn("'enableBackgroundExecutor' feature flag is deprecated");
    }
    if (options.enableCAPIArchitecture && !options.fontResourceByFontFamily) {
      this.logger.warn("No custom fonts registered");
    }
    const workerThread = await this.workerThreadPromise
    if (workerThread !== undefined) {
      await this.createRNInstanceEnvOnWorker(workerThread, id, options)
    }
    const fontPathByFontFamily: Record<string, string> = {}
    for (const entry of Object.entries(options.fontResourceByFontFamily ?? {})) {
      const fontFamily = entry[0]
      const fontResource = entry[1]
      if (typeof fontResource == 'string') {
        font.registerFont({ familyName: fontFamily, familySrc: `file://${fontResource}` })
        fontPathByFontFamily[fontFamily] = fontResource
      } else {
        fontPathByFontFamily[fontFamily] = fontResource.params![0]
        font.registerFont({ familyName: fontFamily, familySrc: fontResource })
      }
    }
    const rnInstance = new RNInstanceImpl(
      this.envId,
      id,
      options?.name,
      this.logger,
      this.globalRNOHErrorEventEmitter,
      this.napiBridge,
      options.disableConcurrentRoot,
      this.devToolsController,
      this.createUITurboModuleContext,
      workerThread,
      options.enableDebugger ?? false,
      options.enableBackgroundExecutor ?? false,
      options.enableNDKTextMeasuring ?? false,
      options.enableImageLoader ?? false,
      options.enableCAPIArchitecture ?? false,
      options.assetsDest ?? DEFAULT_ASSETS_DEST,
      this.resourceManager,
      options.arkTsComponentNames,
      this.displayMetricsManager,
      fontPathByFontFamily,
      this.httpClientProvider,
      options?.httpClient ?? this.defaultHttpClient,
      options?.caPathProvider ?? ((url?: string) => ''),
      options.backPressHandler,
      options.jsvmInitOptions,
      options.hspModuleName,
      this.cacheDir,
    );
    rnInstance.setUIAbilityContext(this.uiAbilityContext);
    const packages = options.createRNPackages({})
    packages.unshift(new RNOHCorePackage({}));
    await rnInstance.initialize(packages);
    this.instanceMap.set(id, rnInstance);
    this.onInstanceCreated?.(rnInstance);
    return rnInstance;
  }

  private async createRNInstanceEnvOnWorker(workerThread: WorkerThread, rnInstanceId: number,
    options: RNInstanceOptions) {
    const logger = this.logger.clone(["RNInstanceRegistry", "createRNInstanceEnvOnWorker"])
    logger.info("waiting for worker's rnInstance environment")
    setTimeout(() => {
      workerThread.postMessage("RNOH_CREATE_WORKER_RN_INSTANCE", {
        rnInstanceId,
        rnInstanceName: options.name,
        uiAbilityContext: this.uiAbilityContext,
        architecture: options.enableCAPIArchitecture ? "C_API" : "ARK_TS",
        assetsDest: options.assetsDest ?? DEFAULT_ASSETS_DEST,
        launchUri: this.launchUri,
      })
    }, 0)
    await workerThread.waitForMessage("RNOH_CREATE_WORKER_RN_INSTANCE_ACK",
      (payload: CreateWorkerRNInstanceAckPayload) => payload.rnInstanceId === rnInstanceId)
  }

  public getInstance(id: number): RNInstance {
    return this.instanceMap.get(id)!;
  }

  public async deleteInstance(id: number): Promise<boolean> {
    if (this.instanceMap.has(id)) {
      this.instanceMap.delete(id);
      if (this.workerThreadPromise !== undefined) {
        const worker = await this.workerThreadPromise;
        const ack =
          worker.waitForMessage("RNOH_DESTROY_WORKER_RN_INSTANCE_ACK",
            (payload: DestroyWorkerRNInstanceAckPayload) => payload.rnInstanceId === id)
        worker.postMessage("RNOH_DESTROY_WORKER_RN_INSTANCE", { rnInstanceId: id })
        await ack;
      }
      return true;
    }
    return false;
  }

  public forEach(cb: (rnInstance: RNInstanceImpl) => void) {
    this.instanceMap.forEach(cb);
  }

  onDestroy() {
    this.unregisterMessageListeners.forEach(unregister => unregister())
  }
}
