/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  Tag,
  RNOHContext,
  RNComponentContext,
  DescriptorWrapper,
  CustomComponentBuilder,
  ComponentBuilderContext,
  DescriptorEssence,
} from "../../RNOH"
import { buildRNView, RNView } from "./RNView"
import { RNImage } from "./RNImage"
import { RNTextInput } from "./RNTextInput"
import { RNText } from "./RNText"
import { RNScrollView } from "./RNScrollView"
import { Descriptor } from "../../RNOH/DescriptorBase";
import { RNModalHostView } from './RNModalHostView'
import { RNSwitch } from './RNSwitch'
import { RNActivityIndicator } from './RNActivityIndicator'


@Builder
export function buildRNComponent(ctx: RNComponentContext, descriptorWrapper: DescriptorWrapper) {
  if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "View" || ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "RootView") {
    buildRNView(ctx, descriptorWrapper.tag)
  } else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "Image") {
    RNImage({
      ctx: ctx,
      tag: descriptorWrapper.tag,
    })
  } else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "TextInput") {
    RNTextInput({
      ctx: ctx,
      tag: descriptorWrapper.tag,
    })
  } else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "Paragraph") {
    RNText({
      ctx: ctx,
      tag: descriptorWrapper.tag,
    })
  } else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "PullToRefreshView") {
    /**
     * RNScrollView displays Refresh component if PullToRefreshView is its direct parent.
     * Native Refresh component works correctly, only when a native Scroll is its direct child.
     */
    buildRNComponent(ctx, ctx.descriptorRegistry.findDescriptorWrapperByTag(descriptorWrapper.childrenTags[0])!)
  }
  else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "ScrollView") {
    RNScrollView({
      ctx: ctx,
      tag: descriptorWrapper.tag,
    })
  } else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "Switch") {
    RNSwitch({
      ctx: ctx,
      tag: descriptorWrapper.tag,
    })
  } else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "ModalHostView") {
    RNModalHostView({
      ctx: ctx,
      tag: descriptorWrapper.tag,
    })
  } else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) === "ActivityIndicatorView") {
    RNActivityIndicator({
      ctx: ctx,
      tag: descriptorWrapper.tag,
    })
  } else if (ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_) !== '') {
    ctx.wrappedCustomRNComponentBuilder.builder(new ComponentBuilderContext(
      ctx,
      ctx,
      descriptorWrapper.tag,
      ctx.rnInstance.getComponentNameFromDescriptorType(descriptorWrapper.type_),
      ctx.descriptorRegistry.getDescriptor<Descriptor>(descriptorWrapper.tag) as DescriptorEssence,
    ))
  }
}

/**
 * @deprecated: Use one of  builder wrappers in `RNComponentContext` instead.
 */
@Component
export struct RNComponentFactory {
  public ctx!: RNOHContext
  public tag: number = 0
  @BuilderParam public buildCustomComponent!: CustomComponentBuilder
  @State private componentName: string = ''
  @State private childrenTags: Tag[] = []
  private cleanupCallback?: () => void = undefined

  aboutToAppear() {
    const descriptor = this.ctx.descriptorRegistry.getDescriptor<Descriptor>(this.tag)
    if (descriptor) {
      this.setDescriptor(descriptor)
    }
    this.cleanupCallback = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(
      this.tag,
      (newDescriptor) => this.setDescriptor(newDescriptor)
    )
  }

  aboutToDisappear() {
    this.cleanupCallback?.()
  }

  setDescriptor(newDescriptor: Descriptor) {
    this.componentName = this.ctx.rnInstance.getComponentNameFromDescriptorType(newDescriptor.type)
    this.childrenTags = newDescriptor.childrenTags
  }

  build() {
    if (this.componentName === "View" || this.componentName === "RootView") {
      RNView({
        ctx: this.ctx,
        tag: this.tag,
        buildCustomComponent: this.buildCustomComponent,
      })
    } else if (this.componentName === "Image") {
      RNImage({ ctx: this.ctx, tag: this.tag })
    } else if (this.componentName === "TextInput") {
      RNTextInput({ ctx: this.ctx, tag: this.tag })
    } else if (this.componentName === "Paragraph") {
      RNText({ ctx: this.ctx, tag: this.tag, buildCustomComponent: this.buildCustomComponent })
    } else if (this.componentName === "PullToRefreshView") {
      /**
       * RNScrollView displays Refresh component if PullToRefreshView is its direct parent.
       * Native Refresh component works correctly, only when a native Scroll is its direct child.
       */
      RNComponentFactory({
        ctx: this.ctx,
        tag: this.childrenTags[0],
        buildCustomComponent: this.buildCustomComponent
      })
    }
    else if (this.componentName === "ScrollView") {
      RNScrollView({
        ctx: this.ctx,
        tag: this.tag,
        buildCustomComponent: this.buildCustomComponent
      })
    } else if (this.componentName === "Switch") {
      RNSwitch({
        ctx: this.ctx,
        tag: this.tag,
      })
    } else if (this.componentName === "ModalHostView") {
      RNModalHostView({
        ctx: this.ctx,
        tag: this.tag,
        buildCustomComponent: this.buildCustomComponent
      })
    } else if (this.componentName === "ActivityIndicatorView") {
      RNActivityIndicator({
        ctx: this.ctx,
        tag: this.tag,
      })
    } else if (this.componentName !== '' && this.ctx instanceof RNComponentContext) {
      this.buildCustomComponent(new ComponentBuilderContext(
        this.ctx,
        this.ctx,
        this.tag,
        this.componentName,
        this.ctx.descriptorRegistry.getDescriptor<Descriptor>(this.tag) as DescriptorEssence,
      ))
    }
  }
}