/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Descriptor, ViewBaseProps } from '@rnoh/react-native-openharmony';
import { RNOHContext, RNViewBase } from '@rnoh/react-native-openharmony';

export interface MarqueeViewProps extends ViewBaseProps {
  src: string;
}

export type MarqueeViewDescriptor = Descriptor<"MarqueeView", MarqueeViewProps>;

@Component
export struct MarqueeView {
  static NAME: string = "MarqueeView";
  ctx!: RNOHContext;
  tag: number = 0;
  private unregisterDescriptorChangesListener?: () => void = undefined;
  @State private descriptor: MarqueeViewDescriptor = {} as MarqueeViewDescriptor;
  @State start: boolean = false;
  private fromStart: boolean = true;
  private step: number = 15;
  private loop: number = Number.POSITIVE_INFINITY;
  @State src: string = "敬请期待"; // 和props里的src绑定

  aboutToAppear() {
    this.start = true;
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<MarqueeViewDescriptor>(this.tag);

    this.unregisterDescriptorChangesListener =
      this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag, (newDescriptor) => {
        this.descriptor = (newDescriptor as MarqueeViewDescriptor);
      })

    console.log('MarqueeView rawProps = ', JSON.stringify(this.descriptor.rawProps), this.tag);
    console.log('MarqueeView props = ', JSON.stringify(this.descriptor.props), this.tag);
    this.src = (this.descriptor.rawProps as MarqueeViewProps).src;

    // 监听rn侧的事件消息
    this.ctx.componentCommandReceiver.registerCommandCallback(this.tag, (commandName) => {
      if (commandName === "toggleMarqueeState") {
        this.start = !this.start;
        console.log("rn marqueeView called toggleMarqueeState!");

        this.ctx.rnInstance.emitComponentEvent(
          this.descriptor.tag,
          "onStop",
          { isStop: !this.start, type: "custom" }
        )
      }
    })
  }

  aboutToDisappear() {
    this.start = false;
    this.unregisterDescriptorChangesListener?.();
  }

  build() {
    RNViewBase({ ctx: this.ctx, tag: this.tag }) {
      Column() {
        Marquee({
          start: this.start,
          step: this.step,
          loop: this.loop,
          fromStart: this.fromStart,
          src: this.src
        })
          .fontColor('#FFFFFF')
          .fontSize(48)
          .fontWeight(700)
          .margin({ left: 10, right: 10 })
          .onTouch(() => { // 通过emit方式发送消息给rn侧调用rn的方法
            this.ctx.rnInstance.emitDeviceEvent("clickMarqueeEvent", { params: { age: 18 } });
          })
      }
      .height(180)
      .justifyContent(FlexAlign.Center)
    }
  }
}
