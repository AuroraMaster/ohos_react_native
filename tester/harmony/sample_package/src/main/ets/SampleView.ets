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
  RNComponentContext
} from '@rnoh/react-native-openharmony';


interface SampleViewProps extends ViewBaseProps {
  size: number
}

export type SampleViewDescriptor = Descriptor<"SampleView", SampleViewProps>

@Component
export struct SampleView {
  public static readonly NAME = "SampleView"
  public ctx!: RNComponentContext
  public tag: number = 0
  @State descriptor: SampleViewDescriptor = {} as SampleViewDescriptor
  @State fontSize: number | undefined = undefined
  private cleanUpCallbacks: (() => void)[] = []

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<SampleViewDescriptor>(this.tag)
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.descriptor = (newDescriptor as SampleViewDescriptor)
      }
    ))
    this.cleanUpCallbacks.push(this.ctx.componentCommandReceiver.registerCommandCallback(this.tag, (commandName) => {
      if (commandName === "toggleFontSize") {
        this.fontSize = this.fontSize ? undefined : 20
      }
    }))
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  build() {
    RNViewBase({ ctx: this.ctx, tag: this.tag }) {
      Text(this.descriptor.tag.toString()).fontColor("white").fontSize(this.fontSize ?? 12)
      this.ctx.wrappedRNChildrenBuilder.builder(this.ctx, this.tag)
    }
  }
}
