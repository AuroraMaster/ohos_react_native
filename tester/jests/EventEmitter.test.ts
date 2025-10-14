/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {EventEmitter} from '../harmony/react_native_openharmony/src/main/ets/RNOH/EventEmitter';

it('should listen to emitted events', async () => {
  const ee = new EventEmitter<{FOO: [string, string]}>();

  setTimeout(() => {
    ee.emit('FOO', 'bar', 'baz');
  }, 0);
  const result = await new Promise(resolve => {
    const unsubscribe = ee.subscribe('FOO', (bar, baz) => {
      resolve([bar, baz]);
      unsubscribe();
    });
  });

  expect(result).toStrictEqual(['bar', 'baz']);
});
