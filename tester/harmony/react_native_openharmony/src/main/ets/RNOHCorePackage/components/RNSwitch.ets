/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNViewManager } from '../componentManagers';
import { ColorSegments, convertColorSegmentsToString, Descriptor, RNOHContext } from '../../RNOH';
import { RNViewBase, ViewBaseProps, ViewRawProps } from './RNViewBase';

export interface SwitchProps extends ViewBaseProps {
  trackColor?: ColorSegments
  thumbColor?: ColorSegments
  value?: boolean
  disabled?: boolean
}

export type SwitchDescriptor = Descriptor<"Switch", SwitchProps, object, ViewRawProps>

@Component
export struct RNSwitch {
  ctx!: RNOHContext
  tag: number = 0
  @State private descriptor: SwitchDescriptor = Object() as SwitchDescriptor
  private cleanUpCallbacks: (() => void)[] = []
  private componentManager!: RNViewManager;

  aboutToAppear() {
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<SwitchDescriptor>(this.tag);
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.descriptor = (newDescriptor as SwitchDescriptor)
        if (this.componentManager) {
          this.componentManager.setIsEnabled(!this.descriptor.props.disabled);
        }
      }
    ));
    this.componentManager = new RNViewManager(this.tag, this.ctx);
    this.componentManager.setIsEnabled(!this.descriptor.props.disabled);
    this.cleanUpCallbacks.push(this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager));
  }

  onChange(isOn: boolean) {
    this.ctx.rnInstance.emitComponentEvent(
      this.descriptor.tag,
      "onChange",
      {
        value: isOn,
        target: this.descriptor.tag,
      }
    )
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cleanUp => cleanUp());
  }

  build() {
    RNViewBase({
      ctx: this.ctx,
      tag: this.tag,
      componentManager: this.componentManager,
      controlsFocus: false,
    }) {
      Toggle({ type: ToggleType.Switch, isOn: this.descriptor.props.value })
        .margin(0)
        .width("100%")
        .height("100%")
        .selectedColor(convertColorSegmentsToString(this.descriptor.props.trackColor))
        .switchPointColor(convertColorSegmentsToString(this.descriptor.props.thumbColor))
        .onChange((isOn) => {
          this.onChange(isOn)
        })
        .enabled(!this.descriptor.props.disabled)
        .focusable(this.descriptor.rawProps.focusable ?? true)
    }
  }
}