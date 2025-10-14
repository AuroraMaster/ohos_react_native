/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNOHContext, RNViewBase } from '@rnoh/react-native-openharmony';
import { RNC } from "@rnoh/react-native-openharmony/generated";

let nextKey = 0;

@Observed
class SelectItems {
  key: number;
  id: number;
  text: string;
  selected: boolean;

  constructor(id: number, text: string, selected: boolean) {
    this.key = nextKey++;
    this.id = id;
    this.text = text;
    this.selected = selected;
  }
}

@Component
export struct SelectBox {
  public static readonly NAME = RNC.SelectBox.NAME;
  public ctx!: RNOHContext;
  public tag: number = 0;
  @State descriptorWrapper: RNC.SelectBox.DescriptorWrapper = {} as RNC.SelectBox.DescriptorWrapper;
  @State data: SelectItems[] = [];
  @State mutil: boolean = false;
  private eventEmitter: RNC.SelectBox.EventEmitter | undefined = undefined;
  private cleanUpCallbacks: (() => void)[] = [];

  aboutToAppear() {
    this.eventEmitter = new RNC.SelectBox.EventEmitter(this.ctx.rnInstance, this.tag);
    this.onDescriptorWrapperChange(this.ctx.descriptorRegistry.findDescriptorWrapperByTag<RNC.SelectBox.DescriptorWrapper>(this.tag)!);
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (descriptor) => {
        this.onDescriptorWrapperChange(new RNC.SelectBox.DescriptorWrapper(descriptor));
      }
    ))
    this.cleanUpCallbacks.push(new RNC.SelectBox.CommandReceiver(this.ctx.componentCommandReceiver,
      this.tag).subscribe("emitNativeEvent", (argv) => {
      console.log(`收到 RN 传来的消息 ================ ${JSON.stringify(argv)}`);
      this.mutil = argv[0] || false;
    }))
  }

  private onDescriptorWrapperChange(descriptorWrapper: RNC.SelectBox.DescriptorWrapper) {
    this.descriptorWrapper = descriptorWrapper;
    this.data = [];
    const data = this.descriptorWrapper.props.data;
    const value = this.descriptorWrapper.props.value;
    for (const item of data) {
      this.data.push(
        new SelectItems(item.id, item.text, value.includes(item.id))
      )
    }
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cb => cb());
  }

  build() {
    RNViewBase({ ctx: this.ctx, tag: this.tag }) {
      Column() {
        ForEach(this.data, (item: SelectItems) => {
          SelectItem({
            item: item,
            onSelect: (idx: number) => {
              for (const tmp of this.data) {
                if (tmp.id === idx) {
                  tmp.selected = !tmp.selected;
                } else if (!this.mutil) {
                  tmp.selected = false;
                }
              }
            }
          })
        }, (item: SelectItems) => item.key.toString())
        Button("确定")
          .onClick(() => {
            const value = this.data
              .filter((item: SelectItems) => item.selected)
              .map((item: SelectItems) => item.id)
            // 发送事件方式[1]
            // this.ctx.rnInstance.emitComponentEvent(this.tag, "change", { value })
            // 发送事件方式[2]
            this.eventEmitter!.emit("change", {
              value,
            })
          })
      }
      .height("100%")
    }
  }
}

@Component
struct SelectItem {
  @ObjectLink item: SelectItems;
  onSelect?: (id: number) => void;

  build() {
    Row() {
      Text(this.item.text)
        .fontColor('#0F294D')
        .fontSize(18)
        .layoutWeight(1)

      Text('选中')
        .fontColor('#FF287DFA')
        .fontSize(20)
        .visibility(this.item.selected ? Visibility.Visible : Visibility.Hidden)
    }
    .padding({
      top: 24,
      left: 24,
      right: 24
    })
    .width("100%")
    .onClick(() => {
      this.onSelect?.(this.item.id)
    })
  }
}
