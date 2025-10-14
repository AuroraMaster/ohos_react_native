/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { TurboModuleContext } from '../../RNOH/TurboModule';
import { TurboModule } from '../../RNOH/TurboModule';
import { DisplayMetrics } from '../../RNOH/types';
import { bundleManager } from '@kit.AbilityKit';

export class DeviceInfoTurboModule extends TurboModule {
  public static readonly NAME = 'DeviceInfo';

  static async create(ctx: TurboModuleContext) {
    const initialDisplayMetrics = ctx.getDisplayMetrics();
    return new DeviceInfoTurboModule(ctx, initialDisplayMetrics)
  }

  private displayMetrics?: DisplayMetrics = null;
  private cleanUpCallbacks?: (() => void)[] = []

  constructor(protected ctx: TurboModuleContext, initialDisplayMetrics: DisplayMetrics) {
    super(ctx);
    this.displayMetrics = initialDisplayMetrics;
    const updateDisplayMetrics = () => {
      this.displayMetrics = this.ctx.getDisplayMetrics();
      this.ctx.rnInstance.emitDeviceEvent("didUpdateDimensions", this.displayMetrics);
      this.ctx.rnInstance.postMessageToCpp("CONFIGURATION_UPDATE",this.displayMetrics);
    }
    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.subscribeToLifecycleEvents("CONFIGURATION_UPDATE", updateDisplayMetrics)
    )
    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.subscribeToLifecycleEvents("WINDOW_SIZE_CHANGE", updateDisplayMetrics)
    )
  }

  __onDestroy__() {
    super.__onDestroy__()
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  getHalfLeading(){
    let bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_METADATA |
    bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION;
    let mataDataArrary = bundleManager.getBundleInfoForSelfSync(bundleFlags).appInfo.metadataArray;
    let mataHalfLeading = "false";
    mataDataArrary.forEach(item => {
      item.metadata.forEach(matedata => {
        if(matedata.name == "half_leading"){
          mataHalfLeading = matedata.value;
          return;
        }
      });
    });
    return mataHalfLeading;
 }
  getConstants() {
    if (!this.displayMetrics) {
      this.ctx.logger.error("DeviceInfoTurboModule::getConstants: JS Display Metrics not set");
    }
    return {
      Dimensions: {
        windowPhysicalPixels: this.displayMetrics.windowPhysicalPixels,
        screenPhysicalPixels: this.displayMetrics.screenPhysicalPixels,
      }
    };
  }
}



