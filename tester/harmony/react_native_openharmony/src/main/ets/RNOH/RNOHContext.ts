/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNInstanceImpl } from './RNInstance';
import { RNInstanceError, RNOHError, RNOHErrorEventEmitter } from './RNOHError';
import type common from '@ohos.app.ability.common';
import { UIContext } from '@kit.ArkUI';
import type { DescriptorRegistry } from './DescriptorRegistry';
import type { RNComponentCommandReceiver } from './RNComponentCommandHub';
import type { RNInstance } from './RNInstance';
import type { ComponentManagerRegistry } from './ComponentManagerRegistry';
import type { HttpClient, CAPathProvider } from '../HttpClient/HttpClient';
import type { RNOHLogger } from './RNOHLogger';
import type { WorkerRNInstance } from './WorkerRNInstance';
import type { DevToolsController } from './DevToolsController';
import type { DisplayMetrics, DevMenu } from './types';
import type { RNInstanceOptions } from './RNInstance';
import type { SafeAreaInsetsProvider } from './SafeAreaInsetsProvider';

export type UIAbilityState = 'FOREGROUND' | 'BACKGROUND';

type RNOHCoreContextDependencies = {
  reactNativeVersion: string;
  rnInstanceRegistry: {
    createInstance(options: RNInstanceOptions): Promise<RNInstance>;
    deleteInstance(id: number): Promise<boolean>;
    forEach(cb: (rnInstance: RNInstanceImpl) => void);
  };
  displayMetricsProvider: () => DisplayMetrics;
  uiAbilityStateProvider: () => UIAbilityState;
  rnohErrorEventEmitter: RNOHErrorEventEmitter;
  logger: RNOHLogger;
  uiAbilityContext: common.UIAbilityContext;
  isDebugModeEnabled: boolean;
  defaultBackPressHandler: () => void;
  devToolsController: DevToolsController;
  devMenu: DevMenu;
  safeAreaInsetsProvider: SafeAreaInsetsProvider;
  launchUri?: string;
  erasedWorkerTaskRunner: unknown;
};

/**
 * @thread: MAIN
 * Provides dependencies and utilities shareable across RNInstances. Also includes methods for creating and destroying
 * RNInstances. For utilities or dependencies specific to an RNInstance, refer to RNOHContext.
 */
export class RNOHCoreContext {
  /**
   * @internal
   */
  constructor(public _rnohCoreContextDeps: RNOHCoreContextDependencies) {
  }

  get reactNativeVersion() {
    return this._rnohCoreContextDeps.reactNativeVersion;
  }

  reportRNOHError(rnohError: RNOHError) {
    this._rnohCoreContextDeps.rnohErrorEventEmitter.emit('NEW_ERROR', rnohError)
  }

  subscribeToRNOHErrors(listener: (err: RNOHError) => void): () => void {
    return this._rnohCoreContextDeps.rnohErrorEventEmitter.subscribe('NEW_ERROR', listener);
  }


  async createAndRegisterRNInstance(
    options: RNInstanceOptions,
  ): Promise<RNInstance> {
    const stopTracing = this._rnohCoreContextDeps.logger
      .clone('createAndRegisterRNInstance')
      .startTracing();
    const result =
      await this._rnohCoreContextDeps.rnInstanceRegistry.createInstance(
        options,
      );
    stopTracing();
    return result;
  }

  async destroyAndUnregisterRNInstance(rnInstance: RNInstance): Promise<void> {
    const stopTracing = this._rnohCoreContextDeps.logger
      .clone('destroyAndUnregisterRNInstance')
      .startTracing();
    if (rnInstance instanceof RNInstanceImpl) {
      rnInstance.onDestroy();
    }
    await this._rnohCoreContextDeps.rnInstanceRegistry.deleteInstance(
      rnInstance.getId(),
    );
    stopTracing();
  }

  getDisplayMetrics(): DisplayMetrics {
    return this._rnohCoreContextDeps.displayMetricsProvider();
  }

  getUIAbilityState(): UIAbilityState {
    return this._rnohCoreContextDeps.uiAbilityStateProvider();
  }

  dispatchBackPress(): void {
    this._rnohCoreContextDeps.rnInstanceRegistry.forEach(rnInstance =>
      rnInstance.onBackPress(),
    );
  }

  /**
   * @deprecated - This function shouldn't be in RNOHCoreContext because readiness is relative to a RNInstance and this context is shared across instances.
   * @depreciationDate 2024-04-08
   */
  markReadiness(): void {
  }

  cancelTouches(): void {
    this._rnohCoreContextDeps.rnInstanceRegistry.forEach(rnInstance => {
      rnInstance.cancelTouches();
    });
  }

  get logger(): RNOHLogger {
    return this._rnohCoreContextDeps.logger;
  }

  get uiAbilityContext(): common.UIAbilityContext {
    return this._rnohCoreContextDeps.uiAbilityContext;
  }

  get isDebugModeEnabled(): boolean {
    return this._rnohCoreContextDeps.isDebugModeEnabled;
  }

  get launchUri(): string | undefined {
    return this._rnohCoreContextDeps.launchUri;
  }

  get devToolsController(): DevToolsController {
    return this._rnohCoreContextDeps.devToolsController!;
  }

  get devMenu(): DevMenu {
    return this._rnohCoreContextDeps.devMenu!;
  }

  get defaultBackPressHandler(): () => void {
    return this._rnohCoreContextDeps.defaultBackPressHandler;
  }

  get safeAreaInsetsProvider() {
    return this._rnohCoreContextDeps.safeAreaInsetsProvider;
  }

  protected get erasedWorkerTaskRunner() {
    return this._rnohCoreContextDeps.erasedWorkerTaskRunner;
  }
}

/**
 * @internal
 */
export class InternalRNOHCoreContext extends RNOHCoreContext {
  onDestroy() {
  }
}

type RNOHContextDependencies = {
  rnInstance: RNInstance;
  rnohCoreContextDependencies: RNOHCoreContextDependencies;
};

/**
 * @thread: MAIN
 * Provides dependencies and utility functions in context of RNInstance.
 */
export class RNOHContext extends RNOHCoreContext {
  /**
   * @internal
   */
  static fromCoreContext(
    rnohCoreContext: RNOHCoreContext,
    rnInstance: RNInstance,
  ): RNOHContext {
    return new RNOHContext({
      rnohCoreContextDependencies: rnohCoreContext._rnohCoreContextDeps,
      rnInstance: rnInstance,
    });
  }

  protected constructor(public _rnohContextDeps: RNOHContextDependencies) {
    super(_rnohContextDeps.rnohCoreContextDependencies);
  }

  protected get rnInstanceImpl() {
    return this._rnohContextDeps.rnInstance as RNInstanceImpl;
  }

  reportRNOHError(rnohError: RNOHError) {
    this.rnInstanceImpl.reportRNOHError(rnohError);
  }

  subscribeToRNInstanceErrors(listener: (err: RNInstanceError) => void): () => void {
    return this.rnInstanceImpl.subscribeToRNOHErrors(listener);
  }

  /**
   * Check DescriptorRegistry documentation for more information.
   */
  get descriptorRegistry(): DescriptorRegistry {
    return this.rnInstanceImpl.descriptorRegistry;
  }

  /**
   * Check RNComponentCommandReceiver documentation for more information.
   */
  get componentCommandReceiver(): RNComponentCommandReceiver {
    return this.rnInstanceImpl.componentCommandHub;
  }

  /**
   * Check ComponentManagerRegistry documentation for more information.
   */
  get componentManagerRegistry(): ComponentManagerRegistry {
    return this.rnInstanceImpl.componentManagerRegistry;
  }

  /**
   * Check RNInstance documentation for more information.
   */
  get rnInstance(): RNInstance {
    return this.rnInstanceImpl;
  }

  /**
   * Check RNInstance::httpClient documentation
   * @returns
   */
  get httpClient(): HttpClient {
    return this.rnInstance.httpClient;
  }

  get caPathProvider(): CAPathProvider {
    return this.rnInstance.caPathProvider;
  }
  /**
   * Invoked by React Native when the React application doesn't want to handle the device back press. This method may be relocated in the future.
   */
  invokeDefaultBackPressHandler() {
    if (this.rnInstanceImpl.backPressHandler) {
      this.rnInstanceImpl.backPressHandler();
    } else {
      this._rnohCoreContextDeps.defaultBackPressHandler();
    }
  }
}

export interface AnyThreadRNInstance extends Partial<RNInstance> {
  emitDeviceEvent(eventName: string, payload: any): void;

  callRNFunction(
    moduleName: string,
    functionName: string,
    args: unknown[],
  ): void;

  postMessageToCpp(name: string, payload: any): void;

  getArchitecture(): 'ARK_TS' | 'C_API';

  getTurboModule<T>(name: string): T;

  getAssetsDest(): string;

  getInitialBundleUrl(): string | undefined

  getId(): number
}

/**
 * @api
 */
export interface AnyThreadTurboModuleContext {
  logger: RNOHLogger;
  uiAbilityContext: common.UIAbilityContext;
  rnInstance: AnyThreadRNInstance;
  httpClient: HttpClient;
  caPathProvider: CAPathProvider;
  launchUri?: string;
}

/**
 * @api
 */
export class UITurboModuleContext
  extends RNOHContext
  implements AnyThreadTurboModuleContext {
  constructor(rnohContext: RNOHContext) {
    super(rnohContext._rnohContextDeps);
  }

  getUIContext(): UIContext | null {
    return this._rnohContextDeps.rnInstance.getUIContext();
  }
}

/**
 * @api
 * @deprecated: Use UITurboModuleContext or WorkerTurboModuleContext instead (latestRNOHVersion: 0.72.30)
 */
export class TurboModuleContext extends UITurboModuleContext {
}

/**
 * @internal
 */
export type WorkerTurboModuleContextDependencies = {
  logger: RNOHLogger;
  uiAbilityContext: common.UIAbilityContext;
  rnInstance: WorkerRNInstance;
  httpClient: HttpClient;
  caPathProvider: CAPathProvider;
  launchUri?: string;
};

/**
 * @api
 */
export class WorkerTurboModuleContext implements AnyThreadTurboModuleContext {
  /**
   * @internal
   */
  constructor(
    public _workerTurboModuleContextDeps: WorkerTurboModuleContextDependencies,
  ) {
  }

  get logger() {
    return this._workerTurboModuleContextDeps.logger;
  }

  get uiAbilityContext() {
    return this._workerTurboModuleContextDeps.uiAbilityContext;
  }

  get rnInstance() {
    return this._workerTurboModuleContextDeps.rnInstance;
  }

  get httpClient() {
    return this._workerTurboModuleContextDeps.httpClient;
  }

  get caPathProvider() {
    return this._workerTurboModuleContextDeps.caPathProvider;
  }

  get launchUri(): string | undefined {
    return this._workerTurboModuleContextDeps.launchUri;
  }
}
