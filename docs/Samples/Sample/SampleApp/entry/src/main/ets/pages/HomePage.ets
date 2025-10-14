/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNInstance } from '@rnoh/react-native-openharmony';
import { BaseRN } from '../rn/BaseRN';
import { LoadManager } from '../rn/LoadBundle';
import { MetroBaseRN } from '../rn/MetroBaseRN';

@Component
export default struct HomePage {
  private instance: RNInstance = LoadManager.cpInstance;
  private bundlePath = 'bundle/cp/homepage.harmony.bundle';
  private moduleName = 'HomePage';
  @StorageLink('isMetroAvailable') isMetroAvailable: boolean = false;
  @Consume('pagePathsHome') pagePathsHome: NavPathStack;

  @Builder
  PageMap(name: string) {
    Text("Empty View");
  }

  build() {
    Navigation(this.pagePathsHome) {
      if (this.isMetroAvailable) {
        MetroBaseRN({
          moduleName: this.moduleName,
        }).align(Alignment.Top).margin({ top: 0 })
      } else if (this.instance) {
        BaseRN({
          rnInstance: this.instance,
          moduleName: this.moduleName,
          bundlePath: this.bundlePath,
        }).align(Alignment.Top).margin({ top: 0 })
      }
    }
    .hideTitleBar(true)
    .hideToolBar(true)
    .navDestination(this.PageMap)
  }
}
