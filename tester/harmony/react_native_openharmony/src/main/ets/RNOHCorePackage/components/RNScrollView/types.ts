/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Tag } from '../../../RNOH/ts';
import { ViewBaseProps, ViewRawProps } from '../RNViewBase/ts';


export interface ScrollViewProps extends ViewBaseProps {
  scrollEnabled?: boolean
  contentOffsetX: number
  contentOffsetY: number
  flexDirection: number
  alwaysBounceHorizontal?:boolean
  alwaysBounceVertical?:boolean
  bounces: boolean
  persistentScrollbar: boolean
  showsHorizontalScrollIndicator: boolean
  showsVerticalScrollIndicator: boolean
  indicatorStyle?: IndicatorStyle
  decelerationRate: number
  scrollEventThrottle: number
  removeClippedSubviews:boolean
}

export interface ScrollViewRawProps extends ViewRawProps {
  alwaysBounceHorizontal?:boolean
  alwaysBounceVertical?:boolean
  snapToInterval?: number
  snapToOffsets?: number[]
  snapToStart?: boolean
  snapToEnd?: boolean
  pagingEnabled?: boolean
  snapToAlignment?: string
  disableIntervalMomentum?: boolean
  inverted?: boolean
  maintainVisibleContentPosition?: MaintainVisibleContentPosition
  scrollToOverflowEnabled?: boolean
}

export interface ScrollViewState {
  contentOffsetX: number
  contentOffsetY: number
  contentSizeWidth: number
  contentSizeHeight: number
}

export enum IndicatorStyle {
  Default = 0,
  Black = 1,
  White = 2,
}

export interface ScrollOffset {
  xOffset: number,
  yOffset: number
}

/**
 * @deprecated: Use ScrollOffset
 */
export type CurrentOffset = ScrollOffset

export interface Dimensions {
  width: number;
  height: number;
}

export interface Coordinates {
  x: number;
  y: number;
}

export interface ScrollEvent {
  contentSize: Dimensions;
  contentOffset: Coordinates;
  containerSize: Dimensions;
  zoomScale: number;
  responderIgnoreScroll: boolean;
}

export interface MaintainVisibleContentPosition {
  minIndexForVisible: number,
  autoscrollToTopThreshold?: number,
}

export interface FirstVisibleView {
  position: number,
  tag: Tag,
}

export enum ScrollPosition {
  EDGE_START,
  MIDDLE,
  EDGE_END,
}
