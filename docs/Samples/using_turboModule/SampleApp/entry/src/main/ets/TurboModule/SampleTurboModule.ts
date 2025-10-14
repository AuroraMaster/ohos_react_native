/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { UITurboModule } from '@rnoh/react-native-openharmony/ts';
import type { UITurboModuleContext } from '@rnoh/react-native-openharmony/ts';
import emitter from '@ohos.events.emitter';
import dataPreferences from '@ohos.data.preferences';
import { BusinessError } from '@ohos.base';
import type common from '@ohos.app.ability.common';

export interface TurboModuleEventData {
  param: string;
}

export class SampleTurboModule extends UITurboModule {
  public static readonly NAME = 'SampleTurboModule';
  public static preferences: dataPreferences.Preferences | null = null;

  constructor(protected ctx: UITurboModuleContext) {
    super(ctx);
    this.initPreferences(ctx.uiAbilityContext);
  }

  private initPreferences(context: common.UIAbilityContext): void {
    try {
      let options: dataPreferences.Options = {
        name: 'test'
      };
      SampleTurboModule.preferences = dataPreferences.getPreferencesSync(context, options);
    } catch (err) {
      console.error('Failed to get preferences');
    }
  }

  pushStringToHarmony(arg: string, id?: number): string {
    if (id) {
      let data: TurboModuleEventData = { param: arg };
      emitter.emit({ eventId: id }, { data: data });
    }
    return arg;
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
      }, 100);
    });
  }

  rnLog(info: string) {
    console.log("ReactNative.log: " + info);
  }

  getPreferencesData(key: string): string {
    if (!SampleTurboModule.preferences) {
      return 'null';
    }
    try {
      let val = SampleTurboModule.preferences.getSync(key, '默认值') as string;
      return val;
    } catch (err) {
      return '未赋值';
    }
  }

  putPreferencesData(key: string, value: string) {
    if (!SampleTurboModule.preferences) {
      return;
    }
    SampleTurboModule.preferences.putSync(key, value);
    try {
      SampleTurboModule.preferences.flush((err: BusinessError) => {
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