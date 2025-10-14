/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNComponentContext,
  RNOHContext,
  convertColorSegmentsToString,
  Tag,
  CustomComponentBuilder,
  DescriptorWrapper} from '../../../RNOH'
import { RNViewBaseAttributeModifier } from '../RNViewBase'
import { PLACEHOLDER_SYMBOL } from '../../../ParagraphMeasurer';
import { PositionedLine, Size, } from '../../../ParagraphMeasurer'
import { RNParagraphManager } from './RNParagraphManager'
import { RNComponentFactory } from '../RNComponentFactory'
import { TextDescriptor, TextFragmentExtraData } from './types'
import { getTextShadow,
  stringToStackAlignment,
} from './conversions'
import { TextDescriptorWrapper } from './TextDescriptorWrapper';
import {
  RNParagraphExperimentalTextAttributeModifier,
  RNParagraphExperimentalSpanAttributeModifier
} from './AttributeModifiers';


@Component
export struct RNParagraphExperimental {
  public ctx!: RNOHContext
  public tag: number = 0
  /** @deprecated  */
  @BuilderParam public buildCustomComponent: CustomComponentBuilder
  //--------------------------------------------------------------------------------------------------------------------
  @State descriptor: TextDescriptor = {} as TextDescriptor
  @State positionedLines: PositionedLine<TextFragmentExtraData>[] = []
  @State paragraphSize: Size = { width: 0, height: 0 }
  @State descriptorWrapper: TextDescriptorWrapper | undefined = undefined
  @State private rnViewBaseAttributeModifier: RNViewBaseAttributeModifier | undefined = undefined
  @State private textAttributeModifier: RNParagraphExperimentalTextAttributeModifier | undefined = undefined
  @State private spanAttributeModifiers: RNParagraphExperimentalSpanAttributeModifier[] | undefined = undefined;
  // @State private fragmentWrappers: FragmentWrapper[] | undefined = undefined;
  private componentManager!: RNParagraphManager
  private cleanUpCallbacks: (() => void)[] = []

  aboutToAppear() {
    this.onDescriptorChange(this.ctx.descriptorRegistry.getDescriptor<TextDescriptor>(this.tag))
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.onDescriptorChange(newDescriptor as TextDescriptor)
      }
    ))
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach((cb) => cb())
  }

  onDescriptorChange(newDescriptor: TextDescriptor) {
    this.descriptor = newDescriptor
    this.descriptorWrapper = new TextDescriptorWrapper(newDescriptor)
    this.spanAttributeModifiers = []
    if (this.ctx instanceof RNComponentContext) {
      this.rnViewBaseAttributeModifier = new RNViewBaseAttributeModifier(this.descriptorWrapper, this.ctx)
      this.textAttributeModifier = new RNParagraphExperimentalTextAttributeModifier(this.descriptorWrapper)
      this.descriptor.props.fragments.forEach(fragment => {
        const mod = new RNParagraphExperimentalSpanAttributeModifier(fragment, this.descriptor.rawProps.allowFontScaling)
        this.spanAttributeModifiers?.push(mod);
      });
    }
  }

  getVerticalAlignmentOffset() {
    const maxOffset = Math.max(this.descriptor.layoutMetrics.frame.size.height - this.paragraphSize.height, 0)
    switch (this.descriptor.props.textAlignVertical) {
      case "bottom":
        return maxOffset
      case "center":
        return maxOffset / 2
      default:
        return 0
    }
  }

  build() {
    Stack() {
      /**
       * Focus hack: container components cannot be focused if they don't contain focusable children.
       * An invisible button is added to a focusable View to avoid this problem.
       */
      if (this.descriptor.rawProps.focusable) {
        Button().width(0).height(0).padding(0).margin(0)
      }
      Text() {
        ForEach(this.spanAttributeModifiers, (attributeModifier: RNParagraphExperimentalSpanAttributeModifier) => {
          if (attributeModifier.propertyHolder.text !== PLACEHOLDER_SYMBOL) {
            Span(attributeModifier.propertyHolder.text)
              .attributeModifier(attributeModifier)//The 2 following properties aren't supported yet by attributeModifier
              .textBackgroundStyle({
                color: convertColorSegmentsToString(attributeModifier.propertyHolder.backgroundColor)
              })
              .textShadow(getTextShadow(attributeModifier.propertyHolder.textShadowProps))
          } else {
            ImageSpan('')
              .width(attributeModifier.propertyHolder.parentShadowView?.layoutMetrics.frame.size.width)
              .height(attributeModifier.propertyHolder.parentShadowView?.layoutMetrics.frame.size.height)
          }
        })
      }
      .attributeModifier(this.textAttributeModifier)
      if (this.ctx instanceof RNComponentContext) {
        LazyForEach(this.ctx.createComponentDataSource({ tag: this.tag }),
          (descriptorWrapper: DescriptorWrapper) => {
            (this.ctx as RNComponentContext).wrappedRNComponentBuilder.builder((this.ctx as RNComponentContext), descriptorWrapper.tag)
          },
          (descriptorWrapper: DescriptorWrapper) => descriptorWrapper.tag.toString() + "@" + descriptorWrapper.renderKey
        )
      } else {
        ForEach(this.descriptor.childrenTags, (childrenTag: Tag) => {
          RNComponentFactory({ ctx: this.ctx, tag: childrenTag, buildCustomComponent: this.buildCustomComponent })
        }, (childrenTag: Tag) => childrenTag.toString())
      }
    }
    .id(this.tag.toString())
    .align(stringToStackAlignment(this.descriptor.props.textAlign, this.descriptor.props.textAlignVertical))
    .attributeModifier(this.rnViewBaseAttributeModifier)
  }
}
