/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNOHContext, PLACEHOLDER_SYMBOL, CustomComponentBuilder } from '../../../RNOH'
import { RNSimpleText } from './RNSimpleText'
import { RNParagraph } from './RNParagraph'
import { RNParagraphExperimental } from './RNParagraphExperimental'
import { TextDescriptor } from './types'
import { RNParagraphManager } from './RNParagraphManager'

@Component
export struct RNText {
  public ctx!: RNOHContext
  public tag: number = 0
  /** @deprecated  */
  @BuilderParam public buildCustomComponent: CustomComponentBuilder
  // -------------------------------------------------------------------------------------------------------------------
  @State descriptor: TextDescriptor = {} as TextDescriptor
  private componentManager!: RNParagraphManager
  private cleanUpCallbacks: (() => void)[] = []

  aboutToAppear() {
    this.componentManager = new RNParagraphManager(this.tag, this.ctx)
    const descriptor = this.ctx.descriptorRegistry.getDescriptor<TextDescriptor>(this.tag)
    this.onDescriptorChange(descriptor)
    this.cleanUpCallbacks.push(this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager));
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.onDescriptorChange(newDescriptor as TextDescriptor)
      }
    ))
  }

  onDescriptorChange(newDescriptor: TextDescriptor) {
    this.descriptor = newDescriptor
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach((cb) => cb())
  }

  build() {

    if (this.ctx.rnInstance.isFeatureFlagEnabled("NDK_TEXT_MEASUREMENTS")) {
      RNParagraphExperimental({
        ctx: this.ctx,
        tag: this.tag,
        componentManager: this.componentManager,
        buildCustomComponent: this.buildCustomComponent
      })
    } else {
      if (this.descriptor.props.fragments.length <= 1 && !this.descriptor.props.fragments.some(fragment => fragment.text === PLACEHOLDER_SYMBOL)) {
        RNSimpleText({ ctx: this.ctx, tag: this.tag, componentManager: this.componentManager })
      } else {
        RNParagraph({
          ctx: this.ctx,
          tag: this.tag,
          componentManager: this.componentManager,
          buildCustomComponent: this.buildCustomComponent
        })
      }
    }
  }
}