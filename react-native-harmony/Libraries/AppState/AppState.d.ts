/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import {NativeEventSubscription} from 'react-native/Libraries/EventEmitter/RCTNativeAppEventEmitter';
import type {
  AppStateStatic as NativeAppStateStatic,
  AppStateStatus as NativeAppStateStatus,
} from 'react-native/Libraries/AppState/AppState';

export type AppStateEvent =
  | 'change'
  | 'memoryWarning'
  | 'memoryLevelChange'
  | 'blur'
  | 'focus';

export type AppStateStatus = NativeAppStateStatus;

export interface AppStateMemoryLevelInfo {
  level: number;
  levelName: string;
}

export interface AppStateStatic
  extends Omit<NativeAppStateStatic, 'addEventListener'> {
  addEventListener(
    type: 'change',
    listener: (state: AppStateStatus) => void,
  ): NativeEventSubscription;

  addEventListener(
    type: 'memoryWarning' | 'blur' | 'focus',
    listener: () => void,
  ): NativeEventSubscription;

  addEventListener(
    type: 'memoryLevelChange',
    listener: (info: AppStateMemoryLevelInfo) => void,
  ): NativeEventSubscription;
}

export const AppState: AppStateStatic;
export type AppState = AppStateStatic;