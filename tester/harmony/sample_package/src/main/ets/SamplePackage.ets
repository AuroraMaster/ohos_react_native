/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNOHPackage,
  DescriptorWrapperFactoryByDescriptorType,
  DescriptorWrapperFactoryByDescriptorTypeCtx,
  UITurboModuleFactory,
  UITurboModuleContext,
  WorkerTurboModuleFactory,
  WorkerTurboModuleContext,
  ComponentBuilderContext
} from '@rnoh/react-native-openharmony';
import { RNC, TM } from "@rnoh/react-native-openharmony/generated/ts"
import { SampleTurboModule } from './SampleTurboModule';
import { SampleWorkerTurboModule } from './SampleWorkerTurboModule';
import { RNC as RNC2, TM as TM2 } from "./generated"
import { SampleLibGenTurboModule } from "./SampleLibGenTurboModule"
import { SampleView } from "./SampleView"
import { PropsDisplayer } from "./PropsDisplayer"
import { GeneratedSampleView } from "./GeneratedSampleView"
import { CodegenLibSampleArkTSComponent } from "./CodegenLibSampleArkTSComponent"
import { ContainerView } from "./ContainerView"

class SampleTurboModulesFactory extends UITurboModuleFactory {
  createTurboModule(name: string) {
    if (name === 'SampleTurboModule' || name === TM.GeneratedSampleTurboModule.NAME ||
      name === TM.GeneratedSampleTurboModule2.NAME) {
      return new SampleTurboModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === 'SampleTurboModule' || name === TM.GeneratedSampleTurboModule.NAME ||
      name === TM.GeneratedSampleTurboModule2.NAME;
  }
}

class SampleWorkerTurboModulesFactory extends WorkerTurboModuleFactory {
  createTurboModule(name: string) {
    if (name === TM.SampleWorkerTurboModule.NAME) {
      return new SampleWorkerTurboModule(this.ctx);
    } else if (name === TM2.CodegenLibSampleModule.NAME) {
      return new SampleLibGenTurboModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === TM.SampleWorkerTurboModule.NAME || name === TM2.CodegenLibSampleModule.NAME
  }
}


@Builder
export function buildSampleView(ctx: ComponentBuilderContext) {
  SampleView({
    ctx: ctx.rnComponentContext,
    tag: ctx.tag,
  })
}

@Builder
export function buildPropsDisplayer(ctx: ComponentBuilderContext) {
  PropsDisplayer({
    ctx: ctx.rnComponentContext,
    tag: ctx.tag
  })
}

@Builder
export function buildGeneratedSampleView(ctx: ComponentBuilderContext) {
  GeneratedSampleView({
    ctx: ctx.rnComponentContext,
    tag: ctx.tag
  })
}

@Builder
export function buildCodegenLibSampleArkTSComponent(ctx: ComponentBuilderContext) {
  CodegenLibSampleArkTSComponent({
    ctx: ctx.rnComponentContext,
    tag: ctx.tag
  })
}

@Builder
export function buildContainerView(ctx: ComponentBuilderContext) {
  ContainerView({
    ctx: ctx.rnComponentContext,
    tag: ctx.tag
  })
}

export class SamplePackage extends RNOHPackage {
  createUITurboModuleFactory(ctx: UITurboModuleContext) {
    return new SampleTurboModulesFactory(ctx);
  }

  createWorkerTurboModuleFactory(ctx: WorkerTurboModuleContext) {
    return new SampleWorkerTurboModulesFactory(ctx);
  }

  createDescriptorWrapperFactoryByDescriptorType(ctx: DescriptorWrapperFactoryByDescriptorTypeCtx): DescriptorWrapperFactoryByDescriptorType {
    const result: DescriptorWrapperFactoryByDescriptorType = {}
    result[RNC.GeneratedSampleViewArkTS.NAME] =
      (ctx2) => new RNC.GeneratedSampleViewArkTS.DescriptorWrapper(ctx2.descriptor)
    result[RNC2.CodegenLibArkTSSampleComponent.NAME] =
      (ctx2) => new RNC2.CodegenLibArkTSSampleComponent.DescriptorWrapper(ctx2.descriptor)
    return result
  }

  createWrappedCustomRNComponentBuilderByComponentNameMap(): Map<string, WrappedBuilder<[ComponentBuilderContext]>> {
    return new Map()
      .set(SampleView.NAME, wrapBuilder(buildSampleView))
      .set(PropsDisplayer.NAME, wrapBuilder(buildPropsDisplayer))
      .set(CodegenLibSampleArkTSComponent.NAME, wrapBuilder(buildCodegenLibSampleArkTSComponent))
      .set(ContainerView.NAME, wrapBuilder(buildContainerView))
    // .set(GeneratedSampleView.NAME, wrapBuilder(buildGeneratedSampleView)) // commented out on purpose — this component is registered using the old approach for testing purposes
  }
}
