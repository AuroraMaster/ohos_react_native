/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import worker from "@ohos.worker"
import { RNOHLogger } from "./RNOHLogger"
import { RNOHError } from "./RNOHError"

interface WorkerThreadDependencies {
  scriptUrl: string
  name: string
  logger: RNOHLogger
  onWorkerError: (rnohError: RNOHError) => void
}

type WorkerThreadMessageListener = (string, WorkerMessage) => void

/**
 * @internal
 */
export interface WorkerMessage {}

/**
 * @internal
 */
export class WorkerThread {
  static async create({ scriptUrl, name, logger, onWorkerError }: WorkerThreadDependencies) {
    const workerThread = new WorkerThread(logger, new worker.ThreadWorker(scriptUrl, { name: name }), onWorkerError)
    const interval = setInterval(() => {
      workerThread.postMessage("RNOH_WORKER_THREAD_READY")
    }, 100);
    setTimeout(() => {
      workerThread.postMessage("RNOH_WORKER_THREAD_READY")
    }, 0)
    await workerThread.waitForMessage("RNOH_WORKER_THREAD_READY_ACK")
    clearInterval(interval)
    return workerThread;
  }

  private logger: RNOHLogger
  private listeners: Set<WorkerThreadMessageListener> = new Set()

  private constructor(logger: RNOHLogger, private rawWorkerThread: worker.ThreadWorker,
    private onWorkerError: (rnohError: RNOHError) => void) {
    this.logger = logger.clone("WorkerThread")
    this.rawWorkerThread.onmessage = (ev) => {
      const messageType = ev.data?.type
      const messagePayload = ev.data?.payload ?? {}
      if (messageType) {
        this.logger.clone("receivedMessage").debug(`${messageType} ${JSON.stringify(messagePayload)}`)
        for (const listener of this.listeners) {
          listener(messageType, messagePayload)
        }
      }
    }
    this.listeners.add((type, payload) => {
      if (type === "RNOH_ERROR") {
        this.onWorkerError(new RNOHError({
          whatHappened: payload.message,
          howCanItBeFixed: [],
          customStack: payload.stack
        }))
      }
    })
  }

  postMessage(type: string, payload: any = undefined) {
    this.logger.clone("postMessage").debug(`${type} ${payload ? JSON.stringify(payload) : "undefined"}`)
    this.rawWorkerThread.postMessageWithSharedSendable({ type, payload })
  }

  async waitForMessage(expectedMsgType: string, filter: undefined | ((payload: any) => boolean) = undefined) {
    return new Promise<void>(resolve => {
      const unsubscribe = this.subscribeToMessages((type, payload) => {
        if (type === expectedMsgType && (filter === undefined || filter(payload ?? {}))) {
          resolve(undefined)
          unsubscribe()
        }
      })
    })
  }

  subscribeToMessages(listener: WorkerThreadMessageListener): () => void {
    this.listeners.add(listener)
    return () => {
      this.listeners.delete(listener)
    }
  }

  destroy() {
    this.listeners.clear()
    this.rawWorkerThread.terminate()
  }
}