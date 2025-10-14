/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNInstance, RNComponentContext, CustomComponentBuilder } from "./RNOH"
import { RNSurface, SurfaceConfig2 } from "./RNSurface"

/**
 * @api: RN_APP_DEVELOPER
 */
@CustomDialog
export struct LogBoxDialog {
  public ctx!: RNComponentContext
  public controller: CustomDialogController
  public rnInstance!: RNInstance
  public initialProps: Record<string, string> = {}
  @BuilderParam public buildCustomComponent!: CustomComponentBuilder

  build() {
    RNSurface({
      ctx: this.ctx!,
      surfaceConfig: {
        initialProps: this.initialProps ?? {},
        appKey: "LogBox",
      } as SurfaceConfig2,
      buildCustomComponent: this.buildCustomComponent,
    })
  }
}
