/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import router from '@ohos.router';

@Entry
@Component
struct Index {
  @State message: string = '打开RN应用';

  build() {
    Row() {
      Column() {
        Button(this.message)
          .fontSize(20)
          .onClick(() => {
            router.pushUrl({ url: 'pages/RNPage' }, router.RouterMode.Standard, (err) => {
              if (err) {
                console.error(`Invoke pushUrl failed, code is ${err.code}, message is ${err.message}`);
                return;
              }
              console.info('Invoke pushUrl succeeded.');
            })
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}