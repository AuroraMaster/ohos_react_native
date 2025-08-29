/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { UITurboModuleContext, SafeAreaInsets } from "../../RNOH/ts";
import { UITurboModule } from "../../RNOH/ts";
import { StatusBarTurboModule } from "./StatusBarTurboModule"


export interface StatusBarStatusProvider {
  isStatusBarHidden(): boolean
}

export class SafeAreaTurboModule extends UITurboModule {
  public static readonly NAME = 'SafeAreaTurboModule';

  private cleanUpCallbacks: (() => void)[] = [];
  private initialInsets: SafeAreaInsets

  static async create(ctx: UITurboModuleContext, statusBarTurboModule: StatusBarTurboModule) {
    return new SafeAreaTurboModule(ctx, statusBarTurboModule)
  }

  constructor(ctx: UITurboModuleContext, statusBarTurboModule: StatusBarTurboModule) {
    super(ctx)
    this.initialInsets = ctx.safeAreaInsetsProvider.safeAreaInsets;
    this.cleanUpCallbacks.push(ctx.safeAreaInsetsProvider.eventEmitter.subscribe("SAFE_AREA_INSETS_CHANGE", this.onSafeAreaChange.bind(this)));
    // Hiding/Showing StatusBar is reflected immediately in SafeAreaView
    this.cleanUpCallbacks.push(statusBarTurboModule.subscribe("SYSTEM_BAR_VISIBILITY_CHANGE", () => ctx.safeAreaInsetsProvider.onSafeAreaChange()));
  }

  private onSafeAreaChange(insets: SafeAreaInsets) {
    this.ctx.rnInstance.emitDeviceEvent("SAFE_AREA_INSETS_CHANGE", insets);
  }

  public getInitialInsets(): SafeAreaInsets {
    return this.initialInsets
  }

  __onDestroy__() {
    super.__onDestroy__();
    this.cleanUpCallbacks.forEach(cb => cb());
    this.cleanUpCallbacks = [];
  }
}
