/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type {NapiBridge} from './NapiBridge';
import {RNOHError} from './RNOHError';
import { AnyThreadTurboModule, WorkerTurboModule, WorkerTurboModuleContext} from './TurboModule';
import type {TurboModuleProvider} from './TurboModuleProvider';
import type {AnyThreadRNInstance} from './RNOHContext'

/**
 * @api
 * @thread: WORKER
 */
export class WorkerRNInstance implements AnyThreadRNInstance {
  protected canCallRNFunction = false;
  protected rnFunctionCallsQueue: {
    moduleName: string;
    functionName: string;
    args: unknown[];
  }[] = [];
  private initialBundleUrl: string | undefined = undefined;

  constructor(
    protected id: number,
    protected napiBridge: NapiBridge,
    protected architecture: 'ARK_TS' | 'C_API',
    protected assetsDest: string,
    protected getTurboModuleProvider: () =>
      | TurboModuleProvider<WorkerTurboModule | AnyThreadTurboModule>
      | undefined,
  ) {}

  emitDeviceEvent(eventName: string, payload: any): void {
    this.callRNFunction('RCTDeviceEventEmitter', 'emit', [eventName, payload]);
  }

  callRNFunction(
    moduleName: string,
    functionName: string,
    args: unknown[],
  ): void {
    if (!this.canCallRNFunction) {
      this.rnFunctionCallsQueue.push({moduleName, functionName, args});
      return;
    }
    this.napiBridge.callRNFunction(this.id, moduleName, functionName, args);
  }

  postMessageToCpp(name: string, payload: any): void {
    this.napiBridge.postMessageToCpp(name, {rnInstanceId: this.id, payload});
  }

  getArchitecture() {
    return this.architecture;
  }

  getTurboModule<T>(name: string): T {
    return this.getWorkerTurboModule(name) as T;
  }

  getWorkerTurboModule<T extends WorkerTurboModule>(name: string): T {
    const tmProvider = this.getTurboModuleProvider();
    if (!tmProvider) {
      throw new RNOHError({
        whatHappened:
          'TurboModuleProvider is undefined. This should never happen.',
        howCanItBeFixed: [],
      });
    }
    return tmProvider.getModule<T>(name);
  }

  getAssetsDest(): string {
    return this.assetsDest;
  }

  getInitialBundleUrl(): string | undefined {
    return this.initialBundleUrl
  }

  setBunleUrl(bundleURL: string) {
    this.initialBundleUrl = this.initialBundleUrl ?? bundleURL
  }
}

/**
 * @internal
 */
export class WorkerRNInstanceInternal extends WorkerRNInstance {
  flushRNFunctionCalls() {
    this.rnFunctionCallsQueue.forEach(({moduleName, functionName, args}) => {
      this.napiBridge.callRNFunction(this.id, moduleName, functionName, args);
    });
    this.rnFunctionCallsQueue = [];
    this.canCallRNFunction = true;
  }
}

