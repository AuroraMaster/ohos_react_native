/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNInstance } from '@rnoh/react-native-openharmony';
import { BaseRN, LoadManager } from '../rn';
import { MetroBaseRN } from '../rn/MetroBaseRN';

@Component
export default struct Details {
  private instance: RNInstance = LoadManager.bpInstance;
  private bundlePath = 'bundle/bp/details.harmony.bundle';
  private moduleName = 'Details';
  @State name: string = "";
  @StorageLink('isMetroAvailable') isMetroAvailable: boolean = false;
  private container: Record<string, string | number> = {
    "backgroundColor": '#E6E6E6',
    "flex": 1,
    "padding": 20,
  }
  private apiFontSize: Record<string, string | number> = {
    'fontSize': 30,
    'color': 'white'
  }
  private styles: Record<string, object> = {
    'container': this.container,
    'apiFontSize': this.apiFontSize
  }
  private initProps: Record<string, string | object> = {
    // 'stringParam': 'ArkTS传递给RN的参数',
    // 'styles' : this.styles
  };

  aboutToAppear() {
    this.initProps = {
      'stringParam': 'ArkTS传递给RN的参数：' + this.name,
      'styles': this.styles
    };
  }

  build() {
    NavDestination() {
      Column() {
        Text(this.name)
          .fontSize(30)
          .fontWeight(30)
          .margin({ top: 10 })
        Row() {
          Text('紫色区域及文字ArkTS渲染')
            .fontSize(30)
            .fontColor('white')
            .fontWeight(20)
        }
        .height(100)
        .width('90%')
        .margin({ top: 10 })
        .backgroundColor('#a0a0ff')
        .borderRadius(10)

        if (this.isMetroAvailable) {
          MetroBaseRN({
            moduleName: this.moduleName,
            initProps: this.initProps as Record<string, string>
          }).align(Alignment.Top).margin({ top: 20 })
        } else if (this.instance) {
          BaseRN({
            rnInstance: this.instance,
            moduleName: this.moduleName,
            bundlePath: this.bundlePath,
            initProps: this.initProps as Record<string, string>
          }).align(Alignment.Top).margin({ top: 20 })
        }
      }.align(Alignment.Top).margin({ top: 40 })
    }
    .hideTitleBar(true)
  }
}