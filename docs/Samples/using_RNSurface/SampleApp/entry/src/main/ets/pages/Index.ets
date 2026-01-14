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
  ComponentBuilderContext,
  TraceJSBundleProviderDecorator,
  AnyJSBundleProvider,
  FileJSBundleProvider,
  MetroJSBundleProvider
} from '@rnoh/react-native-openharmony';

@Builder
function buildCustomComponent(ctx: ComponentBuilderContext) {
  Stack() {
  }
  .position({ x: 0, y: 0 })
}

function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [];
}

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomComponent);

@Entry
@Component
export struct Index {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined
  private rnInstance: RNInstance | undefined;
  private bundlePath = 'bundle.harmony.js';
  private appKey = 'app_name';
  @State isBundleReady: boolean = false;
  private ctx!: RNComponentContext;
  private cleanUpCallbacks: (() => void)[] = [];

  aboutToAppear() {
    this.init();
  }

  aboutToDisappear(){
    this.cleanUp();
  }

  private async init() {
    if (!this.rnohCoreContext) {
      return;
    }
    this.rnInstance = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: true,
      arkTsComponentNames: []
    });
    this.ctx = new RNComponentContext(
      RNOHContext.fromCoreContext(this.rnohCoreContext!, this.rnInstance),
      wrappedCustomRNComponentBuilder,
      wrapBuilder(buildRNComponentForTag),
      new Map()
    )
    // 监听 RELOAD
    this.cleanUpCallbacks.push(this.ctx.devToolsController.eventEmitter.subscribe("RELOAD", async () => {
      await this.cleanUp()
      this.init()
      this.isBundleReady = false
    }))
    // 监听 jsBundle 状态
    this.cleanUpCallbacks.push(this.rnInstance.subscribeToLifecycleEvents("JS_BUNDLE_EXECUTION_FINISH", () => {
      this.isBundleReady = true
    }))
    // runJSBundle
    this.rnInstance.runJSBundle(new TraceJSBundleProviderDecorator(
      new AnyJSBundleProvider([
        new MetroJSBundleProvider(),
        new FileJSBundleProvider(`/data/storage/el2/base/files/${this.bundlePath}`),
        new ResourceJSBundleProvider(this.rnohCoreContext.uiAbilityContext.resourceManager, this.bundlePath)
      ]), this.rnohCoreContext.logger
    ))
  }

  private async cleanUp() {
    this.cleanUpCallbacks.forEach(cleanUp => cleanUp())
  }

  build() {
    Column() {
      if (this.isBundleReady) {
        Column() {
          RNSurface({
            surfaceConfig: {
              appKey: this.appKey,
            },
            ctx: this.ctx
          })
        }
        .width('100%')
        .height('100%')
      }
    }
  }
}
   