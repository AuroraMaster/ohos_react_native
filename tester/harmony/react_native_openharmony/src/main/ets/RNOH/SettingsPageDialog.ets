/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { preferences } from '@kit.ArkData'
import { BusinessError } from '@kit.BasicServicesKit';
import type common from '@ohos.app.ability.common';

export class SettingsPageParams {
  public uiAbilityContext: common.UIAbilityContext;
  public offsetY: number;

  constructor(uiAbilityContext: common.UIAbilityContext, offsetY: number) {
    this.uiAbilityContext = uiAbilityContext
    this.offsetY = offsetY
  }
}

@Component
@CustomDialog
struct DevSetHostAndPortContent {
  @State address: string = ''
  uiAbilityContext: common.UIAbilityContext | null = null
  settingsPreferences: preferences.Preferences | null = null
  controller?: CustomDialogController
  cancel: () => void = () => {
    this.controller?.close()
  }
  confirm: () => void = () => {
    if (this.settingsPreferences !== null) {
      const putPromise = this.settingsPreferences.put('devHostAndPortAddress', this.address)
      putPromise.then(() => {
        this.settingsPreferences?.flush((err: BusinessError) => {
          if (err) {
            return;
          }
        })
      }).catch((err: BusinessError) => {
        console.error("Failed to put value of 'devHostAndPortAddress'. code =" + err.code + ", message =" + err.message);
      }).finally(() => {
        this.cancel()
      })
    }
  }
  aboutToAppear() {
    this.settingsPreferences?.get('devHostAndPortAddress', '', (err: BusinessError, val: preferences.ValueType) => {
      if (!err) {
        this.address = val.toString()
      }
    })
  }
  build() {
    Column() {
      Text('Debug Server host & port for device')
        .width('100%').fontSize(20).fontWeight(FontWeight.Bold).margin({ top: 10, bottom: 10 })
      TextInput({ text: this.address })
        .showUnderline(true).margin({ bottom: 20 })
        .onChange((value: string) => {
          this.address = value
        })
      Flex({ justifyContent: FlexAlign.End }) {
        Text('Cancel')
          .fontWeight(FontWeight.Bold)
          .onClick(this.cancel)
        Text('Confirm')
          .fontWeight(FontWeight.Bold).margin({ left: 16 })
          .onClick(this.confirm)
      }.margin({ top: 10, bottom: 10 })
    }.width('94%').backgroundColor(Color.White).borderRadius(4).padding(10).margin({ bottom: -16 })
  }
}

@Component
struct SettingsClickItem {
  title: string = '';
  description: string = '';
  click: () => void = () => {};
  private clickBackgroundColor: ResourceColor = Color.White;
  @Styles normalStyle() {
    .backgroundColor(Color.White)
  }
  @Styles pressedStyle() {
    .backgroundColor('rgba(224,224,224, 0.5)')
  }
  build() {
    Flex({ direction: FlexDirection.Column }) {
      Column() {
        Text(this.title)
          .width('100%').fontWeight(FontWeight.Bold).textAlign(TextAlign.Start).fontSize(20).margin({ bottom: 5 })
        Text(this.description)
          .width('100%').textAlign(TextAlign.Start).fontColor(Color.Grey).margin({ bottom: 10 })
      }
      .backgroundColor(this.clickBackgroundColor).padding(12).borderWidth({ left: 0, right: 0, top: 0, bottom: 1 }).borderColor(Color.Gray)
      .stateStyles({
        normal: this.normalStyle,
        pressed: this.pressedStyle,
      })
      .onClick(() => {
        this.click()
      })
    }
  }
}

@Component
struct SettingsPageDialog {
  uiAbilityContext: common.UIAbilityContext | null = null
  settingsPreferences : preferences.Preferences | null = null
  offsetY: number = 0
  setHostAndPortDialogController: CustomDialogController | null = new CustomDialogController({
    builder: DevSetHostAndPortContent({ uiAbilityContext: this.uiAbilityContext, settingsPreferences: this.settingsPreferences }),
    cancel: ()=> {},
    autoCancel: false,
    customStyle: true,
  })

  aboutToAppear() {
    preferences.getPreferences(this.uiAbilityContext, "devSettings", (err: BusinessError, val: preferences.Preferences) => {
      if (!err) {
        this.settingsPreferences = val
      }
    })
  }
  aboutToDisappear() {
    this.setHostAndPortDialogController = null
  }
  build() {
    Column() {
      Column() {
        Text('Debugging')
          .width('100%').fontWeight(FontWeight.Bold).textAlign(TextAlign.Start).fontColor(0X32cd32).fontSize(18).padding(12)
        SettingsClickItem({
          title: 'Debug Server host & port for device',
          description: 'Debug server host & port for downloading JS bundle or communicating with JS debugger. With this setting empty launcher should work fine when running on emulator (or genymotion) and connection to debug server running on emulator\'s host.',
          click:() => {
            if (this.setHostAndPortDialogController != null) {
              this.setHostAndPortDialogController.open()
            }
          }
        })
      }
    }.backgroundColor(Color.White).width('100%').padding({ top: this.offsetY })
  }
}

@Builder
function SettingsPageDialogBuilder(Params: SettingsPageParams) {
  SettingsPageDialog({ uiAbilityContext: Params.uiAbilityContext, offsetY: Params.offsetY })
}

export default SettingsPageDialogBuilder