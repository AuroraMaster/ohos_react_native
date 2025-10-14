/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import router from '@ohos.router';
import { ResourceJSBundleProvider, } from '@rnoh/react-native-openharmony';
import { RNInstanceManager } from "../rn/RNInstanceManager";

@Entry
@Component
struct Index {
  aboutToAppear() {
    RNInstanceManager.prepareRN('Hotel', new ResourceJSBundleProvider(
      getContext().resourceManager, 'bundle/basic/basic.harmony.bundle'))
    RNInstanceManager.prepareRN('Flight', new ResourceJSBundleProvider(
      getContext().resourceManager, 'bundle/basic/basic.harmony.bundle'))
  }

  build() {
    Column() {
      Button("前往酒店页面")
        .margin({
          bottom: 50
        })
        .onClick(() => {
          router.pushUrl({
            url: 'pages/Hotel'
          })
        })

      Button("前往机票页面")
        .onClick(() => {
          router.pushUrl({
            url: 'pages/Flight'
          })
        })
    }
    .justifyContent(FlexAlign.Center)
    .width('100%')
    .height('100%')
    .alignItems(HorizontalAlign.Center)
  }
}