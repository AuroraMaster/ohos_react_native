/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNOHCoreContext, ComponentBuilderContext, } from '@rnoh/react-native-openharmony';
import { NodeController, FrameNode, BuilderNode, Size } from '@ohos.arkui.node';
import { LoadManager } from '../rn';
import { promptAction } from '@kit.ArkUI';
import FrameNodeRNPage from './FrameNodeRNPage';
import { Params, BuilderData } from './BuilderType';
import { getRNController } from './common';

@Builder
function myRNPageBuilder(params: Params) {
  Stack() {
    Column() {
      FrameNodeRNPage(params)
        .width('100%')
        .height('30%')
    }
  }
}

class MyNodeController extends NodeController {
  private rootNode: BuilderNode<[Params]> | null = null;
  private isRemove: boolean = false;

  aboutToDisappear(): void {
    this.dispose();
  }

  dispose() {
    if (this.rootNode != null) {
      this.rootNode.dispose();
    }
  }

  setRemove(status: boolean) {
    this.isRemove = status;
  }

  makeNode(uiContext: UIContext): FrameNode | null {
    if (this.isRemove) {
      return null;
    }

    if (this.rootNode === null) {
      this.rootNode = new BuilderNode(uiContext);
      this.rootNode.build(wrapBuilder<BuilderData[]>(myRNPageBuilder), { text: 'This is a Text' });
    }

    return this.rootNode.getFrameNode();
  }

  rebuild(): void {
    super.rebuild();
  }

  update(message: string) {
    if (this.rootNode !== null) {
      this.rootNode.update({ text: message });
    }
  }
}

@Entry()
@Component
struct RNPage {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined;

  @Builder
  logBoxBuilder(_: ComponentBuilderContext) {
  }

  private nodeController: MyNodeController = new MyNodeController();
  @State hasRemoved: boolean = false;

  aboutToAppear() {
    console.log('TTXXX RNPage.ets aboutToAppear');

    setTimeout(() => {
      LoadManager.cpInstance.emitDeviceEvent('updatePage', { params: { title: 'Hello' } });
    }, 3000);
  }

  aboutToDisappear() {
    LoadManager.resetLoadedData();
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.SpaceEvenly }) {
      Text("This is a NodeContainer contains a list")
        .fontSize(16)
        .fontColor(0x0000FF)
      NodeContainer(getRNController('cp'))
        .borderWidth(1)
        .onClick(() => {
          promptAction.showToast({ message: 'NodeContainer click', duration: 1000 });
        })
    }
    .padding({ left: 20, right: 20, top: 20 })
    .margin({ top: 32 })
    .height(800)
    .width(400)
  }
}