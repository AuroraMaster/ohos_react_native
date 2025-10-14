/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { NativeEventEmitter } from 'react-native';
import { TestCase, TestSuite } from '@rnoh/testerino';

export const NativeEventEmitterTest = () => {
  return (
    <TestSuite name="NativeEventEmitter">
      <TestCase
        itShould="emit and react to events"
        fn={async ({ expect }) => {
          const eventEmitter = new NativeEventEmitter();
          return new Promise(resolve => {
            eventEmitter.addListener('foo', event => {
              expect(event).to.be.eq('payload');
              resolve();
            });
            eventEmitter.emit('foo', 'payload');
          });
        }}
      />
    </TestSuite>
  );
};
