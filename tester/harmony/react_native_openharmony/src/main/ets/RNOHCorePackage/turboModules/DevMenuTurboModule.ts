/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { TurboModuleContext } from '../../RNOH/TurboModule';
import { TurboModule } from '../../RNOH/TurboModule';
import { RNOHLogger } from '../../RNOH/RNOHLogger';

export class DevMenuTurboModule extends TurboModule {
  public static readonly NAME = 'DevMenu';

  private logger: RNOHLogger;

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    this.logger = this.ctx.logger.clone("DevMenuTurboModule")
  }

  public show() {
    this.ctx.devMenu.show()
  }

  public reload() {
    this.ctx.devToolsController.reload(undefined)
  }

  public debugRemotely(enableDebug: boolean) {
    this.logger.warn("DevMenu::enableDebug is not supported");
  }

  public setProfilingEnabled(enabled: boolean) {
    this.logger.warn("DevMenu::setProfilingEnabled is not supported");
  }

  public setHotLoadingEnabled(enabled: boolean) {
    this.logger.warn("DevMenu::setHotLoadingEnabled is not supported");
  }
}