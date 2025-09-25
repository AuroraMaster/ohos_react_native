/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
import {
  Descriptor,
  ViewBaseProps,
  RNViewBase,
  RNComponentContext,
} from '@rnoh/react-native-openharmony';

interface BindSheetViewProps extends ViewBaseProps {}

export type BindSheetViewDescriptor = Descriptor<"BindSheetView", BindSheetViewProps>

@Component
export struct BindSheetView {
  public static readonly NAME = "BindSheetView"
  public ctx!: RNComponentContext
  public tag: number = 0
  @State showSheet: boolean = false;

  @State descriptor: BindSheetViewDescriptor = {} as BindSheetViewDescriptor
  private cleanUpCallbacks: (() => void)[] = []

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<BindSheetViewDescriptor>(this.tag)
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.descriptor = (newDescriptor as BindSheetViewDescriptor)
      }
    ))
    this.showSheet = true;
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cb => cb())
    this.showSheet = false;
  }

  @Builder
  myBuilder() {
    Column(){
      ContentSlot(this.ctx.getContentForTag(this.tag))
    }
    .width('100%')
    .height('100%')
    .onTouch((event: TouchEvent) => {this.ctx.rnInstance.postTouchEventMessageToCpp(this.tag, event)})
  }

  build() {
    RNViewBase({ tag: this.tag, ctx: this.ctx }) {
      Stack(){}.bindSheet(this.showSheet, this.myBuilder())
    }
  }
}
