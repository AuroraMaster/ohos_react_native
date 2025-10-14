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
import { SurfaceConfig2 } from '@rnoh/react-native-openharmony/src/main/ets/RNSurface';
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
    }).catch((err: Error) => {
      console.log('BaseRN.ets aboutToAppear 加载失败');
    })
  }

  build() {
    Column() {
      if (this.rnohCoreContext && this.isBundleReady) {
        RNSurface({
          surfaceConfig: {
            appKey: this.moduleName,
            initialProps: { "propA": "good" } as Record<string, string>,
          } as SurfaceConfig2,
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
