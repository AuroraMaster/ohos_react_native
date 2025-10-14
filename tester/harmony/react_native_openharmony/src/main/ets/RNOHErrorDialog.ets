/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Edges, RNOHError, FatalRNOHError, SafeAreaInsetsProvider, RNInstanceError } from "./RNOH"
import { RNOHCoreContext } from './RNOH/RNOHContext'
import { DevToolsController } from './RNOH/DevToolsController'

/**
 * @api: RN_APP_DEVELOPER
 */
@Component
export struct RNOHErrorDialog {
  public controller!: CustomDialogController
  public ctx!: RNOHCoreContext
  private isOpened: boolean = false
  private cleanUpListeners: (() => void)[] = []

  aboutToAppear(): void {
    this.controller = new CustomDialogController({
      cornerRadius: 0,
      customStyle: true,
      cancel: () => {
        this.isOpened = false
      },
      alignment: DialogAlignment.TopStart,
      backgroundColor: Color.Transparent,
      builder: RNOHErrorCustomDialog({
        devToolsController: this.ctx.devToolsController,
        safeAreaInsetsProvider: this.ctx.safeAreaInsetsProvider,
        onClose: () => {
          this.isOpened = false
        }
      })
    })
    this.cleanUpListeners.push(this.ctx.devToolsController.eventEmitter.subscribe(
      "NEW_ERROR",
      (_err) => {
        if (!this.isOpened) {
          this.controller.open()
          this.isOpened = true
        }
      }))
    this.cleanUpListeners.push(this.ctx.devToolsController.eventEmitter.subscribe(
      "DISMISS_RNOH_ERROR_DIALOG",
      () => {
        this.controller.close()
        this.isOpened = false
      }))
  }

  aboutToDisappear(): void {
    this.cleanUpListeners.forEach(cb => cb())
  }

  build() {
  }
}

const GUTTER = 16

@CustomDialog
struct RNOHErrorCustomDialog {
  public controller!: CustomDialogController
  public devToolsController!: DevToolsController
  public safeAreaInsetsProvider!: SafeAreaInsetsProvider
  public onClose: () => void = () => {
  }
  private cleanUpListeners: (() => void)[] = []
  @State private insets: Edges<number> = { top: 0, left: 0, right: 0, bottom: 0 }
  @State private lastError: RNOHError | null = null

  aboutToAppear(): void {
    const lastError = this.devToolsController.getLastError()
    if (lastError) {
      this.handleError(lastError)
    }
    this.cleanUpListeners.push(this.devToolsController.eventEmitter.subscribe(
      "NEW_ERROR",
      (err: RNOHError) => {
        this.handleError(err)
      }))
    this.insets = this.safeAreaInsetsProvider.safeAreaInsets
    this.cleanUpListeners.push(this.safeAreaInsetsProvider.eventEmitter.subscribe(
      "SAFE_AREA_INSETS_CHANGE",
      (insets) => {
        this.insets = insets
      }))
  }

  aboutToDisappear(): void {
    this.cleanUpListeners.forEach(cb => cb())
  }

  private handleError(err: RNOHError) {
    this.lastError = err
  }

  private getMessage(): string {
    return this.lastError?.getMessage() ?? ""
  }

  private getSuggestions(): string {
    const suggestions = this.lastError?.getSuggestions() ?? []
    if (suggestions.length === 1) {
      return suggestions[0]
    }
    return suggestions.map((s, idx) => `${idx + 1}) ${s}`).join("\n\n")
  }

  private getStack(): string {
    if (!this.lastError) {
      return ""
    }
    const stack = this.lastError.getStack()?.toString();
    if (!stack) {
      return ""
    }
    return stack.split("\n").map(s => s.trim()).join("\n\n").trim()
  }

  private getDetails(): string {
    if (!this.lastError) {
      return ""
    }
    const processData = (data: string | Error | object | null) => {
      if (!data) {
        return "";
      }
      if (typeof data === "string") {
        return data.trim()
      }
      if (data instanceof Error) {
        let lines = [`${data.name}: ${data.message}`]
        for (let stackEntry of (data.stack ?? "").split("\n")) {
          lines.push("")
          lines.push(stackEntry)
        }
        return lines.join("\n")
      }
      try {
        return JSON.stringify(data, null, 2)
      } catch (err) {
        return ""
      }
    }

    if (this.lastError instanceof RNInstanceError) {
      const errorData = this.lastError.getExtraData();
      let rnInstanceNameString = "";
      if (errorData.rnInstanceName) {
        rnInstanceNameString = `rnInstanceName="${errorData.rnInstanceName}", `;
      }
      let str = `(${rnInstanceNameString}rnInstanceId=${errorData.rnInstanceId})\n`
      str += processData(this.lastError.getExtraData().wrappedData);
      return str;
    } else {
      const errorData: object | null = this.lastError.getExtraData();
      return processData(errorData);
    }
  }

  @Builder
  buildButton(label: string, onClick: () => void) {
    Button(label)
      .onClick(() => {
        onClick()
        this.controller.close()
        this.onClose()
      })
      .backgroundColor("#1A1A1A")
      .enabled(!(this.lastError instanceof FatalRNOHError))
  }

  build() {
    Column() {
      Flex({ direction: FlexDirection.Column }) {
        Scroll() {
          Column() {
            if (this.getMessage()) {
              Text(this.getMessage())
                .width("100%")
                .fontColor("white")
                .backgroundColor("#C13130")
                .padding({
                  top: GUTTER,
                  left: this.insets.left + GUTTER,
                  right: this.insets.right + GUTTER,
                  bottom: GUTTER
                })
                .fontWeight("bold")
            }
            if (this.getSuggestions()) {
              Text((this.lastError?.getSuggestions() ?? []).length === 1 ? "Suggestion" : "Suggestions")
                .width("100%")
                .fontColor("white")
                .fontSize(12)
                .fontWeight("bold")
                .backgroundColor("#6D2525")
                .padding({ top: GUTTER, left: this.insets.left + GUTTER, right: this.insets.right + GUTTER })
              Text(this.getSuggestions())
                .width("100%")
                .fontColor("white")
                .fontSize(12)
                .backgroundColor("#6D2525")
                .padding({
                  top: GUTTER / 2,
                  left: this.insets.left + GUTTER,
                  right: this.insets.right + GUTTER,
                  bottom: GUTTER
                })
            }
            if (this.getStack()) {
              Text("Stack")
                .width("100%")
                .fontColor("#888")
                .fontSize(12)
                .fontWeight("bold")
                .padding({ top: GUTTER, left: this.insets.left + GUTTER, right: this.insets.right + GUTTER })
              Text(this.getStack())
                .width("100%")
                .fontColor("#AAA")
                .fontSize(12)
                .padding({
                  top: GUTTER / 2,
                  left: this.insets.left + GUTTER,
                  right: this.insets.right + GUTTER,
                  bottom: GUTTER
                })
            }
            if (this.getDetails()) {
              Text("Details")
                .width("100%")
                .fontColor("#888")
                .fontSize(12)
                .fontWeight("bold")
                .padding({ top: GUTTER, left: this.insets.left + GUTTER, right: this.insets.right + GUTTER })
              Text(this.getDetails())
                .width("100%")
                .fontColor("#AAA")
                .fontSize(12)
                .padding({ top: GUTTER / 2, left: this.insets.left + GUTTER, right: this.insets.right + GUTTER })
            }
          }.padding({ bottom: GUTTER })
        }
        .width("100%")
        .flexGrow(1)
        .align(Alignment.TopStart)
        .edgeEffect(EdgeEffect.Spring)

        Row() {
          this.buildButton("Dismiss", () => {
          })
          this.buildButton("Reload", () => {
            this.devToolsController.reload()
          })
        }
        .width("100%")
        .height(64)
        .justifyContent(FlexAlign.SpaceEvenly)
        .borderWidth({ top: 1 })
        .borderColor("#666")
      }.backgroundColor("#1A1A1A")
      .padding({ bottom: 64 })

    }
    .width("100%")
    .height("100%")
    .padding({ top: this.insets.top })
  }
}