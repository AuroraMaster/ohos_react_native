/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNComponentContext, RNViewBase } from '@rnoh/react-native-openharmony';
import { RNC } from "@rnoh/react-native-openharmony/generated"
import { getDirectProps } from "./GeneratedSampleViewUtils"

@Component
export struct GeneratedSampleView {
  public static readonly NAME = RNC.GeneratedSampleViewArkTS.NAME
  public ctx!: RNComponentContext
  public tag: number = 0
  @State private descriptorWrapper: RNC.GeneratedSampleViewArkTS.DescriptorWrapper = {
  } as RNC.GeneratedSampleViewArkTS.DescriptorWrapper
  private eventEmitter: RNC.GeneratedSampleViewArkTS.EventEmitter | undefined = undefined
  private cleanUpCallbacks: (() => void)[] = []

  aboutToAppear() {
    this.eventEmitter = new RNC.GeneratedSampleViewArkTS.EventEmitter(this.ctx.rnInstance, this.tag)
    this.onDescriptorWrapperChange(this.ctx.descriptorRegistry.findDescriptorWrapperByTag<RNC.GeneratedSampleViewArkTS.DescriptorWrapper>(this.tag)!)
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (_descriptor, newDescriptorWrapper) => {
        this.onDescriptorWrapperChange(newDescriptorWrapper! as RNC.GeneratedSampleViewArkTS.DescriptorWrapper)
      }
    ))

    this.cleanUpCallbacks.push(new RNC.GeneratedSampleViewArkTS.CommandReceiver(this.ctx.componentCommandReceiver, this.tag).subscribe("emitNativeEvent", (argv) => {
      const eventType = argv[0] as keyof RNC.GeneratedSampleViewArkTS.EventPayloadByName
      this.eventEmitter!.emit(eventType, this.getDirectProps())
    }))
  }

  private onDescriptorWrapperChange(descriptorWrapper: RNC.GeneratedSampleViewArkTS.DescriptorWrapper) {
    this.descriptorWrapper = descriptorWrapper
  }

  private getDirectProps() {
    return getDirectProps(this.descriptorWrapper)
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  build() {
    RNViewBase({ ctx: this.ctx, tag: this.tag }) {
      Column() {
        Text(JSON.stringify(this.getDirectProps(), null, 2)).width("100%").fontSize(8)
      }.width("100%").height("100%").backgroundColor(this.descriptorWrapper.props.colorTest.toRGBAString())

      this.ctx.wrappedRNChildrenBuilder.builder(this.ctx, this.tag)
    }
  }
}
