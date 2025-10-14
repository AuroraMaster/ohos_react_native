/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import HomePage from './HomePage';
import Goods from './Goods';
import Test from './Test';

@Component
export default struct MultiHome {
  @Provide('tabBarHeight') barHeight: number = 64;
  @Provide('pagePathsHome') @Watch('onPathsUpdate') pagePathsHome: NavPathStack = new NavPathStack();
  @Provide('pagePathsGoods') @Watch('onPathsUpdate') pagePathsGoods: NavPathStack = new NavPathStack();
  @Provide('pagePathsTest') @Watch('onPathsUpdate') pagePathsTest: NavPathStack = new NavPathStack();
  @State fontColor: string = '#182431';
  @State selectedFontColor: string = '#1980E6';
  @State currentIndex: number = 0;
  private controller: TabsController = new TabsController();

  onPathsUpdate(propName: string) {
    if (propName === 'pagePathsGoods') {
      const paths: Array<string> = this.pagePathsGoods.getAllPathName();
      animateTo({ duration: 300 }, () => {
        if (paths.length == 0) {
          this.barHeight = 64;
        } else {
          this.barHeight = 0;
        }
      })
    }
  }

  @Builder
  TabBuilder(index: number, name: string) {
    Column() {
      Text(name)
        .fontColor(this.currentIndex === index ? this.selectedFontColor : this.fontColor)
        .fontSize(16)
        .fontWeight(this.currentIndex === index ? 500 : 400)
        .lineHeight(22)
        .margin({ top: 12, bottom: 7 })
      Divider()
        .strokeWidth(2)
        .color(this.selectedFontColor)
        .opacity(this.currentIndex === index ? 1 : 0)
        .width(50)
    }.width('100%')
  }

  build() {
    Column() {
      Tabs({ barPosition: BarPosition.End }) {
        TabContent() {
          HomePage();
        }
        .tabBar(this.TabBuilder(0, '首页'))

        TabContent() {
          Goods();
        }
        .tabBar(this.TabBuilder(1, "商品"))

        TabContent() {
          Test();
        }
        .tabBar(this.TabBuilder(2, "测试用例"))
      }
      .barHeight(this.barHeight)
      .scrollable(false)
      .onChange((index: number) => {
        this.currentIndex = index;
      })
    }
  }
}
