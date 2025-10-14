/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { lang } from '@kit.ArkTS';
import { UITurboModuleContext } from './RNOHContext';
import { SupportedTaskParams, Task, TaskRunnable, TaskRunner } from './TaskRunner';

type ISendable = lang.ISendable

/**
 * @api
 * @deprecated Use `SupportedTaskParams` instead (latestRNOHVersion: 0.72.40)
 */
export type SupportedUITaskParams = SupportedTaskParams

/**
 * @internal
 */
export class UITaskRunner extends TaskRunner<UITurboModuleContext, UITask> {
  private id: string
  private nextTaskId = 0;

  constructor(id: string, onScheduleUITask: (uiTask: UITask) => void) {
    super(onScheduleUITask)
    this.id = id
  }

  createTask<TParams extends SupportedTaskParams,
  TResult extends void | lang.ISendable,
  TRunnable extends UITaskRunnable<TParams, TResult>>(
    runnable: TRunnable,
    params: TParams): UITask<TParams, TRunnable> {
    return new UITask(`${this.id}::${this.nextTaskId++}`, runnable, params);
  }
}

/**
 * @api
 */
export interface UITaskRunnable<TParams extends SupportedTaskParams = undefined, TResult extends ISendable | void = ISendable | void> extends TaskRunnable<UITurboModuleContext, TParams, TResult> {
  run(uiTurboModuleContext: UITurboModuleContext, params: TParams): TResult;
}

/**
 * @internal
 */
@Sendable
export class UITask<TParams extends SupportedTaskParams = SupportedTaskParams,
TUITaskRunnable extends UITaskRunnable<TParams> = UITaskRunnable<TParams>>
  extends Task<UITurboModuleContext, TUITaskRunnable, TParams> {
  constructor(id: string, runnable: TUITaskRunnable, params: TParams) {
    super(id, runnable, params)
  }
}