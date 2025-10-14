/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModule } from "../../RNOH/TurboModule";

export class DeviceEventManagerTurboModule extends TurboModule {
  public static readonly NAME = 'DeviceEventManager';

  invokeDefaultBackPressHandler() {
    this.ctx.invokeDefaultBackPressHandler();
  }
}
