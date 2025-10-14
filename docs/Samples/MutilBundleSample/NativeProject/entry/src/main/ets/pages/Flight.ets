/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNApp,
  RNOHErrorDialog,
  RNOHLogger,
  RNOHCoreContext,
  RNInstance,
  ResourceJSBundleProvider
} from '@rnoh/react-native-openharmony';
import { RNInstanceManager } from "../rn/RNInstanceManager";
import { buildCustomRNComponent } from "../rn/CustomRNComponent";

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomRNComponent);

@Entry
@Component
struct Flight {
  @StorageLink('RNOHCoreContext') private rnohCoreContext: RNOHCoreContext | undefined = undefined;
  @State shouldShow: boolean = false;
  private logger!: RNOHLogger;
  private rnInstance: RNInstance | undefined;

  async aboutToAppear() {
    this.logger = this.rnohCoreContext!.logger.clone("Flight");
    const stopTracing = this.logger.clone("aboutToAppear").startTracing();
    this.rnInstance = await RNInstanceManager.getInstance("Flight");
    this.shouldShow = true;
    stopTracing();
  }

  onBackPress(): boolean | undefined {
    this.rnohCoreContext!.dispatchBackPress();
    return true;
  }

  build() {
    Column() {
      if (this.rnohCoreContext && this.shouldShow) {
        if (this.rnohCoreContext?.isDebugModeEnabled) {
          RNOHErrorDialog({ ctx: this.rnohCoreContext });
        }
        RNApp({
          rnInstanceConfig: {
            rnInstance: this.rnInstance as RNInstance,
          },
          initialProps: { "foo": "bar" } as Record<string, string>,
          appKey: "flight",
          wrappedCustomRNComponentBuilder: wrappedCustomRNComponentBuilder,
          onSetUp: (rnInstance) => {
            rnInstance.enableFeatureFlag("ENABLE_RN_INSTANCE_CLEAN_UP");
          },
          jsBundleProvider: new ResourceJSBundleProvider(
            this.rnohCoreContext.uiAbilityContext.resourceManager, 'bundle/cp/flight.harmony.bundle')
        })
      }
    }
    .height('100%')
    .width('100%')
  }
}
