/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { UPRNSDK } from '../UPRNSDK';
import { createRNPackages } from '../rn/RNPackagesFactory';
import { RNInfo } from '../model/RNInfo';
import {
  RNInstance,
  ResourceJSBundleProvider,
  RNSurface,
  ComponentBuilderContext,
  RNAbility,
  RNOHCoreContext,
  RNComponentContext,
  RNOHContext,
  buildRNComponentForTag
} from "@rnoh/react-native-openharmony";

@Builder
export function buildCustomComponent(ctx: ComponentBuilderContext) {
}

@Entry
@Component
export struct UPReactView {
  rnInfo!: RNInfo;
  @StorageLink('UPRNSDK') rnSdk: UPRNSDK | undefined = undefined;
  private rnInstance!: RNInstance;
  @State isBundleReady: boolean = false;
  @StorageLink('RNAbility') private rnAbility: RNAbility = {} as RNAbility;
  private rnohCoreContext: RNOHCoreContext | undefined = AppStorage.get('RNOHCoreContext');

  @Builder
  public buildCustomComponent(ctx: ComponentBuilderContext) {
  }

  async aboutToAppear() {
    if (this.rnSdk) {
      this.rnInstance = await this.rnohCoreContext!.createAndRegisterRNInstance({
        createRNPackages: createRNPackages,
        enableNDKTextMeasuring: true,
        enableBackgroundExecutor: false,
        enableCAPIArchitecture: true,
        arkTsComponentNames: []
      });
      await this.rnInstance.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager,
        this.rnInfo.bundlePath)).then(() => {
        this.isBundleReady = true;
      })
    }
  }

  build() {
    Row() {
      Column() {
        if (this.isBundleReady) {
          RNSurface({
            ctx: new RNComponentContext(
              RNOHContext.fromCoreContext(this.rnohCoreContext!, this.rnInstance),
              wrapBuilder(buildCustomComponent),
              wrapBuilder(buildRNComponentForTag),
              new Map()
            ),
            surfaceConfig: {
              appKey: this.rnInfo.businessId,
            },
          })
        }
      }
      .width('100%')
    }
    .height('100%')
  }
}