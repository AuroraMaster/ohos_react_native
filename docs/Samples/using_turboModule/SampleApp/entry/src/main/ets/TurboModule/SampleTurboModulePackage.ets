/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNOHPackage,
  AnyThreadTurboModule,
  AnyThreadTurboModuleContext,
  UITurboModule,
  UITurboModuleContext
} from '@rnoh/react-native-openharmony';

import { SampleTurboModule } from './SampleTurboModule';
import { SampleTurboModule2 } from './SampleTurboModule2';
import { SampleWorkerTurboModule } from './SampleWorkerTurboModule';

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

  override getAnyThreadTurboModuleFactoryByNameMap(): Map<string, (ctx: AnyThreadTurboModuleContext) => AnyThreadTurboModule | null> {
    return new Map<string, ((ctx: AnyThreadTurboModuleContext) => AnyThreadTurboModule)>()
      .set('SampleWorkerTurboModule', (ctx) => new SampleWorkerTurboModule(ctx))
  }

  override async createEagerAnyThreadTurboModuleByNameMap(ctx: AnyThreadTurboModuleContext): Promise<Map<string, AnyThreadTurboModule>> {
    const sampleWorkerTurboModule = new SampleWorkerTurboModule(ctx);
    return new Map()
      .set('SampleWorkerTurboModule', sampleWorkerTurboModule)
  }
}