/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * Imports from ETS to be done like that, otherwise ArkTS fails on runtime.
 */
import { AnyThreadTurboModule, } from '@rnoh/react-native-openharmony/ts';
import { preferences } from '../entryability/EntryAbility';
import { BusinessError } from '@ohos.base';

export class SampleWorkerTurboModule extends AnyThreadTurboModule {
  registerFunction(onComplete: (value: string) => void): void {
    console.log(`RNOH SampleTurboModule::registerFunction + trigger`);
    setTimeout(() => {
      onComplete?.('... from native worker side');
    }, 1000);
  }

  doAsyncJob(shouldResolve: boolean): Promise<string> {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        if (shouldResolve) {
          resolve('resolved on native worker side');
        } else {
          reject('rejected on native worker side');
        }
      }, 100);
    });
  }

  rnLog(info: string) {
    console.log("ReactNative.log in Worker: " + info);
  }

  getPreferencesData(key: string): string {
    if (!preferences) {
      return 'null';
    }
    try {
      let val = preferences.getSync(key, '默认值') as string;
      return val;
    } catch (err) {
      return '未赋值';
    }
  }

  putPreferencesData(key: string, value: string) {
    if (!preferences) {
      return;
    }
    preferences.putSync(key, value);
    try {
      preferences.flush((err: BusinessError) => {
        if (err) {
          console.error('Failed to flush');
          return;
        }
        console.info('Succeeded in flushing.');
      })
    } catch (err) {
      console.error('Failed to flush');
    }
  }
}
