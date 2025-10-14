/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { BorderMetrics, ColorSegments, ColorValue } from './DescriptorBase'

/**
 * @api: RN_APP_DEVELOPER
 */
export enum DisplayMode {
  /*
   * The surface is running normally. All visual side-effects will be rendered
   * on the screen.
   */
  Visible = 0,

  /*
   * The surface is `Suspended`. All new (committed after switching to the
   * mode) visual side-effects will *not* be mounted on the screen (the screen
   * will stop updating).
   *
   * The mode can be used for preparing a surface for possible future use.
   * The surface will be prepared without spending computing resources
   * on mounting, and then can be instantly mounted if needed.
   */
  Suspended = 1,

  /*
   * The surface is `Hidden`. All previously mounted visual side-effects
   * will be unmounted, and all new (committed after switching to the mode)
   * visual side-effects will *not* be mounted on the screen until the mode is
   * switched back to `normal`.
   *
   * The mode can be used for temporarily freeing computing resources of
   * off-the-screen surfaces.
   */
  Hidden = 2,
}

/**
 * @api
 * @deprecated Use "Color.fromColorSegments().toRGBAString()" instead (latestRNOHVersion: 0.72.26)
 */
export function convertColorSegmentsToString(colorSegments?: ColorSegments) {
  if (!colorSegments) return undefined
  const [r, g, b, a] = colorSegments
  return `rgba(${Math.round(r * 255)}, ${Math.round(g * 255)}, ${Math.round(
    b * 255
  )}, ${a})`
}

/**
 * @api
 */
export function getTintColorMatrix(colorSegments?: ColorSegments) {
  if (!colorSegments || colorSegments.every((element) => element === 0)) {
    return [
      1, 0, 0, 0, 0,
      0, 1, 0, 0, 0,
      0, 0, 1, 0, 0,
      0, 0, 0, 1, 0,
    ]
  }
  const [r, g, b, a] = colorSegments
  return [
    0, 0, 0, r, 0,
    0, 0, 0, g, 0,
    0, 0, 0, b, 0,
    0, 0, 0, 1, 0,
  ]
}

/**
 * 0-255
 */
type ColorChannel = number

/**
 * @api
 */
export class Color {
  static fromColorValue(colorValue: ColorValue) {
    return Color.fromColorSegments(convertColorValueToColorSegments(colorValue))
  }

  static fromColorSegments([r,g,b,a]: ColorSegments) {
    return new Color({ r: r * 255, g: g * 255, b: b * 255, a: a * 255 })
  }

  constructor(private rgba: {
    r: ColorChannel,
    g: ColorChannel,
    b: ColorChannel,
    a: ColorChannel
  }) {
  }

  toRGBAString() {
    return `rgba(${this.rgba.r}, ${this.rgba.g}, ${this.rgba.b}, ${this.rgba.a / 255})`
  }
}

/**
 * @api
 * @deprecated Use Color::fromColorValue().toRGBAString() instead (latestRNOHVersion: 0.72.26)
 */
export function convertColorValueToRGBA(colorValue: ColorValue | undefined, defaultColor: string = "rgba(0,0,0,0.0)") {
  if (colorValue === undefined) return defaultColor;
  const rgba = {
    a: ((colorValue >> 24) & 0xff) / 255,
    r: (colorValue >> 16) & 0xff,
    g: (colorValue >> 8) & 0xff,
    b: ((colorValue >> 0) & 0xff),
  }
  return `rgba(${rgba.r}, ${rgba.g}, ${rgba.b}, ${rgba.a})`
}

/**
 * @api
 */
export function convertColorValueToHex(colorValue: ColorValue | undefined, defaultColor: string = "#00000000") {
  if (colorValue === undefined) return defaultColor;
  const toHex = (num, padding) => num.toString(16).padStart(padding, '0');
  const argb = {
    a: (colorValue >> 24) & 0xff,
    r: (colorValue >> 16) & 0xff,
    g: (colorValue >> 8) & 0xff,
    b: ((colorValue >> 0) & 0xff),
  }
  return `#${toHex(argb.a, 2)}${toHex(argb.r, 2)}${toHex(argb.g, 2)}${toHex(argb.b, 2)}`;
}

/**
 * @api
 */
export function convertColorValueToColorSegments(colorValue: ColorValue | undefined): ColorSegments | undefined {
  if (colorValue === undefined) return undefined
  const rgba = {
    a: ((colorValue >> 24) & 0xff) / 255,
    r: ((colorValue >> 16) & 0xff) / 255,
    g: ((colorValue >> 8) & 0xff) / 255,
    b: ((colorValue >> 0) & 0xff) / 255,
  }
  return [rgba.r, rgba.g, rgba.b, rgba.a]
}

/**
 * @api
 */
export type TransformMatrix = [
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number
];

/**
 * @api
 */
export type ReadonlyTransformationMatrix = readonly [
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number,
  number
]

/**
 * @api
 * @deprecated Use ViewDescriptorWrapperBase::resolveEdges instead (latestRNOHVersion: 0.72.26)
 */
export enum BorderEdgePropsType {
  COLOR = "Color",
  WIDTH = "Width",
}

/**
 * @api
 * @deprecated Use ViewDescriptorWrapperBase::resolveEdges and ViewDescriptorWrapperBase::resolveCorners instead (latestRNOHVersion: 0.72.26)
 */
export function resolveBorderMetrics(props: BorderMetrics, isRTL: boolean): BorderMetrics {
  const colorProps = resolveBorderEdgeProps(props, BorderEdgePropsType.COLOR, isRTL);
  const widthProps = resolveBorderEdgeProps(props, BorderEdgePropsType.WIDTH, isRTL);
  const radiusProps = resolveBorderRadius(props);
  return { ...colorProps, ...widthProps, ...radiusProps, borderStyle: props.borderStyle };
}

/**
 * @api
 * @deprecated Use ViewDescriptorWrapperBase::resolveCorners instead (latestRNOHVersion: 0.72.26)
 */
export function resolveBorderRadius(props: BorderMetrics): BorderMetrics {
  const topLeft = props.borderTopLeftRadius;
  const topRight = props.borderTopRightRadius;
  const bottomLeft = props.borderBottomLeftRadius;
  const bottomRight = props.borderBottomRightRadius;
  const all = props.borderRadius;
  const resolvedProps = {
    borderTopLeftRadius: topLeft ?? all,
    borderTopRightRadius: topRight ?? all,
    borderBottomLeftRadius: bottomLeft ?? all,
    borderBottomRightRadius: bottomRight ?? all,
  }
  return resolvedProps;
}

/**
 * @api
 * @deprecated Use ViewDescriptorWrapperBase::resolveEdges instead (latestRNOHVersion: 0.72.26)
 */
export function resolveBorderEdgeProps(props: BorderMetrics, type: BorderEdgePropsType, isRTL: boolean): BorderMetrics {
  const left = props[`borderLeft${type}`]
  const top = props[`borderTop${type}`]
  const right = props[`borderRight${type}`]
  const bottom = props[`borderBottom${type}`]
  const all = props[`border${type}`]
  const start = props[`borderStart${type}`]
  const end = props[`borderEnd${type}`]

  const resolvedProps = {
    [`borderLeft${type}`]: left ?? ((isRTL ? end : start) ?? all),
    [`borderTop${type}`]: top ?? all,
    [`borderRight${type}`]: right ?? ((isRTL ? start : end) ?? all),
    [`borderBottom${type}`]: bottom ?? all,
  };

  return resolvedProps;
}

/**
 * @api
 */
export function getTransformedVector(transformMatrix: ReadonlyTransformationMatrix, vector: Array<number>): Array<number> {
  const resultVector = [0, 0, 0, 0]
  for (let i = 0; i < 4; ++i) {
    for (let j = 0; j < 4; ++j) {
      resultVector[i] += transformMatrix[i * 4+j] * vector[j]
    }
  }
  return resultVector
}