/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNInstance,
  RNOHCoreContext,
  ResourceJSBundleProvider,
  RNSurface,
  RNComponentContext,
  RNOHContext,
  buildRNComponentForTag,
  RNPackageContext,
  RNPackage,
  ComponentBuilderContext
} from '@rnoh/react-native-openharmony';
import { SampleTurboModulePackage } from '../TurboModule/SampleTurboModulePackage';

@Builder
export function buildCustomComponent(ctx: ComponentBuilderContext) {
  Stack() {
  }.position({ x: 0, y: 0 })
}

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomComponent);

export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [
    new SampleTurboModulePackage(ctx),
  ];
}

@Entry
@Component
export struct MultiSurface {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined;
  private instance: RNInstance | undefined;
  private bundlePath = 'bundle.harmony.js';
  private moduleName = 'App';
  @State isBundleReady: boolean = false;

  async aboutToAppear() {
    console.log("MultiSurface=====")
    if (!this.rnohCoreContext) {
      return;
    }

    this.instance = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: true,
      arkTsComponentNames: []
    });
    const ctxInstance: RNComponentContext = new RNComponentContext(
      RNOHContext.fromCoreContext(this.rnohCoreContext!, this.instance),
      wrapBuilder(buildCustomComponent),
      wrapBuilder(buildRNComponentForTag),
      new Map()
    );
    // await this.instance.runJSBundle(
    //   new ResourceJSBundleProvider(
    //     getContext().resourceManager,
    //     'bundle/basic/basic.harmony.bundle'
    //   )
    // );
    await this.instance.runJSBundle(
      new ResourceJSBundleProvider(
        getContext().resourceManager,
        this.bundlePath
      )).then(() => {
      this.isBundleReady = true;
      console.log('instance 加载完成' + this.bundlePath);
    })
  }

  @Builder
  MultiSurfaceItem(moduleName: string, instance: RNInstance) {
    Column() {
      RNSurface({
        surfaceConfig: {
          appKey: moduleName,
        },
        ctx: new RNComponentContext(
          RNOHContext.fromCoreContext(this.rnohCoreContext!, instance),
          wrappedCustomRNComponentBuilder,
          wrapBuilder(buildRNComponentForTag),
          new Map()
        ),
      })
    }
    .align(Alignment.Top)
    .width('100%')
  }

  build() {
    NavDestination() {
      Column() {
        if (this.isBundleReady) {
          this.MultiSurfaceItem(this.moduleName, this.instance!)
        }
      }
      .width('100%')
      .height('100%')
    }
  }
}
   