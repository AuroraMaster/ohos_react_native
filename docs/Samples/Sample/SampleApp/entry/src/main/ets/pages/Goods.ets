/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNInstance } from '@rnoh/react-native-openharmony';
import { BaseRN, LoadManager } from '../rn';
import { MetroBaseRN } from '../rn/MetroBaseRN'
import emitter from '@ohos.events.emitter';
import Details from "./Details";
import PrecreateRN from './PrecreateRN';
import { MultiSurface } from './MultiSurface';
import SandBoxApp from './SandboxBundle';

@Component
export default struct Goods {
  private instance: RNInstance = LoadManager.cpInstance;
  private bundlePath = 'bundle/cp/goods.harmony.bundle';
  private moduleName = 'Goods';
  @StorageLink('isMetroAvailable') isMetroAvailable: boolean = false;
  @Consume('pagePathsGoods') pagePathsGoods: NavPathStack;

  @Builder
  PageMap(name: string, param: string) {
    if (name === 'Details') {
      Details({ name: param });
    } else if (name === 'PrecreateRN') {
      PrecreateRN();
    } else if (name === 'MultiSurface') {
      MultiSurface();
    } else if (name === 'Sandbox') {
      SandBoxApp();
    } else {
      Text('无效页面');
    }
  }

  aboutToAppear() {
    emitter.on({ eventId: 1 }, (eventData) => {
      if (eventData.data) {
        console.log('[TurboModule] param = ' + eventData.data['param']);
        const path: string = eventData.data['param'];
        if (path === "pages/Details") {
          this.pagePathsGoods.pushPath({ name: "Details", param: "details1" });
        } else if (path === "pages/Details2") {
          this.pagePathsGoods.pushPath({ name: "Details", param: "detail2" });
        } else if (path === "pages/DetailsMultiInstance") {
          this.pagePathsGoods.pushPath({ name: "DetailsMultiInstance", param: "MultiInstance" });
        } else if (path === "pages/PrecreateRN") {
          this.pagePathsGoods.pushPath({ name: "PrecreateRN" });
        } else if (path === "pages/MultiSurface") {
          this.pagePathsGoods.pushPath({ name: "MultiSurface" });
        } else if (path === "pages/Sandbox") {
          this.pagePathsGoods.pushPath({ name: "Sandbox" });
        }
        // this.pagePathsGoods.pushPath({ name: "DetailsMultiInstance", param: "MultiInstance" })
      }
    });
  }

  aboutToDisappear() {
    emitter.off(1);
  }

  build() {
    Navigation(this.pagePathsGoods) {
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
    .navDestination(this.PageMap)
    .hideTitleBar(true)
    .hideToolBar(true)
  }
}