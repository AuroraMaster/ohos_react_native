/**
 * MIT License
 * 
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * 
 * This code is licensed under the MIT License.
 * Please see the LICENSE-MIT file for more information.
 */
/**
* This file location relative to the package root shouldn't change to avoid introducing breaking changes.
*/
import { lang } from '@kit.ArkTS';
import { RNOHError } from "./RNOHError"

type ISendable = lang.ISendable

/**
 * @api
 */
export type SupportedTaskParams = ISendable | undefined

/**
 * @api
 */
export interface TaskRunnable<TContext, TParams extends SupportedTaskParams = undefined, TResult extends ISendable | void = ISendable | void> extends ISendable {
  run(context: TContext, params: TParams): TResult;
}

/**
 * @internal
 */
@Sendable
export class Task<TContext,
TTaskRunnable extends TaskRunnable<TContext, TParams> = TaskRunnable<TContext, SupportedTaskParams>,
TParams extends SupportedTaskParams = SupportedTaskParams> {
  id: string
  runnable: TTaskRunnable;
  params: TParams;

  constructor(id: string, runnable: TTaskRunnable, params: TParams) {
    this.id = id
    this.runnable = runnable
    this.params = params;
  }
}


/**
 * @internal
 */
export abstract class TaskRunner<TContext, TTask extends Task<TContext>> {
  private taskResultResolverByTaskId =
    new Map<string, (result: ISendable | void) => void>()
  private onScheduleTask: (task: TTask) => void;

  constructor(onScheduleTask: (task: TTask) => void) {
    this.onScheduleTask = onScheduleTask
  }

  run<TParams extends SupportedTaskParams, TResult extends ISendable | void, TRunnable extends TaskRunnable<TContext, TParams>>(task: TTask): Promise<TResult> {
    this.onScheduleTask(task)
    const promisedResultAndPromiseResolver = createPromiseAndResolver<ISendable | void>();
    this.taskResultResolverByTaskId.set(task.id, promisedResultAndPromiseResolver[1])
    return promisedResultAndPromiseResolver[0] as Promise<TResult>;
  }

  resolve(taskId: string, result: ISendable | void) {
    const resolveResult = this.taskResultResolverByTaskId.get(taskId)
    if (!resolveResult) {
      throw new RNOHError({
        whatHappened: "Tried to resolve unknown task result " +
        JSON.stringify({ taskId, registeredTaskIds: Array.from(this.taskResultResolverByTaskId.keys()) }),
        howCanItBeFixed: []
      })
    }
    resolveResult(result);
    this.taskResultResolverByTaskId.delete(taskId)
  }
}

function createPromiseAndResolver<T = Object | undefined>(): [Promise<T>, ((result: T) => void)] {
  let resolve: ((result: T) => void) = (_value: T) => {
  }
  const promise = new Promise<T>((_resolve) => resolve = _resolve)
  return [promise, resolve]
}