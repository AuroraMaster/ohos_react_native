/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNInfo, UPReactView, UPRNSDK } from 'library';

@Entry
@Component
export struct RNPage {
  rnInfo: RNInfo = { businessId: 'App', bundlePath: 'bundle.harmony.js', isFromMetro: false };
  @StorageLink('UPRNSDK') rnSdk: UPRNSDK | undefined = undefined;

  onBackPress(): boolean | void {
    this.rnSdk?.onBackPress();
  }

  build() {
    Stack() {
      UPReactView({
        rnInfo: this.rnInfo
      })
    }.width('100%').height('100%')
  }
}