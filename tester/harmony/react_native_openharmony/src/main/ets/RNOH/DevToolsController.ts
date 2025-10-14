/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { EventEmitter } from './EventEmitter'
import { DevServerHelper } from './DevServerHelper'
import { RNOHLogger } from './RNOHLogger'
import { RNOHError } from './RNOHError'

interface DevMenuItem {
  title: string
}

/**
 * @api
 */
export class DevToolsController {
  public eventEmitter = new EventEmitter<{
    "RELOAD": [{ reason: string | undefined }],
    "DISMISS_RNOH_ERROR_DIALOG",
    "SHOW_DEV_LOADING_VIEW": [
      rnInstanceId: number,
      message: string,
      color?: number,
      backgroundColor?: number
    ],
    "HIDE_DEV_LOADING_VIEW": [rnInstanceId: number,
    ],
    "NEW_ERROR": [RNOHError],
    "TOGGLE_ELEMENT_INSPECTOR",
    "DEV_MENU_SHOWN",
    "DID_PRESS_MENU_ITEM": [item: DevMenuItem],
    "OPEN_URL": [url: string, onError: () => void],
  }>()

  private logger: RNOHLogger;
  protected lastError: RNOHError | null = null

  constructor(logger: RNOHLogger) {
    this.logger = logger.clone("DevMenu");
  }

  reload(reason: string | undefined = undefined): void {
    // Disable debugger to resume the JsVM & avoid thread locks while reloading
    DevServerHelper.disableDebugger();
    this.eventEmitter.emit("RELOAD", { reason })
  }

  toggleElementInspector(): void {
    this.eventEmitter.emit("TOGGLE_ELEMENT_INSPECTOR");
  }

  emitDevMenuShown(): void {
    this.eventEmitter.emit("DEV_MENU_SHOWN");
  }

  emitDidPressMenuItem(title: string): void {
    this.eventEmitter.emit("DID_PRESS_MENU_ITEM", { title })
  }

  dismissRNOHErrorDialog(): void {
    this.eventEmitter.emit("DISMISS_RNOH_ERROR_DIALOG")
  }

  openDebugger(): void {
    this.eventEmitter.emit(
      "OPEN_URL",
      "flipper://null/Hermesdebuggerrn?device=React%20Native",
      () => this.logger.error("Failed to open Flipper. Please check that Metro is running.")
    )
  }

  openDevTools(): void {
    this.eventEmitter.emit(
      "OPEN_URL",
      "flipper://null/React?device=React%20Native",
      () => this.logger.error("Failed to open Flipper. Please check that Metro is running.")
    )
  }

  getLastError(): RNOHError | null {
    return this.lastError
  }

  /**
   * @internal
   */
  setLastError(error: RNOHError): void {
    this.lastError = error;
  }
}