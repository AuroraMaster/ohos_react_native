/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// setting paragraph attributes to the container text component
import { AttributedFragment, BaseAttributeModifier, PaddingProps } from '..'
import { ColorSegments, convertColorSegmentsToString } from '../../../RNOH'
import {
  stringToAlignment,
  stringToVerticalAlign,
  ellipsizeModeToTextOverflow,
  getMaxLines,
  getFontSize,
  stringToFontStyle,
  getTextDecorationLine,
  getLetterSpacing,
  stringToTextTransform
} from './conversions'
import { TextDescriptorWrapper } from './TextDescriptorWrapper'

export class RNParagraphExperimentalTextAttributeModifier extends BaseAttributeModifier<TextDescriptorWrapper, TextAttribute> {
  applyNormalAttribute(instance: TextAttribute): void {
    this.maybeAssignAttribute((dw => dw.width), (val) => instance.width(val), 0)
    this.maybeAssignAttribute((dw => dw.height), (val) => instance.height(val), 0)
    this.maybeAssignAttribute((dw => dw.textAlign), (val) => instance.textAlign(stringToAlignment(val)), undefined)
    this.maybeAssignAttribute((dw => dw.textAlignVertical), (val) => instance.align(stringToVerticalAlign(val)), undefined)
    this.maybeAssignAttribute((dw => dw.paragraphPadding), (val) => instance.padding(val), {
      top: 0,
      right: 0,
      bottom: 0,
      left: 0
    } as PaddingProps)
    this.maybeAssignAttribute((dw => dw.selectable), (val) => instance.copyOption(val ? CopyOptions.LocalDevice : CopyOptions.None), false)
    this.maybeAssignAttribute((dw => dw.enabled), (val) => instance.enabled(val), true)
    this.maybeAssignAttribute((dw => dw.ellipsizeMode), (val) => instance.textOverflow({
      overflow: ellipsizeModeToTextOverflow(val)
    }), undefined)
    this.maybeAssignAttribute((dw => dw.maximumNumberOfLines), (val) => instance.maxLines(getMaxLines(val)), undefined)
  }
}

interface DecorationWrapper {
  type: number | undefined,
  fontColor: ColorSegments | undefined,
  textDecorationColor: ColorSegments | undefined,
}

export class RNParagraphExperimentalSpanAttributeModifier extends BaseAttributeModifier<AttributedFragment, SpanAttribute> {
  protected allowFontScaling?: boolean;

  applyNormalAttribute(instance: SpanAttribute): void {
    this.maybeAssignAttribute((fr => fr.fontColor), (val) => instance.fontColor(convertColorSegmentsToString(val)), [0, 0, 0, 1] as ColorSegments)
    this.maybeAssignAttribute((fr => fr.fontSize), (val) => instance.fontSize(getFontSize(val, this.allowFontScaling)), 16)
    this.maybeAssignAttribute((fr => fr.fontFamily), (val) => instance.fontFamily(val), "HarmonyOS Sans")
    this.maybeAssignAttribute((fr => fr.fontWeight), (val) => instance.fontWeight(val), FontWeight.Normal)
    this.maybeAssignAttribute((fr => fr.fontStyle), (val) => instance.fontStyle(stringToFontStyle(val)), undefined)
    this.maybeAssignAttribute(((fr): DecorationWrapper => {
      return {
        type: fr.textDecorationLine,
        fontColor: fr.fontColor,
        textDecorationColor: fr.textDecorationColor
      }
    }), (val) => instance.decoration({
      type: getTextDecorationLine(val?.type),
      color: val?.textDecorationColor
        ? convertColorSegmentsToString(val?.textDecorationColor)
        : convertColorSegmentsToString(val?.fontColor)
    }
    ), undefined)
    this.maybeAssignAttribute((fr => fr.letterSpacing), (val) => instance.letterSpacing(getLetterSpacing(val)), undefined)
    this.maybeAssignAttribute((fr => fr.lineHeight), (val) => instance.lineHeight(val), undefined)
    //currently not supported for attributeModifiers
    // this.maybeAssignAttribute((fr => fr.textShadowProps), (val) => instance.textShadow(getTextShadow(val)), undefined)
    // this.maybeAssignAttribute((fr => fr.backgroundColor), (val) => instance.textBackgroundStyle({
    //   color: convertColorSegmentsToString(val)
    // }), undefined)
    this.maybeAssignAttribute((fr => fr.textTransform), (val) => instance.textCase(stringToTextTransform(val)), undefined)

  }

  constructor(propertyHolder: AttributedFragment, allowFontScaling?: boolean) {
    super(propertyHolder);
    this.allowFontScaling = allowFontScaling
  }
}


