/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * Imports from ETS to be done like that, otherwise ArkTS fails on runtime.
 */
import { EtsWorkerTurboModule, } from '@rnoh/react-native-openharmony/src/main/ets/RNOH/EtsWorkerTurboModule';
import { UITaskRunnable } from '@rnoh/react-native-openharmony/src/main/ets/RNOH/UITaskRunner';
import {
  AlertManagerTurboModule
} from '@rnoh/react-native-openharmony/src/main/ets/RNOHCorePackage/turboModules/AlertManagerTurboModule';
import { UITurboModuleContext } from '@rnoh/react-native-openharmony/ts';
import { TM } from "@rnoh/react-native-openharmony/generated/ts"

@Sendable
class RNVersionReaderParams {
  prefix: string

  constructor(prefix: string) {
    this.prefix = prefix
  }
}

@Sendable
class RNVersionReaderRunnable implements UITaskRunnable<RNVersionReaderParams, string> {
  run(uiTurboModuleContext: UITurboModuleContext, params: RNVersionReaderParams): string {
    uiTurboModuleContext.logger.clone("RNVersionReaderRunnable").debug("")
    return params.prefix + uiTurboModuleContext.reactNativeVersion
  }
}

@Sendable
class ShowAlertRunnable implements UITaskRunnable {
  run(uiTurboModuleContext: UITurboModuleContext) {
    uiTurboModuleContext.rnInstance.getTurboModule<AlertManagerTurboModule>(AlertManagerTurboModule.NAME).alert({
      title: "ShowAlertRunnable triggered this alert",
      cancelable: true,
    }, () => {
    }, () => {
    })
  }
}


export class SampleWorkerTurboModule extends EtsWorkerTurboModule implements TM.SampleWorkerTurboModule.Spec {
  runComputeIntensiveTask(): void {
    const endTime = Date.now() + 1000;
    let count = 0;
    while (Date.now() < endTime) {
      count++;
    }
  }

  runAsyncTask(): Promise<number> {
    return new Promise(resolve => {
      setTimeout(() => resolve(42), 3000)
    })
  }

  async getRNVersionFromUIContext(prefix: string) {
    const result: string =
      await this.ctx.runOnUIThread(new RNVersionReaderRunnable(), new RNVersionReaderParams(prefix))
    return result;
  }

  showAlert(): void {
    this.ctx.runOnUIThread(new ShowAlertRunnable(), undefined)
  }

  emitDeviceEvent(eventName: string, payload: number): void {
    this.ctx.rnInstance.emitDeviceEvent(eventName, payload)
  }

  private resolvePayloadProvidedToInvokeFromCpp: (payload: number) => void = () => {}
  async testCommunicationBetweenWorkerAndCpp(): Promise<boolean> {
    const receivedPayload = new Promise<number>(resolve => {
      this.resolvePayloadProvidedToInvokeFromCpp = resolve
    })
    this.ctx.rnInstance.postMessageToCpp("SAMPLE_WORKER_MESSAGE", 42)
    return (await receivedPayload) == 42
  }

  protected onInvokeFromCpp(messagePayload: number) {
    this.resolvePayloadProvidedToInvokeFromCpp(messagePayload)
  }

  getArchitecture(): string {
    return this.ctx.rnInstance.getArchitecture()
  }

  private resolvePromisedCallFromOtherTM = () => {}
  async testCommunicatingWithOtherWorkerTurboModules(): Promise<boolean> {
    await new Promise<void>((resolve) => {
      this.resolvePromisedCallFromOtherTM = resolve
      this.ctx.rnInstance.getWorkerTurboModule<SampleWorkerTurboModule>(TM.SampleWorkerTurboModule.NAME).onCallFromOtherTM()
    })
    return true;
  }

  onCallFromOtherTM() {
    this.resolvePromisedCallFromOtherTM()
  }

  __onDestroy__() {
    this.ctx.logger.info("SampleWorkerTurboModule::__onDestroy__")
  }
}
