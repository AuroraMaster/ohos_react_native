/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import resourceManager from '@ohos.resourceManager';
import { BusinessError } from '@ohos.base';
import { TurboModule, TurboModuleContext } from '../../RNOH/TurboModule';
import { convertColorValueToRGBA } from '../../RNOH/CppBridgeUtils';

export class PlatformColorTurboModule extends TurboModule {
  public static readonly NAME = 'PlatformColor';

  constructor(ctx: TurboModuleContext) {
    super(ctx);
  }

  public getSystemColor(colorKeys: string[]) {
    for (let colorKey of colorKeys) {
      const systemColor = this.getColorFromResources(colorKey);
      if (systemColor) {
        return systemColor;
      }
    }

    return null;
  }

  private getColorFromResources(colorKey: string) {
    let systemColor = null;

    // under the hood getColorByNameSync will throw exception if colorKey doesn't exist
    try {
      if (colorKey.startsWith('ohos_id')) {
        systemColor = resourceManager.getSystemResourceManager().getColorByNameSync(`${colorKey}`);
      } else {
        systemColor = this.ctx.uiAbilityContext.resourceManager.getColorByNameSync(`${colorKey}`);
      }
    } catch (error) {
      let code = (error as BusinessError).code;
      let message = (error as BusinessError).message;
      this.ctx.logger.clone(["PlatformColorTurboModule", "getColorFromResources"])
        .warn(
          `systemResourceManager getColorByName ${colorKey} failed, error code: ${code}, message: ${message}.`
        );
    }

    return systemColor ? convertColorValueToRGBA(systemColor) : systemColor;
  }
}
