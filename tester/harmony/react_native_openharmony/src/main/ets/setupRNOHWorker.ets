/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { lang, worker, MessageEvents } from '@kit.ArkTS';
import { HttpClient, DefaultHttpClient, CAPathProvider } from "./HttpClient/HttpClient"
import {
  RNPackage,
  StandardRNOHLogger,
  TurboModuleProvider,
  NapiBridge,
  RNOHLogger,
  RNPackageContext,
  WorkerTurboModuleFactory,
  RNOHError,
  WorkerRNInstanceInternal,
  WorkerTurboModule,
  SafeAreaInsetsProvider,
  AnyThreadTurboModule,
} from "./RNOH/ts"
import { RNOHCorePackage } from "./RNOHCorePackage/Package"
import { EtsWorkerTurboModuleContext } from "./RNOH/EtsRNOHContext"
import { UITaskRunner, UITask } from "./RNOH/UITaskRunner"
import { WorkerTask } from "./RNOH/WorkerTaskRunner"
import common from '@ohos.app.ability.common'


/**
 * @api
 */
export interface WorkerRNInstanceConfig {
  thirdPartyPackagesFactory: (ctx: RNPackageContext) => RNPackage[]
  httpClient?: HttpClient
  caPathProvider?: CAPathProvider
}

/**
 * @api: RN_APP_DEVELOPER
 */
interface RNOHWorkerConfig {
  createWorkerRNInstanceConfig: ((rnInstanceName: string) => WorkerRNInstanceConfig)
}

interface CreateRNInstanceWorkerEnvPayload {
  rnInstanceName: string,
  rnInstanceId: number
  uiAbilityContext: common.UIAbilityContext
  architecture: "C_API" | "ARK_TS"
  assetsDest: string
  launchUri: string
}

class CreateRNInstanceWorkerEnvAckPayload {
  rnInstanceId: number

  constructor(rnInstanceId: number) {
    this.rnInstanceId = rnInstanceId
  }
}

interface TurboModuleUITaskResultPayload {
  taskId: string
  rnInstanceId: number
  result: Object
}

interface DestroyWorkerRNInstancePayload {
  rnInstanceId: number
}

interface JSBundleExecutionFinish {
  rnInstanceId: number
  bundleURL: string
}

class DestroyWorkerRNInstanceAckPayload {
  rnInstanceId: number

  constructor(rnInstanceId: number) {
    this.rnInstanceId = rnInstanceId
  }
}

@Sendable
class UITaskMessagePayload {
  task: UITask
  rnInstanceId: number

  constructor(task: UITask, rnInstanceId: number) {
    this.task = task
    this.rnInstanceId = rnInstanceId
  }
}

@Sendable
class SendableMessage<TPayload extends lang.ISendable> {
  type: string;
  payload: TPayload;

  constructor(type: string, payload: TPayload) {
    this.type = type
    this.payload = payload
  }
}

@Sendable
class SendableRNOHError {
  message: string
  stack: string

  constructor(rnohError: RNOHError) {
    this.message = rnohError.getMessage();
    this.stack = rnohError.getStack()?.toString() ?? ""
  }
}

interface WorkerTurboModuleDeps {
  uiTaskRunner: UITaskRunner
  rnInstance: WorkerRNInstanceInternal
  turboModuleProvider: TurboModuleProvider<WorkerTurboModule | AnyThreadTurboModule>
}

interface WorkerScheduleTaskResultPayload {
  rnInstanceId: number
  taskId: string,
  result: lang.ISendable | undefined
}

/**
 * @api: RN_APP_DEVELOPER
 * @thread: WORKER
 */
export async function setupRNOHWorker(config: RNOHWorkerConfig) {
  const workerPort = worker.workerPort
  const workerLogger = new StandardRNOHLogger().setThreadPrefix("___█_")
  const localLogger = workerLogger.clone(`setupRNOHWorker`)
  localLogger.info(`initializing`)
  const napiBridge = new NapiBridge(localLogger)
  const turboModuleDepsByRNInstanceId = new Map<number, WorkerTurboModuleDeps>()

  const postMessage = (type: string, payload: Object | undefined = undefined) => {
    localLogger.debug(`postMessage: ${type}`)
    workerPort.postMessage({ type, payload })
  }

  workerPort.onmessage = async (e: MessageEvents) => {
    try {
      const messageType = e.data?.type as string
      const messagePayload = e.data?.payload as Object
      if (messageType && typeof messageType === "string") {
        localLogger.debug(`receivedMessage: ${messageType}`)
        switch (messageType) {
          case "RNOH_WORKER_THREAD_READY":
            postMessage("RNOH_WORKER_THREAD_READY_ACK")
            break;
          case "RNOH_CREATE_WORKER_RN_INSTANCE": {
            const typedMessagePayload = messagePayload as CreateRNInstanceWorkerEnvPayload
            const rnInstanceName = typedMessagePayload.rnInstanceName;
            const rnInstanceId = typedMessagePayload.rnInstanceId;
            const uiAbilityContext = typedMessagePayload.uiAbilityContext
            const launchUri = typedMessagePayload.launchUri
            if (turboModuleDepsByRNInstanceId.has(rnInstanceId)) {
              throw new RNOHError({
                whatHappened: `Tried to re-create UITaskRunner (rnInstanceId=${rnInstanceId})`,
                howCanItBeFixed: []
              })
            }
            const uiTaskRunner = new UITaskRunner(rnInstanceId.toString(), (uiTask) => {
              workerPort.postMessageWithSharedSendable(new SendableMessage("RNOH_TURBO_MODULE_UI_TASK",
                new UITaskMessagePayload(uiTask, rnInstanceId)));
            })
            const rnInstanceConfig = config.createWorkerRNInstanceConfig(rnInstanceName)
            const packages = rnInstanceConfig.thirdPartyPackagesFactory({})
            let turboModuleProvider: TurboModuleProvider<WorkerTurboModule | AnyThreadTurboModule> | undefined
            const rnInstance = new WorkerRNInstanceInternal(rnInstanceId, napiBridge, typedMessagePayload.architecture,
              typedMessagePayload.assetsDest,
              () => turboModuleProvider)
            const etsWorkerTurboModuleContext =
              new EtsWorkerTurboModuleContext({
                workerTurboModuleContextDependencies: {
                  logger: workerLogger,
                  uiAbilityContext,
                  rnInstance,
                  httpClient: rnInstanceConfig.httpClient ?? new DefaultHttpClient({ logger: workerLogger }),
                  caPathProvider: rnInstanceConfig.caPathProvider ?? ((url: string) => ''),
                  safeAreaInsetsProvider: new SafeAreaInsetsProvider(uiAbilityContext),
                  launchUri
                },
                uiTaskRunner: uiTaskRunner
              })
            turboModuleProvider =
              await createTurboModuleProvider(etsWorkerTurboModuleContext, packages, localLogger)
            turboModuleDepsByRNInstanceId.set(rnInstanceId, {
              uiTaskRunner,
              rnInstance,
              turboModuleProvider,
            })
            napiBridge.registerWorkerTurboModuleProvider(turboModuleProvider, rnInstanceId)
            localLogger.info("registered WorkerTurboModuleProvider")
            postMessage("RNOH_CREATE_WORKER_RN_INSTANCE_ACK", new CreateRNInstanceWorkerEnvAckPayload(rnInstanceId))
            break;
          }
          case "JS_BUNDLE_EXECUTION_FINISH": {
            const jsBundleExecutionFinishPayload = (messagePayload as JSBundleExecutionFinish)
            if (!turboModuleDepsByRNInstanceId.has(jsBundleExecutionFinishPayload.rnInstanceId)) {
              localLogger.error("Couldn't find tmDeps (JS_BUNDLE_EXECUTION_FINISH)")
              return;
            }
            const rnInstance =
              turboModuleDepsByRNInstanceId.get(jsBundleExecutionFinishPayload.rnInstanceId)!.rnInstance
            rnInstance.flushRNFunctionCalls()
            rnInstance.setBunleUrl(jsBundleExecutionFinishPayload.bundleURL)
            break;
          }
          case "RNOH_TURBO_MODULE_UI_TASK_RESULT":
            const taskResultMessagePayload = messagePayload as TurboModuleUITaskResultPayload
            if (!turboModuleDepsByRNInstanceId.has(taskResultMessagePayload.rnInstanceId)) {
              throw new RNOHError({
                whatHappened: "Received UI Task result but UITaskRunner isn't created",
                howCanItBeFixed: []
              })
            }
            turboModuleDepsByRNInstanceId.get(taskResultMessagePayload.rnInstanceId)!.uiTaskRunner.resolve(taskResultMessagePayload.taskId,
              taskResultMessagePayload.result as lang.ISendable)
            break;
          case "RNOH_DESTROY_WORKER_RN_INSTANCE":
            const destroyWorkerRNInstancePayload = messagePayload as DestroyWorkerRNInstancePayload
            const deps = turboModuleDepsByRNInstanceId.get(destroyWorkerRNInstancePayload.rnInstanceId)
            if (!deps) {
              throw new RNOHError({
                whatHappened: "Tried to destroy not existing WorkerRNInstance",
                howCanItBeFixed: []
              })
            }
            deps.turboModuleProvider.onDestroy()
            turboModuleDepsByRNInstanceId.delete(destroyWorkerRNInstancePayload.rnInstanceId)
            postMessage("RNOH_DESTROY_WORKER_RN_INSTANCE_ACK",
              new DestroyWorkerRNInstanceAckPayload(destroyWorkerRNInstancePayload.rnInstanceId))
            break;
          case "RNOH_WORKER_SCHEDULE_TASK":
            const task = messagePayload as WorkerTask;
            const rnInstanceId = task.rnInstanceId;
            const turboModuleDeps = turboModuleDepsByRNInstanceId.get(rnInstanceId);
            if (!turboModuleDeps) {
              throw new RNOHError({
                whatHappened: "Tried to schedule task in a not existing WorkerRNInstance",
                howCanItBeFixed: []
              })
            }
            const result = task.runnable.run(turboModuleDeps.rnInstance, task.params);
            postMessage("RNOH_WORKER_SCHEDULE_TASK_RESULT", {
              rnInstanceId,
              taskId: task.id,
              result
            } as WorkerScheduleTaskResultPayload);
            break;
        }
      }
    } catch (err) {
      if (err instanceof RNOHError) {
        localLogger.error(err)
        workerPort.postMessageWithSharedSendable(new SendableMessage("RNOH_ERROR", new SendableRNOHError(err)))
        return;
      } else {
        throw (err as Error);
      }
    }
  }
}

async function createTurboModuleProvider(etsWorkerTurboModuleContext: EtsWorkerTurboModuleContext,
  thirdPartyPackages: RNPackage[],
  logger: RNOHLogger) {
  const packages = [...thirdPartyPackages]
  packages.unshift(new RNOHCorePackage({}));
  const factories = (await Promise.all(packages.map(async (pkg, idx) => {
    const pkgDebugName = pkg.getDebugName()
    let loggerName = `package${idx + 1}`
    if (pkgDebugName) {
      loggerName += `: ${pkgDebugName}`
    }
    logger.clone(loggerName).debug("")
    const workerTurboModuleFactory = pkg.createWorkerTurboModuleFactory(etsWorkerTurboModuleContext);
    const anyThreadTurboModuleFactory = pkg.createAnyThreadTurboModuleFactory(etsWorkerTurboModuleContext);
    await Promise.all([workerTurboModuleFactory?.prepareEagerTurboModules(), anyThreadTurboModuleFactory?.prepareEagerTurboModules()]);
    return [workerTurboModuleFactory, anyThreadTurboModuleFactory]
  }))).flat();
  return new TurboModuleProvider<WorkerTurboModule | AnyThreadTurboModule>(
    factories.filter(tmf => tmf != null) as WorkerTurboModuleFactory[],
    logger
  )
}
