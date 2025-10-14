/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { TurboModuleContext } from '../../RNOH/TurboModule';
import { TurboModule } from '../../RNOH/TurboModule';
import window from '@ohos.window';

type AlertOptions = {
  title: string
  message?: string
  primaryButton?: string
  secondaryButton?: string
  tertiaryButton?: string
  cancelable: boolean
}

export class AlertManagerTurboModule extends TurboModule {
  public static readonly NAME = 'AlertManager';

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);

  }

  private constants = {
    buttonClicked: 'buttonClicked',
    dismissed: 'dismissed',
    primaryButton: 1,
    secondaryButton: 2,
    tertiaryButton: 3,
  }

  getConstants() {
    return this.constants;
  }

  alert(options: AlertOptions, onError: (error: string) => void, onAction: (action: string, buttonKey?: number) => void) {
    this.ctx.rnInstance.getRNWindow().then((value) => {
      {
        const uiContext = value.getUIContext()

        const primaryButton = 
        options.primaryButton? {
          value: options.primaryButton,
          action: () => {
            onAction?.(this.constants.buttonClicked, this.constants.primaryButton);
            onAction = undefined;
          }
        } : undefined;
        const secondaryButton = 
        options.secondaryButton? {
          value: options.secondaryButton,
          action: () => {
            onAction?.(this.constants.buttonClicked, this.constants.secondaryButton);
            onAction = undefined;
          }
        } : undefined;
        const tertiaryButton =
        options.tertiaryButton? {
          value: options.tertiaryButton,
          action: () => {
            onAction?.(this.constants.buttonClicked, this.constants.tertiaryButton);
            onAction = undefined;
          }
        } : undefined;
        const buttons = [primaryButton, secondaryButton, tertiaryButton];

        const alertParams = {
          title: options.title,
          message: options.message,
          autoCancel: options.cancelable,
          buttons: buttons.filter((button) => button !== undefined),
          cancel: () => {
            onAction?.(this.constants.dismissed);
            onAction = undefined;
          },
        }
        uiContext.showAlertDialog(alertParams)
      }
    }).catch(() => onError("Alert dialog couldn't be displayed."))
  }
}