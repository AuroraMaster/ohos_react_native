/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  AnyJSBundleProvider,
  ComponentBuilderContext,
  FileJSBundleProvider,
  MetroJSBundleProvider,
  ResourceJSBundleProvider,
  RNApp,
  RNOHErrorDialog,
  RNOHLogger,
  TraceJSBundleProviderDecorator,
  RNOHCoreContext,
  RNAbility
} from '@rnoh/react-native-openharmony';
import { createRNPackages } from '../RNPackagesFactory';
import fs from '@ohos.file.fs';

@Builder
export function buildCustomRNComponent(ctx: ComponentBuilderContext) {
};
const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomRNComponent);

@Entry
@Component
struct Index {
  @StorageLink('RNOHCoreContext') private rnohCoreContext: RNOHCoreContext | undefined = undefined;
  @State shouldShow: boolean = false;
  private logger!: RNOHLogger;
  @StorageLink('RNAbility') rnAbility: RNAbility | undefined = undefined;
  // bundlePath 和 assetsPath 都是相对于 rnAbility.context.filesDir目录下的路径。
  bundlePath: string = 'sandbox.harmony.bundle';
  assetsPath: string = 'assets';
  @State hasBundle: Boolean = false;
  @State bundleStatus: string = '';

  aboutToAppear() {
    this.logger = this.rnohCoreContext!.logger.clone("Index");
    const stopTracing = this.logger.clone("aboutToAppear").startTracing();
    this.shouldShow = true;
    stopTracing();
    this.checkBundleUpdated();
  }

  async downloadBundle() {
    if (this.rnAbility) {
      // 将bundlejs下载并保存到沙箱。
      let uint8Array =
        await this.rnAbility.context.resourceManager.getRawFileContent('rawfile/bundle/sandbox.harmony.bundle');
      let rawBuffer = uint8Array.buffer;
      let num = rawBuffer.byteLength;
      // 获取沙箱路径
      const sandboxDir = this.rnAbility.context.filesDir;
      const bundlePath = sandboxDir + '/' + this.bundlePath;
      let stream = fs.createStreamSync(bundlePath, 'w');
      stream.writeSync(rawBuffer);
      stream.closeSync();
      this.hasBundle = true;
    }
  }

  checkBundleUpdated(): void {
    if (this.rnAbility) {
      const sandboxDir = this.rnAbility.context.filesDir;
      const bundlePath = sandboxDir + '/' + this.bundlePath;
      try {
        const stat = fs.statSync(bundlePath);
        if (stat.size == 0) {
          this.downloadBundle();
        } else {
          this.hasBundle = true;
        }
      } catch (e) {
        this.downloadBundle();
      }
    }
  }

  onBackPress(): boolean | undefined {
    // NOTE: this is required since `Ability`'s `onBackPressed` function always。
    // terminates or puts the app in the background, but we want Ark to ignore it completely。
    // when handled by RN。
    this.rnohCoreContext!.dispatchBackPress();
    return true;
  }

  build() {
    Column() {
      if (this.rnohCoreContext && this.shouldShow) {
        if (this.rnohCoreContext?.isDebugModeEnabled) {
          RNOHErrorDialog({ ctx: this.rnohCoreContext });
        }
        if (this.rnAbility && this.hasBundle) {
          RNApp({
            rnInstanceConfig: {
              createRNPackages,
              enableCAPIArchitecture: true,
              arkTsComponentNames: []
            },
            appKey: "Sandbox",
            wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
            onSetUp: (rnInstance) => {
              rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP")
            },
            jsBundleProvider: new FileJSBundleProvider(this.rnAbility.context.filesDir + '/' + this.bundlePath)
          })
        } else {
          Row() {
            Text('未加载bundle')
            Text(this.bundleStatus)
          }
          .height(100)
        }
      }
    }
    .height('100%')
    .width('100%')
  }
}
