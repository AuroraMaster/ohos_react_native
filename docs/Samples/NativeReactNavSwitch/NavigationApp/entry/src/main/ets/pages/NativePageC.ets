/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import router from '@ohos.router';

@Builder
export function NativePageCBuilder() {
  NativePageC()
}

@Component
struct NativePageC {
  @State message: string = '跳转到RN页面B';
  pathStack: NavPathStack | undefined;
  info: string = '原生页面C';

  build() {
    NavDestination() {
      Column() {
        Button('回到上一个页面', { stateEffect: true, type: ButtonType.Capsule })
          .width('80%')
          .height(40)
          .margin(20)
          .onClick(() => {
            this.pathStack?.pop();
          })
      }
      .width('100%')
      .height('100%')
    }
    .id('NativePageC')
    .title('原生页面C')
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}