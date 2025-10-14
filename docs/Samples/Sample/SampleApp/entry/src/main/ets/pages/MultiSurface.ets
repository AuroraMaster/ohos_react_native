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
  CustomRNComponentFrameNodeFactory,
  buildRNComponentForTag
} from '@rnoh/react-native-openharmony';
import { createRNPackages, buildCustomComponent, ENABLE_CAPI_ARCHITECTURE } from '../rn';
import { arkTsComponentNames } from '../rn/LoadBundle';

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomComponent);

@Entry
@Component
export struct MultiSurface {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined;
  private instance1: RNInstance | undefined;
  private instance2: RNInstance | undefined;
  private instance3: RNInstance | undefined;
  private bundlePath1 = 'bundle/cp/homepage.harmony.bundle';
  private moduleName1 = 'HomePage';
  private bundlePath2 = 'bundle/cp/goods.harmony.bundle';
  private moduleName2 = 'Goods';
  private bundlePath3 = 'bundle/bp/details.harmony.bundle';
  private moduleName3 = 'Details';
  @State isBundleReady1: boolean = false;
  @State isBundleReady2: boolean = false;
  @State isBundleReady3: boolean = false;
  container: Record<string, string | number> = {
    "backgroundColor": '#E6E6E6',
    "flex": 1,
    "padding": 20,
  }
  apiFontSize: Record<string, string | number> = {
    'fontSize': 30,
    'color': 'white'
  }
  styles: Record<string, object> = {
    'container': this.container,
    'apiFontSize': this.apiFontSize
  }
  initProps: Record<string, string | object> = {
    'stringParam': 'ArkTS传递给RN的参数',
    'styles': this.styles
  };

  async aboutToAppear() {
    console.log("MultiSurface=====")
    if (!this.rnohCoreContext) {
      return;
    }

    this.instance1 = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: ENABLE_CAPI_ARCHITECTURE,
      arkTsComponentNames: arkTsComponentNames
    });
    const ctxInstance1: RNComponentContext = new RNComponentContext(
      RNOHContext.fromCoreContext(this.rnohCoreContext!, this.instance1),
      wrapBuilder(buildCustomComponent),
      wrapBuilder(buildRNComponentForTag),
      new Map()
    );
    await this.instance1.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager,
      'bundle/basic/basic.harmony.bundle'));
    await this.instance1.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager, this.bundlePath1))
      .then(() => {
        this.isBundleReady1 = true;
        console.log('instance1 加载完成' + this.bundlePath1);
      })

    this.instance2 = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: ENABLE_CAPI_ARCHITECTURE,
      arkTsComponentNames: arkTsComponentNames
    });
    const ctxInstance2: RNComponentContext = new RNComponentContext(
      RNOHContext.fromCoreContext(this.rnohCoreContext!, this.instance2),
      wrapBuilder(buildCustomComponent),
      wrapBuilder(buildRNComponentForTag),
      new Map()
    );
    await this.instance2.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager,
      'bundle/basic/basic.harmony.bundle'));
    await this.instance2.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager, this.bundlePath2))
      .then(() => {
        this.isBundleReady2 = true;
        console.log('instance2 加载完成' + this.bundlePath2);
      })

    this.instance3 = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: ENABLE_CAPI_ARCHITECTURE,
      arkTsComponentNames: arkTsComponentNames
    });
    const ctxInstance3: RNComponentContext = new RNComponentContext(
      RNOHContext.fromCoreContext(this.rnohCoreContext!, this.instance3),
      wrapBuilder(buildCustomComponent),
      wrapBuilder(buildRNComponentForTag),
      new Map()
    );
    await this.instance3.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager, this.bundlePath3))
      .then(() => {
        this.isBundleReady3 = true;
        console.log('instance3 加载完成' + this.bundlePath3);
      })
  }

  @Builder
  MultiSurfaceItem(moduleName: string, instance: RNInstance) {
    Column() {
      RNSurface({
        surfaceConfig: {
          appKey: moduleName,
          initialProps: this.initProps,
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
    // .margin({ top: 10 })
    .width('100%')
    .height('30%')
  }

  build() {
    NavDestination() {
      Column() {
        // It's OK
        // if (this.isBundleReady1 && this.isBundleReady2 && this.isBundleReady3) {
        //   this.MultiSurfaceItem(this.moduleName1, this.instance1!)
        //   this.MultiSurfaceItem(this.moduleName2, this.instance2!)
        //   this.MultiSurfaceItem(this.moduleName3, this.instance3!)
        // }
        // It's OK
        if (this.isBundleReady1) {
          this.MultiSurfaceItem(this.moduleName1, this.instance1!);
        }
        if (this.isBundleReady2) {
          this.MultiSurfaceItem(this.moduleName2, this.instance2!);
        }
        if (this.isBundleReady3) {
          this.MultiSurfaceItem(this.moduleName3, this.instance3!);
        }
      }
      .width('100%')
      .height('100%')
    }
  }
}
   