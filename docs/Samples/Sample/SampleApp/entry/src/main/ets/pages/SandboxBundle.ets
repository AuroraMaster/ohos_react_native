/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNApp,
  TraceJSBundleProviderDecorator,
  AnyJSBundleProvider,
  RNOHLogger,
  RNOHCoreContext,
  RNAbility,
  FileJSBundleProvider,
  ComponentBuilderContext,
} from '@rnoh/react-native-openharmony'
import fs from '@ohos.file.fs';
import { createRNPackages } from '../rn/RNPackagesFactory';
import { arkTsComponentNames } from '../rn/LoadBundle';

@Builder
function buildCustomComponent(ctx: ComponentBuilderContext) {
}

@Component
export default struct SandBoxApp {
  @Consume('pagePathsHome') pagePathsHome: NavPathStack;
  @StorageLink('RNAbility') rnAbility: RNAbility | undefined = undefined;
  @StorageLink('RNOHCoreContext') private rnohCoreContext: RNOHCoreContext | undefined = undefined;
  // bundlePath 和 assetsPath 都是相对于 rnAbility.context.filesDir目录下的路径
  bundlePath: string = 'sandbox.harmony.bundle';
  assetsPath: string = 'assets';
  @State hasBundle: Boolean = false;
  @State bundleStatus: string = '';

  @Builder
  PageMap(name: string) {
    Text("Empty View");
  }

  @Builder
  public buildCustomComponent(ctx: ComponentBuilderContext) {
  }

  aboutToAppear(): void {
    this.checkBundleUpdated();
  }

  async downloadBundle() {
    if (this.rnAbility) {
      // 将bundlejs下载并保存到沙箱
      let uint8Array =
        await this.rnAbility.context.resourceManager.getRawFileContent('rawfile/bundle/bp/sandbox.harmony.bundle');
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

  build() {
    NavDestination() {
      Row() {
        Text('紫色区域由ArkTS渲染')
          .fontSize(30)
          .fontWeight(30)
          .margin({ top: 10 })
      }
      .height(100)
      .width('90%')
      .margin({ top: 10 })
      .backgroundColor('#a0a0ff')
      .borderRadius(10)

      if (this.rnAbility && this.hasBundle) {
        RNApp({
          rnInstanceConfig: {
            createRNPackages,
            enableNDKTextMeasuring: true,
            enableBackgroundExecutor: false,
            enableCAPIArchitecture: true,
            arkTsComponentNames: [],
          },
          appKey: "Sandbox",
          initialProps: { 'params1': '哈哈哈' },
          wrappedCustomRNComponentBuilder: wrapBuilder(buildCustomComponent),
          onSetUp: (rnInstance) => {
            rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP");
          },
          jsBundleProvider: new TraceJSBundleProviderDecorator(
            new AnyJSBundleProvider([
              new FileJSBundleProvider(this.rnAbility.context.filesDir + '/' + this.bundlePath)
            ]),
            this.rnohCoreContext!.logger),
        })
      } else {
        Row() {
          Text('未加载bundle')
          Text(this.bundleStatus)
        }
        .height(100)
      }
    }
    .hideTitleBar(true)
  }
}