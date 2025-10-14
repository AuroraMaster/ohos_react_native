/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNOHCoreContext } from './RNOHContext'
import { SafeAreaInsets } from "./SafeAreaInsetsProvider"

@Component
export struct RNDevLoadingView {
  public useSafeAreaInsets: boolean = false
  public ctx!: RNOHCoreContext
  private cleanUpCallbacks: (() => void)[] = []
  @State private message: string = ""
  @State private safeAreaInsets: SafeAreaInsets | undefined = undefined

  aboutToAppear() {
    if (this.ctx.safeAreaInsetsProvider !== undefined) {
      this.safeAreaInsets = this.ctx.safeAreaInsetsProvider.safeAreaInsets
      this.cleanUpCallbacks.push(
        this.ctx.safeAreaInsetsProvider.eventEmitter.subscribe(
          "SAFE_AREA_INSETS_CHANGE",
          (insets) => {
            this.safeAreaInsets = insets
          }))
    } else {
      this.safeAreaInsets = { top: 0, bottom: 0, left: 0, right: 0 }
    }
    this.cleanUpCallbacks.push(
      this.ctx.devToolsController.eventEmitter.subscribe(
        "SHOW_DEV_LOADING_VIEW",
        (_rnInstanceId, msg) => {
          this.message = msg
        }))
    this.cleanUpCallbacks.push(
      this.ctx.devToolsController.eventEmitter.subscribe(
        "HIDE_DEV_LOADING_VIEW",
        () => {
          this.message = ""
        }))
  }

  aboutToDisappear(): void {
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  build() {
    if (this.safeAreaInsets && this.message) {
      Text(this.message)
        .fontColor("white")
        .fontSize(12)
        .backgroundColor("rgba(0,0,0,0.6)")
        .width("100%")
        .padding({ top: 8, bottom: 8 })
        .position({ x: 0, y: this.useSafeAreaInsets ? this.safeAreaInsets.top : 0 })
        .textAlign(TextAlign.Center)
    }

  }
}
