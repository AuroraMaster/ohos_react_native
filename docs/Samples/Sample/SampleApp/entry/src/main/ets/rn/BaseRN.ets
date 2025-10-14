/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNOHCoreContext,
  RNSurface,
  RNInstance,
  RNOHContext,
  RNComponentContext,
  buildRNComponentForTag
} from "@rnoh/react-native-openharmony";
import { LoadManager, buildCustomComponent } from './LoadBundle';

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomComponent);

@Component
export struct BaseRN {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined;
  useBundleCache: boolean = true;
  rnInstance!: RNInstance;
  moduleName: string = '';
  initProps: Record<string, string> = {};
  bundlePath: string = '';
  @State isBundleReady: boolean = false;

  aboutToAppear() {
    LoadManager.loadBundle(this.rnInstance, this.bundlePath, this.useBundleCache).then(() => {
      this.isBundleReady = true;
      console.log('加载完成' + this.bundlePath);
    }).catch((err: Error) => {
      console.log('加载失败' + this.bundlePath + err);
    });
  }

  build() {
    Column() {
      if (this.rnohCoreContext && this.isBundleReady) {
        RNSurface({
          surfaceConfig: {
            appKey: this.moduleName,
            initialProps: this.initProps,
          },
          ctx: new RNComponentContext(
            RNOHContext.fromCoreContext(this.rnohCoreContext!, this.rnInstance),
            wrappedCustomRNComponentBuilder,
            wrapBuilder(buildRNComponentForTag),
            new Map()
          ),
        })
      }
    }
    .height('100%')
    .width('100%')
  }
}
