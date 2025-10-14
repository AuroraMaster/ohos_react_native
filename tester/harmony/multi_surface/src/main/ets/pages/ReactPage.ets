/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { buildRNComponent, ComponentBuilderContext, RNComponentContext, RNOHContext, RNOHCoreContext, Tag } from 'rnoh';
import { RNAbility, RNInstance, RNSurface, SurfaceHandle } from 'rnoh'
import { SampleView, PropsDisplayer, GeneratedSampleView } from 'rnoh-sample-package'
import router from '@ohos.router'
import { SurfaceConfig1 } from 'rnoh/src/main/ets/RNSurface';

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


@Builder
export function buildRNComponentForTag(ctx: RNComponentContext, tag: Tag) {
  buildRNComponent(ctx, ctx.descriptorRegistry.findDescriptorWrapperByTag(tag)!)
}

interface PageParams {
  appKey: string
}

@Entry
@Component
struct ReactPage {
  @State appKey: string = (router.getParams() as PageParams).appKey
  @State surfaceHandle: SurfaceHandle | undefined = undefined;
  @State showSurface: boolean = false;
  @StorageLink('RNOHCoreContext') private rnohCoreContext: RNOHCoreContext | undefined = undefined
  @StorageLink('AppRNInstance') rnInstance: RNInstance | undefined = undefined

  onBackPress(): boolean {
    this.rnohCoreContext?.dispatchBackPress();
    return false;
  }

  getOrCreateSurface() {
    const surfacesMap: Record<string, SurfaceHandle> = AppStorage.get("ReactSurfaceByAppKey") ?? {};

    if (surfacesMap[this.appKey]) {
      return surfacesMap[this.appKey];
    }

    const surfaceHandle = this.rnInstance?.createSurface(this.appKey);
    if (surfaceHandle !== undefined) {
      surfaceHandle.start(
        {
          width: 0,
          height: 0,
          surfaceOffsetX: 0,
          surfaceOffsetY: 0,
          pixelRatio: vp2px(1),
          isRTL: false
        },
        {});
      surfacesMap[this.appKey] = surfaceHandle;
    }
    return surfaceHandle;
  }

  aboutToAppear() {
    this.surfaceHandle = this.getOrCreateSurface();
  }

  aboutToDisappear() {
  }

  build() {
    Column() {
      Row() {
        Navigator({ target: "pages/Index", type: NavigationType.Back }) {
          Text("Go back")
            .width('100%')
            .margin({ left: 12 })
            .fontColor(Color.Blue)
        }
      }.height("5%")


      if (!this.showSurface) {
        Row() {
          Button("Show Surface")
            .onClick(() => this.showSurface = !this.showSurface)
        }
        .height("5%")
        .enabled(this.surfaceHandle !== undefined)
      } else if (this.rnohCoreContext && this.rnInstance && this.surfaceHandle) {
        Row() {
          RNSurface({
            ctx: new RNComponentContext(
              RNOHContext.fromCoreContext(this.rnohCoreContext!, this.rnInstance),
              wrappedCustomRNComponentBuilder,
              wrapBuilder(buildRNComponentForTag),
              new Map(),
            ),
            surfaceConfig: { surfaceHandle: this.surfaceHandle } as SurfaceConfig1,
          })
        }
        .height("90%")
        .width("100%")
      }
    }
    .width("100%")
    .height("100%")
    .margin({ top: 32 })
  }
}