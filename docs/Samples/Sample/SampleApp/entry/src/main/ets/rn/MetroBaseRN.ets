/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNSurface,
  RNComponentContext,
  RNOHCoreContext,
  RNOHContext,
  buildRNComponentForTag
} from "@rnoh/react-native-openharmony";
import { LoadManager, buildCustomComponent } from './LoadBundle';

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomComponent);

@Component
export struct MetroBaseRN {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined;
  moduleName: string = '';
  initProps: Record<string, string> = {};

  build() {
    Column() {
      if (this.rnohCoreContext) {
        RNSurface({
          surfaceConfig: {
            appKey: this.moduleName,
            initialProps: this.initProps,
          },
          ctx: new RNComponentContext(
            RNOHContext.fromCoreContext(this.rnohCoreContext!, LoadManager.metroInstance),
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

