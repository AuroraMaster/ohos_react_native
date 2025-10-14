/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { ColorSegments } from '../../../RNOH/DescriptorBase'
import type { ViewBaseProps, ViewRawProps } from '../ts'

export type ClearButtonMode = 'never' | 'while-editing' | 'unless-editing' | 'always'

export interface TextInputProps extends ViewBaseProps {
  text?: string
  multiline?: boolean
  editable?: boolean
  caretHidden?: boolean
  selectionColor?: ColorSegments
  secureTextEntry?: boolean
  placeholder?: string
  placeholderTextColor?: ColorSegments
  returnKeyType?: string
  textAlign?: string
  keyboardType?: string
}

export interface TextInputRawProps extends ViewRawProps {
  maxLength?: number
  fontWeight?: number
  color?: number
  fontFamily?: string
  fontSize?: number
  fontStyle?: string
  autoFocus?: boolean
  allowFontScaling?: boolean
  clearTextOnFocus?: boolean
  mostRecentEventCount?: number
  clearButtonMode?: ClearButtonMode
  selectTextOnFocus?: boolean
  contextMenuHidden?: boolean
}

export interface TextInputState {
  wasAlreadyRendered?: boolean
}

export interface Selection {
  start: number,
  end: number,
}

export interface TextInputEvent {
  text: string,
  eventCount: number,
  selectionRange: {
    location: number,
    length: number,
  }
}

export interface TextInputKeyEvent {
  key: string,
  eventCount: number,
}
