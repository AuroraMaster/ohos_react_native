/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNOHPackage,
  UITurboModuleContext,
  ComponentBuilderContext,
  AnyThreadTurboModuleContext,
  AnyThreadTurboModule,
  UITurboModule,
} from '@rnoh/react-native-openharmony';
import type {
  DescriptorWrapperFactoryByDescriptorTypeCtx,
  DescriptorWrapperFactoryByDescriptorType
} from '@rnoh/react-native-openharmony/ts';
import { RNC } from "./generated/ts";
import { SelectBox } from './SelectBox';
import { SampleTurboModule } from './SampleTurboModule';
import { SampleAnyThreadTurboModule } from './SampleAnyThreadTurboModule';

@Builder
function buildSelectBox(ctx: ComponentBuilderContext) {
  SelectBox({ ctx: ctx.rnComponentContext, tag: ctx.tag, })
}

export class ThirdPartyLibrarySamplePackage extends RNOHPackage {
  createDescriptorWrapperFactoryByDescriptorType(ctx: DescriptorWrapperFactoryByDescriptorTypeCtx): DescriptorWrapperFactoryByDescriptorType {
    const result: DescriptorWrapperFactoryByDescriptorType = {}
    result[RNC.SelectBox.NAME] = (ctx) => new RNC.SelectBox.DescriptorWrapper(ctx.descriptor)
    return result
  }

  override getUITurboModuleFactoryByNameMap(): Map<string, (ctx: UITurboModuleContext) => UITurboModule | null> {
    return new Map<string, ((ctx: UITurboModuleContext) => UITurboModule)>()
      .set(SampleTurboModule.NAME, (ctx) => new SampleTurboModule(ctx))
  }

  override getAnyThreadTurboModuleFactoryByNameMap(): Map<string, (ctx: AnyThreadTurboModuleContext) => AnyThreadTurboModule | null> {
    return new Map<string, ((ctx: AnyThreadTurboModuleContext) => AnyThreadTurboModule)>()
      .set(SampleAnyThreadTurboModule.NAME, (ctx) => new SampleAnyThreadTurboModule(ctx))
  }

  createWrappedCustomRNComponentBuilderByComponentNameMap(): Map<string, WrappedBuilder<[ComponentBuilderContext]>> {
    return new Map().set(SelectBox.NAME, wrapBuilder(buildSelectBox))
  }
}