/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  Descriptor,
  ViewBaseProps,
  RNViewBase,
  RNComponentContext,
} from '@rnoh/react-native-openharmony';


interface ContainerViewProps extends ViewBaseProps {
  size: number
}

export type ContainerViewDescriptor = Descriptor<"ContainerView", ContainerViewProps>

@Component
export struct ContainerView {
  public static readonly NAME = "ContainerView"
  public ctx!: RNComponentContext
  public tag: number = 0
  private currentOffset: Position = {
    x: 0,
    y: 0
  }
  private navigationPosition: Position = {
    x: 0,
    y: 0
  }

  @State descriptor: ContainerViewDescriptor = {} as ContainerViewDescriptor
  private cleanUpCallbacks: (() => void)[] = []

  getCurrentOffset(): Position {
    return this.currentOffset
  }

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<ContainerViewDescriptor>(this.tag)
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.descriptor = (newDescriptor as ContainerViewDescriptor)
      }
    ))
    this.ctx.setArkTSComponentDelegateForTag(this.tag, {
      'getCurrentOffset': this.getCurrentOffset.bind(this)
    });
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  build() {
    Navigation(){
      Stack() {
        RNViewBase({ tag: this.tag, ctx: this.ctx }) {
          ContentSlot(this.ctx.getContentForTag(this.tag))
        }
        Text("Custom Container").alignSelf(ItemAlign.Center)
      }
      .onAreaChange((oldValue, newValue) => {
        const origin = this.navigationPosition;
        this.currentOffset = {
          x: (origin.x as number) - (newValue.globalPosition.x as number),
          y: (origin.y as number) - (newValue.globalPosition.y as number),
        }
      })
    }
    .onAreaChange((oldValue, newValue) => {
      this.navigationPosition = newValue.globalPosition
    })
  }
}
