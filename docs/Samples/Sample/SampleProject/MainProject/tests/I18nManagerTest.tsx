/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TestSuite, TestCase } from '@rnoh/testerino';
import { I18nManager } from 'react-native';

export function I18nManagerTest() {
  return (
    <TestSuite name="I18nManager">
      <TestCase
        itShould="be LTR be true"
        fn={({ expect }) => {
          expect(I18nManager.isRTL).to.be.false;
        }}
      />
      <TestCase
        itShould="doLeftAndRightSwapInRTL to be true"
        fn={({ expect }) => {
          expect(I18nManager.doLeftAndRightSwapInRTL).to.be.true;
        }}
      />
    </TestSuite>
  );
}
