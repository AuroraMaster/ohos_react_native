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
export default struct Test {
  private instance: RNInstance = LoadManager.cpInstance;
  private bundlePath = 'bundle/bp/test.harmony.bundle';
  private moduleName = 'AppTest';
  @StorageLink('isMetroAvailable') isMetroAvailable: boolean = false;
  @Consume('pagePathsTest') pagePathsTest: NavPathStack;

  build() {
    Navigation(this.pagePathsTest) {
      if (this.isMetroAvailable) {
        MetroBaseRN({
          moduleName: this.moduleName,
        }).align(Alignment.Top).margin({ top: 40 })
      } else if (this.instance) {
        BaseRN({
          rnInstance: this.instance,
          moduleName: this.moduleName,
          bundlePath: this.bundlePath,
        }).align(Alignment.Top).margin({ top: 40 })
      }
    }
    .hideTitleBar(true)
    .hideToolBar(true)
  }
}