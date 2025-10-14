/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNOHLogger } from '../../RNOH/RNOHLogger';
import type { TurboModuleContext } from '../../RNOH/TurboModule';
import { TurboModule } from '../../RNOH/TurboModule';

export class DevSettingsTurboModule extends TurboModule {
  public static readonly NAME = 'DevSettings';

  private logger: RNOHLogger

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    this.logger = this.ctx.logger.clone("DevSettingsTurboModule")
  }

  public reload() {
    this.ctx.logger.clone("reload").debug()
    this.ctx.devToolsController.reload(undefined)
  }

  public reloadWithReason(reason: string) {
    this.ctx.logger.clone("reloadWithReason").debug(reason)
    this.ctx.devToolsController.reload(reason)
  }

  public onFastRefresh() {
    this.logger.warn("DevSettings::onFastRefresh is not supported");
  }

  public setHotLoadingEnabled(isHotLoadingEnabled: boolean) {
    this.logger.warn("DevSettings::setHotLoadingEnabled is not supported");
  }

  public setIsDebuggingRemotely(isDebuggingRemotelyEnabled: boolean) {
    this.logger.warn("DevSettings::setIsDebuggingRemotely is not supported");
  }

  public setProfilingEnabled(isProfilingEnabled: boolean) {
    this.logger.warn("DevSettings::setProfilingEnabled is not supported");
  }

  public toggleElementInspector() {
    this.ctx.rnInstance.emitDeviceEvent("toggleElementInspector", {});
  }

  public addMenuItem(title: string) {
    this.ctx.devMenu.addMenuItem(title);
  }

  public addListener(eventName: string) {
    this.logger.warn("DevSettings::addListener is not supported");
  }

  public removeListeners(count: number) {
    this.logger.warn("DevSettings::removeListeners is not supported");
  }

  public setIsShakeToShowDevMenuEnabled(enabled: boolean) {
    this.logger.warn("DevSettings::setIsShakeToShowDevMenuEnabled is not supported");
  }
}