/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export {
  UITurboModuleContext,
  TurboModuleContext,
  WorkerTurboModuleContext,
} from './RNOHContext';

import {
  AnyThreadTurboModuleContext,
  UITurboModuleContext,
  WorkerTurboModuleContext,
} from './RNOHContext';

/**
 * @thread: MAIN
 * @api
 */
export class UITurboModule {
  public static readonly NAME: string;

  public constructor(protected ctx: UITurboModuleContext) {}

  public __onDestroy__() {}
}

/**
 * @api
 * @deprecated Use UITurboModule or AnyThreadTurboModule instead (latestRNOHVersion: 0.72.30)
 */
export class TurboModule extends UITurboModule {}

/**
 * @thread: WORKER
 * @deprecated (latestRNOHVersion: 0.72.38)
 * Use AnyThreadTurboModule
 */
export class WorkerTurboModule {
  public static readonly NAME: string;

  public constructor(protected ctx: WorkerTurboModuleContext) {}

  public __onDestroy__() {}
}

/**
 * @api
 * @thread: MAIN | WORKER
 * A TurboModule which can run on either UI or WorkerThread eTS context.
 * Application developers can optionally enable WorkerThread to execute these TurboModules
 * on a separate thread and offload some work off the UI thread.
 * Extend this class unless you need to use UI-thread-only APIs.
 */
export class AnyThreadTurboModule {
  public static readonly NAME: string;
  public constructor(protected ctx: AnyThreadTurboModuleContext) {}

  public __onDestroy__() {}
}
