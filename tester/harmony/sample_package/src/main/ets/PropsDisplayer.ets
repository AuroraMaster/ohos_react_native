/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Descriptor, ViewDynamicProps, RNOHContext, RNViewBase } from '@rnoh/react-native-openharmony';

export type PropsDisplayerDescriptor = Descriptor<"PropsDisplayer", ViewDynamicProps>;

@Component
export struct PropsDisplayer {
  static NAME: string = "PropsDisplayer"
  ctx!: RNOHContext
  tag: number = 0
  private unregisterDescriptorChangesListener?: () => void = undefined
  @State private descriptor: PropsDisplayerDescriptor = {} as PropsDisplayerDescriptor

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<PropsDisplayerDescriptor>(this.tag)
    this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag, () => {
      this.descriptor = this.ctx.descriptorRegistry.getDescriptor<PropsDisplayerDescriptor>(this.tag)
    })
  }

  aboutToDisappear() {
    this.unregisterDescriptorChangesListener?.()
  }

  build() {
    RNViewBase({ ctx: this.ctx, tag: this.tag }) {
      Text(JSON.stringify(this.descriptor ?? {}, null, 2))
        .width("100%")
        .height("100%")
        .fontSize(12)
        .fontColor("#AAA")
    }
  }
}
