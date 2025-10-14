/**
 * MIT License
 * 
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * 
 * This code is licensed under the MIT License.
 * Please see the LICENSE-MIT file for more information.
 */
import lang from '@arkts.lang';
import { SupportedTaskParams, Task, TaskRunnable, TaskRunner } from './TaskRunner'
import { AnyThreadRNInstance } from './RNOHContext';
import { WorkerThread } from '.';

type ISendable = lang.ISendable

interface WorkerScheduleTaskResultPayload {
  rnInstanceId: number
  taskId: string,
  result: lang.ISendable | undefined
}

/**
 * @api
 */
export interface WorkerTaskRunnable<TParams extends SupportedTaskParams = undefined, TResult extends ISendable | void = ISendable | void> extends TaskRunnable<AnyThreadRNInstance, TParams, TResult> {
  run(turboModuleContext: AnyThreadRNInstance, params: TParams): TResult;
}

/**
 * @internal
 */
@Sendable
export class WorkerTask<TParams extends SupportedTaskParams = SupportedTaskParams,
TWorkerTaskRunnable extends WorkerTaskRunnable<TParams> = WorkerTaskRunnable<TParams>>
  extends Task<AnyThreadRNInstance, TWorkerTaskRunnable, TParams> {
  public rnInstanceId: number

  constructor(rnInstanceId: number, id: string, runnable: TWorkerTaskRunnable, params: TParams) {
    super(id, runnable, params)
    this.rnInstanceId = rnInstanceId
  }
}

/**
 * @internal
 */
export class WorkerTaskRunner extends TaskRunner<AnyThreadRNInstance, WorkerTask> {
  private nextTaskId: number = 0;
  private workerThread: WorkerThread | undefined
  public onDestroy: () => void = () => {
  }

  constructor(workerThreadPromise: Promise<WorkerThread> | undefined) {
    super((task) => {
      this.scheduleOnWorker(task)
    });
    workerThreadPromise?.then(workerThread => {
      if (workerThread) {
        this.workerThread = workerThread
        this.onDestroy = workerThread.subscribeToMessages(
          (type: string, payload: WorkerScheduleTaskResultPayload) => {
            this.onWorkerMessage(type, payload)
          })
      }
    })
  }

  createTask<TParams extends SupportedTaskParams,
  TResult extends void | lang.ISendable,
  TRunnable extends WorkerTaskRunnable<TParams, TResult>>(
    rnInstanceId: number,
    runnable: TRunnable,
    params: TParams): WorkerTask<TParams, TRunnable> {
    const taskId = this.nextTaskId++;
    return new WorkerTask(rnInstanceId, taskId.toString(), runnable, params);
  }

  private scheduleOnWorker(task: WorkerTask) {
    this.workerThread?.postMessage("RNOH_WORKER_SCHEDULE_TASK", task);
  }

  private async onWorkerMessage(type: string, payload: WorkerScheduleTaskResultPayload): Promise<void> {
    if (type === "RNOH_WORKER_SCHEDULE_TASK_RESULT") {
      this.resolve(payload.taskId, payload.result);
    }
  }
}