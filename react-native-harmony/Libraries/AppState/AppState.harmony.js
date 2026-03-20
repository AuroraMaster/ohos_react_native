/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

// RNOH: patch
// Wrap the upstream AppState implementation and only extend it with
// the Harmony-specific `memoryLevelChange` event.

import type { EventSubscription } from 'react-native/Libraries/vendor/emitter/EventEmitter';

export type AppStateMemoryLevelInfo = {
  level: number,
  levelName: string,
};

const AppState = require('react-native/Libraries/AppState/AppState');

const originalAddEventListener = AppState.addEventListener.bind(AppState);

AppState.addEventListener = function (
  type: string,
  handler: (...args: any[]) => void,
): EventSubscription {
  if (type === 'memoryLevelChange') {
    const emitter = AppState._emitter;
    if (emitter == null) {
      throw new Error('Cannot use AppState when `isAvailable` is false.');
    }
    return emitter.addListener('memoryLevelChange', handler);
  }

  return originalAddEventListener(type, handler);
};

module.exports = AppState;