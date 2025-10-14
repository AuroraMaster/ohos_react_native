/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { NavigationManager } from '../NavigationTool/NavigationManager';

@Entry
@Component
struct NativePageA {
  @State message: string = '跳转到RN页面B';
  info: string = '这是原生页面A';
  pathStack: NavPathStack = NavigationManager.getInstance().createNavPathStack();

  build() {
    Navigation(this.pathStack) {
      Column() {
        Button(this.message)
          .fontSize(20)
          .onClick(() => {
            console.log(`hyb_log NativePageA to RNPageB`);
            this.pathStack.pushDestinationByName("RNPageB", "RNPageB param");
          })
      }
      .width('100%')
      .height('100%')
    }
    .id('NativePageA')
    .title('原生页面A')
    .mode(NavigationMode.Stack)
  }
}