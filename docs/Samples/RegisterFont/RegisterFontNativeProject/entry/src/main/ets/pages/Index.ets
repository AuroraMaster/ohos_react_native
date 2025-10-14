/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  AnyJSBundleProvider,
  ComponentBuilderContext,
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

  aboutToAppear() {
    this.logger = this.rnohCoreContext!.logger.clone("Index");
    const stopTracing = this.logger.clone("aboutToAppear").startTracing();
    this.shouldShow = true;
    stopTracing();
  }

  onBackPress(): boolean | undefined {
    // NOTE: this is required since `Ability`'s `onBackPressed` function always
    // terminates or puts the app in the background, but we want Ark to ignore it completely
    // when handled by RN
    this.rnohCoreContext!.dispatchBackPress();
    return true;
  }

  build() {
    Column() {
      if (this.rnohCoreContext && this.shouldShow) {
        if (this.rnohCoreContext?.isDebugModeEnabled) {
          RNOHErrorDialog({ ctx: this.rnohCoreContext })
        }
        if (this.rnAbility) {
          RNApp({
            rnInstanceConfig: {
              createRNPackages,
              enableCAPIArchitecture: true,
              arkTsComponentNames: [],
              fontResourceByFontFamily: {
                "StintUltraCondensed-Regular": $rawfile("fonts/StintUltraCondensed-Regular.ttf")
              }
            },
            appKey: "RegisterFont",
            wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
            onSetUp: (rnInstance) => {
              rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP");
              rnInstance.registerFont("Pacifico-Regular", $rawfile("fonts/Pacifico-Regular.ttf"));
            },
            jsBundleProvider: new TraceJSBundleProviderDecorator(
              new AnyJSBundleProvider([
                new MetroJSBundleProvider("http://localhost:8066/index.bundle?platform=harmony&dev=true&minify=false"),
                new ResourceJSBundleProvider(getContext().resourceManager, 'bundle/RegisterFont.harmony.bundle')
              ]),
              this.rnohCoreContext!.logger),
          })
        } else {
          Row() {
            Text('未加载bundle')
          }
          .height(100)
        }
      }
    }
    .height('100%')
    .width('100%')
  }
}
