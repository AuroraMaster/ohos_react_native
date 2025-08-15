/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNOHPackage,
  UITurboModule,
  UITurboModuleContext
} from '@rnoh/react-native-openharmony';

import { SampleTurboModule } from './SampleTurboModule';
import { SampleTurboModule2 } from './SampleTurboModule2';

export class SampleTurboModulePackage extends RNOHPackage {
  override getUITurboModuleFactoryByNameMap(): Map<string, (ctx: UITurboModuleContext) => UITurboModule | null> {
    return new Map<string, ((ctx: UITurboModuleContext) => UITurboModule)>()
      .set('SampleTurboModule', (ctx) => new SampleTurboModule(ctx))
      .set('SampleTurboModule2',(ctx) => new SampleTurboModule2(ctx))
  }

  override async createEagerUITurboModuleByNameMap(ctx: UITurboModuleContext): Promise<Map<string, UITurboModule>> {
    const sampleTurboModule = new SampleTurboModule(ctx);
    const sampleTurboModule2 = new SampleTurboModule2(ctx);
    return new Map()
      .set('SampleTurboModule', sampleTurboModule)
      .set('SampleTurboModule2', sampleTurboModule2)
  }
}