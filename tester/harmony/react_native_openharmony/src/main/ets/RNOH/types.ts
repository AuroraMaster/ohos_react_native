/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export type Point = {
  x: number,
  y: number,
}

export type Position = {
  x: number,
  y: number,
}

export type BoundingBox = {
  left: number,
  right: number,
  top: number,
  bottom: number,
}

export interface Size {
  width: number;
  height: number;
}

export type Edges<T> = {
  top: T,
  left: T,
  right: T,
  bottom: T
}

export type Corners<T> = {
  topLeft: T,
  topRight: T,
  bottomLeft: T,
  bottomRight: T,
}

// The minimum supported SDK API version for getWindowDensityInfo is 15
export const OH_API_LEVEL_15 = 15;

export type PointerEvents = "auto" | "none" | "box-none" | "box-only"

export type DisplayMetrics = {
  windowPhysicalPixels: PhysicalPixels,
  screenPhysicalPixels: PhysicalPixels,
};

export type PhysicalPixels = {
  top?: number,
  left?: number,
  width: number,
  height: number,
  decorHeight?: number,
  scale: number,
  fontScale: number,
  densityDpi: number,
  xDpi: number,
  yDpi: number
}

export type InspectorPageId = number

export interface InspectorPage {
  id: InspectorPageId,
  title: string,
  vm: string
}

// RemoteConnection allows the VM to send debugger messages to the debugger.
export interface InspectorRemoteConnection {
  onMessage(message: string)
  onDisconnect()
}

// LocalConnection allows the debugger to send debugger messages to the VM.
export interface InspectorLocalConnection {
  sendMessage(payload: unknown)
  disconnect()
}

export interface InspectorInstance {
  getPages(): InspectorPage[]
  connect(pageId: InspectorPageId, remote: InspectorRemoteConnection): InspectorLocalConnection
}

export interface DevMenu {
  addMenuItem: (title: string) => void;
  show: () => void;
}

export interface InternalDevMenu extends DevMenu {
  onDestroy: () => void;
}
