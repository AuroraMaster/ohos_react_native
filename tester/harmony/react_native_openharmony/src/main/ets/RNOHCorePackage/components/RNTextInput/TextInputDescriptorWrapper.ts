/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { convertColorValueToRGBA, Descriptor, Point } from '../../../RNOH/ts';
import { ViewDescriptorWrapperBase } from '../RNViewBase/ViewDescriptorWrapper';
import { ClearButtonMode, Selection, TextInputProps, TextInputRawProps, TextInputState } from './types';


export class TextInputDescriptorWrapper extends ViewDescriptorWrapperBase<string, TextInputProps, TextInputState, TextInputRawProps> {
  constructor(descriptor: Descriptor<string, TextInputProps, TextInputState, TextInputRawProps>) {
    super(descriptor)
  }

  public get position(): Point {
    return {
      x: this.padding.left.asNumber(),
      y: this.padding.top.asNumber(),
    }
  }

  public get contentWidth(): number {
    return this.width - (this.padding.left.asNumber() + this.padding.right.asNumber())
  }

  public get contentHeight(): number {
    return this.height - (this.padding.top.asNumber() + this.padding.bottom.asNumber())
  }

  public get fontWeight(): number | undefined {
    return this.rawProps.fontWeight;
  }

  public get fontFamily(): string | undefined {
    return this.rawProps.fontFamily;
  }

  public get fontSize(): string | number | undefined {
    const pixelUnit = this.rawProps.allowFontScaling ? 'fp' : 'vp';
    const size = this.rawProps.fontSize;
    return size ? `${size}${pixelUnit}` : size;
  }

  public get fontColor(): string {
    return convertColorValueToRGBA(this.rawProps.color, '#FF000000');
  }

  public get fontStyle(): string {
    return this.rawProps.fontStyle;
  }

  public get autoFocus(): boolean {
    return this.rawProps.autoFocus ?? false;
  }

  public get clearTextOnFocus(): boolean {
    return this.rawProps.clearTextOnFocus ?? false;
  }

  public get clearButtonMode(): ClearButtonMode {
    return this.rawProps.clearButtonMode ?? 'never';
  }

  public get selectTextOnFocus(): boolean {
    return this.rawProps.selectTextOnFocus ?? false;
  }

  public get contextMenuHidden(): boolean {
    return this.rawProps.contextMenuHidden ?? false;
  }
}