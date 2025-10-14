/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { ComponentBuilderContext, RNOHCoreContext } from 'rnoh';
import { RNApp, RNAbility, AnyJSBundleProvider, MetroJSBundleProvider, ResourceJSBundleProvider, } from 'rnoh'
import { SampleView, PropsDisplayer, GeneratedSampleView } from "rnoh-sample-package"
import { createRNPackages } from '../RNPackagesFactory'

@Builder
export function buildCustomRNComponent(ctx: ComponentBuilderContext) {
  // There seems to be a problem with the placement of ArkTS components in mixed mode. Nested Stack temporarily avoided.
  Stack() {
    if (ctx.componentName === SampleView.NAME) {
      SampleView({
        ctx: ctx.rnComponentContext,
        tag: ctx.tag,
      })
    }
    if (ctx.componentName === GeneratedSampleView.NAME) {
      GeneratedSampleView({
        ctx: ctx.rnComponentContext,
        tag: ctx.tag,
      })
    }
    if (ctx.componentName === PropsDisplayer.NAME) {
      PropsDisplayer({
        ctx: ctx.rnComponentContext,
        tag: ctx.tag
      })
    }
  }
  .position({ x: 0, y: 0 })

}

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomRNComponent)

@Entry
@Component
struct ReactApplicationPage {
  @StorageLink('RNOHCoreContext') private rnohCoreContext: RNOHCoreContext | undefined = undefined

  onBackPress(): boolean {
    this.rnohCoreContext?.dispatchBackPress();
    return false;
  }

  build() {
    Column() {
      RNApp({
        rnInstanceConfig: {
          createRNPackages,
          enableNDKTextMeasuring: true,
          enableBackgroundExecutor: false,
          enableCAPIArchitecture: true,
        },
        appKey: "app_name",
        wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
        onSetUp: (rnInstance) => {
          rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP")
        },
        jsBundleProvider: new AnyJSBundleProvider([new MetroJSBundleProvider(), new ResourceJSBundleProvider(this.rnohCoreContext!.uiAbilityContext.resourceManager)]),
      })
    }
    .height('100%')
    .width('100%')
  }
}
