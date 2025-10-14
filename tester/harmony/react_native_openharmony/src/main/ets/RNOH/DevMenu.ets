/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import window from '@ohos.window';
import { DevToolsController } from './DevToolsController';
import { RNOHLogger } from './RNOHLogger';
import { ShakeDetector } from './ShakeDetector';
import { RNOHError } from "./RNOHError";
import type common from '@ohos.app.ability.common';
import { ComponentContent, PromptAction } from '@kit.ArkUI';
import SettingsPageDialog, { SettingsPageParams } from './SettingsPageDialog'
import {  DevMenu as DevMenuInterface } from './types'

interface AlertDialogButtonOptions {
  value: string,
  action: () => void,
}

enum DialogButtonDirection {
  AUTO, HORIZONTAL, VERTICAL,
}

class SettingDialog {
  private uiAbilityContext: common.UIAbilityContext;
  private logger: RNOHLogger;
  private contentNode: ComponentContent<Object> | undefined = undefined;
  private promptAction: PromptAction | undefined = undefined;
  private offsetY: number = 0;
  public visible: boolean = false;

  constructor(uiAbilityContext: common.UIAbilityContext, logger: RNOHLogger) {
    this.uiAbilityContext = uiAbilityContext;
    this.logger = logger.clone("Debug Server host & port for device dialog");
    this.createDevSettingDialog()
  }

  private createDevSettingDialog() {
    window.getLastWindow(this.uiAbilityContext).then((value) => {
      const uiContext = value.getUIContext()
      const avoidAreaType = window.AvoidAreaType.TYPE_SYSTEM;
      const avoidArea = value.getWindowAvoidArea(avoidAreaType);
      const avoidAreaHeight = avoidArea.topRect.height;
      this.offsetY = uiContext.px2vp(avoidAreaHeight)
      this.promptAction = uiContext.getPromptAction();
      this.contentNode = new ComponentContent(uiContext, wrapBuilder(SettingsPageDialog), new SettingsPageParams(this.uiAbilityContext, this.offsetY));
    }).catch(() => {
      this.logger.error("Debug Server host & port for device dialog couldn't be displayed.");
    })
  }

  public open() {
    this.promptAction?.openCustomDialog(this.contentNode, {
      isModal: true,
      onWillDismiss: this.close.bind(this),
      autoCancel: false,
      keyboardAvoidMode: KeyboardAvoidMode.NONE,
    });
    this.visible = true
  }

  public close() {
    this.promptAction?.closeCustomDialog(this.contentNode);
    this.visible = false
  }
}

/**
 * @api
 */
export class DevMenu {
  private devMenuDialogVisible: boolean = false;
  private devMenuButtons: AlertDialogButtonOptions[] = []
  private logger: RNOHLogger
  protected shakeDetector: ShakeDetector | null = null;
  private devToolsController: DevToolsController;
  private uiAbilityContext: common.UIAbilityContext;
  private settingDialog: SettingDialog | undefined = undefined;

  constructor(devToolsController: DevToolsController, uiAbilityContext: common.UIAbilityContext, logger: RNOHLogger) {
    this.devToolsController = devToolsController;
    this.uiAbilityContext = uiAbilityContext;
    this.logger = logger.clone("DevMenu");
    const localLogger = this.logger.clone("constructor")
    this.createDevMenuDefaultButtons();
    try {
      this.shakeDetector = ShakeDetector.from(this.show.bind(this))
    } catch (err) {
      if (err instanceof RNOHError) {
        localLogger.error(new RNOHError({
          whatHappened: "Shake to open Dev Menu is disabled. " + err.getMessage(),
          howCanItBeFixed: err.getSuggestions()
        }))
      } else {
        console.error(err)
      }
    }
  }

  addMenuItem(title: string): void {
    this.devMenuButtons.push({
      value: title,
      action: () => {
        this.devToolsController.emitDidPressMenuItem(title);
        this.hideDevMenuDialog();
      }
    })
  }

  private createDevMenuDefaultButtons() {
    this.devMenuButtons.push({
      value: "Reload",
      action: () => {
        this.devToolsController.reload(undefined)
        this.hideDevMenuDialog();
      },
    });
    this.devMenuButtons.push({
      value: "Toggle Element Inspector",
      action: () => {
        this.devToolsController.toggleElementInspector()
        this.hideDevMenuDialog();
      },
    });
    this.devMenuButtons.push({
      value: "Open Debugger",
      action: () => {
        this.devToolsController.openDebugger();
        this.hideDevMenuDialog();
      }
    });
    this.devMenuButtons.push({
      value: "Open React DevTools",
      action: () => {
        this.devToolsController.openDevTools();
        this.hideDevMenuDialog();
      }
    });
    this.devMenuButtons.push({
      value: "Settings",
      action: () => {
        this.settingDialog?.open();
        this.hideDevMenuDialog();
      }
    });
  }

  public show() {
    if (!this.devMenuDialogVisible && !this.settingDialog?.visible) {
      this.showDevMenuDialog();
    }
  }

  private showDevMenuDialog() {
    window.getLastWindow(this.uiAbilityContext).then((value) => {
      {
        const uiContext = value.getUIContext()
        const dialogParams: AlertDialogParamWithOptions = {
          title: "React Native Dev Menu",
          message: "",
          buttons: this.devMenuButtons,
          buttonDirection: DialogButtonDirection.VERTICAL,
          cancel: this.hideDevMenuDialog.bind(this),
        }
        uiContext.showAlertDialog(dialogParams)
        this.devMenuDialogVisible = true;
        this.devToolsController.emitDevMenuShown();
        this.settingDialog = new SettingDialog(this.uiAbilityContext, this.logger);
      }
    }).catch(() => {
      this.logger.error("DevMenu dialog couldn't be displayed.");
      this.hideDevMenuDialog();
    })
  }
  private hideDevMenuDialog() {
    this.devMenuDialogVisible = false;
    this.shakeDetector?.resetDetector();
  }
}

/**
 * @internal
 */
export class InternalDevMenu extends DevMenu implements DevMenuInterface {
  onDestroy() {
    this.shakeDetector?.onDestroy()
  }
}

export class NoopDevMenu implements DevMenuInterface {
  addMenuItem(title: string): void {
  }

  show(): void {
  }
}