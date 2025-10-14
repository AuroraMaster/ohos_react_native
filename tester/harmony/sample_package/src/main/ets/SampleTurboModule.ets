/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNOHError, Tag } from '@rnoh/react-native-openharmony/ts';
import { TM } from "@rnoh/react-native-openharmony/generated/ts"
import { EtsUITurboModule } from '@rnoh/react-native-openharmony/ets';
import { ErrorData } from '@rnoh/react-native-openharmony';
import { WorkerTaskRunnable, AnyThreadRNInstance } from '@rnoh/react-native-openharmony/src/main/ets/RNOH';

interface Constants {
  const1: boolean,
  const2: number,
  const3: string
}

interface Empty {}

interface Vec3 {
  x: number,
  y: string,
  z: Object,
}

interface CppMessagePayload {
  foo: string
}

interface EnumPack {
  enum1: TM.GeneratedSampleTurboModule.SomeEnum1;
  enum2: TM.GeneratedSampleTurboModule.SomeEnum2;
  enum3: TM.GeneratedSampleTurboModule.SomeEnum3;
  hardcodedEnum1: TM.GeneratedSampleTurboModule.SomeEnum1;
}

export class SampleTurboModule extends EtsUITurboModule implements TM.GeneratedSampleTurboModule.Spec, TM.GeneratedSampleTurboModule2.Spec {
  getConstants(): Object {
    return { const1: true, const2: 1228, const3: "something" } as Constants
  }

  getUnionValue(arg: null | Object): Object {
    return arg ?? {} as Empty
  }

  getRootTag(arg: Tag): Tag {
    return arg
  }

  getEnum(
    enum1: TM.GeneratedSampleTurboModule.SomeEnum1,
    enum2: TM.GeneratedSampleTurboModule.SomeEnum2,
    enum3: TM.GeneratedSampleTurboModule.SomeEnum3): EnumPack {
    return {
      enum1,
      enum2,
      enum3,
      hardcodedEnum1: TM.GeneratedSampleTurboModule.SomeEnum1.FOO
    }
  }

  voidFunc() {
    console.log('RNOH SampleTurboModule::voidFunc');
  }

  getBool(arg: boolean): boolean {
    console.log(`RNOH SampleTurboModule::getBool(${arg})`);
    return arg;
  }

  getNull(arg: null) {
    console.log(`RNOH SampleTurboModule::getNull(${arg})`);
    return arg;
  }

  getString(arg: string): string {
    console.log(`RNOH SampleTurboModule::getString(${arg})`);
    return arg;
  }

  getNumber(arg: number): number {
    console.log(`RNOH SampleTurboModule::getNumber(${arg})`);
    return arg;
  }

  getObject(arg: Object): Object {
    console.log(`RNOH SampleTurboModule::getObject(${arg})`);
    return arg;
  }

  getUnsafeObject(arg: Object): Object {
    console.log(`RNOH SampleTurboModule::getUnsafeObject(${arg})`);
    return arg;
  }

  getArray(args: Object[]): Object[] {
    console.log(`RNOH SampleTurboModule::getArray(${args})`);
    return args;
  }

  getValue(x: number, y: string, z: Object): Object {
    console.log(`RNOH SampleTurboModule::getValue(${x} ${y} ${z})`);
    return { x: x, y: y, z: z } as Vec3;
  }

  getValueWithCallback(onComplete: (value: string) => void): void {
    console.log(`RNOH SampleTurboModule::getValueWithCallback`);
    setTimeout(() => {
      onComplete?.('value from callback!');
    }, 1000);
  }

  getValueWithPromise(error: boolean): Promise<string> {
    console.log(`RNOH SampleTurboModule::getValueWithPromise(${error})`)
    if (error) {
      return Promise.reject('intentional promise rejection');
    }
    return Promise.resolve('result!');
  }

  registerFunction(onComplete: (value: string) => void): void {
    console.log(`RNOH SampleTurboModule::registerFunction + trigger`);
    setTimeout(() => {
      onComplete?.('... from native side');
    }, 1000);
  }

  doAsyncJob(shouldResolve: boolean): Promise<string> {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        if (shouldResolve) {
          resolve('resolved on native side');
        } else {
          reject('rejected on native side');
        }
      }, 1000);
    });
  }

  getPromisedArray() {
    return Promise.resolve([1, 2, 3])
  }


  displayRNOHError(data: ErrorData) {
    this.ctx.reportRNOHError(new RNOHError(data))
  }

  throwExceptionArk() {
    throw new Error("Exception thrown from ArkTS")
  }

  getUnknown(arg: Object) {
    return arg
  }

  async emitEventFromArkTS2Cpp(payload: CppMessagePayload): Promise<CppMessagePayload> {
    return await new Promise((resolve) => {
      const unsubscribe =
        this.ctx.rnInstance.cppEventEmitter.subscribe("SAMPLE_MESSAGE", (value: CppMessagePayload) => {
          resolve(value);
          unsubscribe();
        })
      this.ctx.rnInstance.postMessageToCpp("SAMPLE_MESSAGE", payload);
    })
  }

  setNativeResponderBlocked(isBlocked: boolean, origin: string, componentInstanceId: string): void {
    this.ctx.rnInstance.postMessageToCpp("BLOCK_NATIVE_RESPONDER", { isBlocked, origin, componentInstanceId })
  }

  // demonstrates calling `runOnWorkerThread` as well as
  // sharing state between the MAIN and Worker runtimes through `params`
  async calculateOnWorker(n: number): Promise<number> {
    const params = new FibonacciRunnableParams(n);
    return this.ctx.runOnWorkerThread(new FibonacciRunnable(), params);
  }
}

@Sendable
class FibonacciRunnableParams {
  public n: number

  constructor(n: number) {
    this.n = n;
  }
}

@Sendable
class FibonacciRunnable implements WorkerTaskRunnable<FibonacciRunnableParams> {
  run(rnInstance: AnyThreadRNInstance, params: FibonacciRunnableParams): number {
    const fibo = (n: number): number => {
      if (n <= 0) {
        return 0;
      }
      if (n <= 1) {
        return 1;
      }
      return fibo(n - 1) + fibo(n - 2);
    }

    return fibo(params.n);
  }
}
