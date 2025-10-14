/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModule, EventEmitter } from "../../RNOH/ts"


export class LogBoxTurboModule extends TurboModule {
  static NAME: Readonly<string> = "LogBox"

  public eventEmitter = new EventEmitter<{
    "SHOW": [],
    "HIDE": []
  }>()

  public show() {
    this.eventEmitter.emit("SHOW")
  }

  public hide() {
    this.eventEmitter.emit("HIDE")
  }
}