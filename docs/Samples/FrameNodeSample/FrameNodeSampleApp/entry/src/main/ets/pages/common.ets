/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { UIContext } from '@kit.ArkUI';
import { NodeController, BuilderNode, Size, FrameNode } from '@kit.ArkUI';
import { BuilderData, Params } from './BuilderType';
import FrameNodeRNPage from './FrameNodeRNPage';

let NodeMap: Map<string, MyNodeController | undefined> = new Map();

export const createRNController = (instanceKey: string, uiContext: UIContext) => {
  let baseNode = new MyNodeController();
  baseNode.init(instanceKey, uiContext);
  NodeMap.set(instanceKey, baseNode);
}

export const getRNController = (instanceKey: string): MyNodeController | undefined => {
  return NodeMap.get(instanceKey);
}

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

  aboutToAppear(): void {
  }

  aboutToResize(size: Size): void {
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

  init(instanceKey: string, uiContext: UIContext) {
    if (this.rootNode != null) {
      return;
    }
    this.rootNode = new BuilderNode(uiContext);
    this.rootNode.build(wrapBuilder<BuilderData[]>(myRNPageBuilder), { text: 'This is a Text' });
  }
}
