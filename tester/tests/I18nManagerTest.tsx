/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {I18nManager} from 'react-native';
import {Button, TestCase} from '../components';

export function I18nManagerTest() {
  return (
    <TestSuite name="I18nManager">
      <TestCase.Logical
        itShould="be LTR be true"
        fn={({expect}) => {
          expect(I18nManager.isRTL).to.be.false;
        }}
      />
      <TestCase.Logical
        itShould="doLeftAndRightSwapInRTL to be true"
        fn={({expect}) => {
          expect(I18nManager.doLeftAndRightSwapInRTL).to.be.true;
        }}
      />
      <TestCase.Logical
        itShould="forceRTL"
        fn={({expect}) => {
          I18nManager.forceRTL(false);
        }}
      />
      <TestCase.Logical
        itShould="allowRTL"
        fn={({expect}) => {
          I18nManager.allowRTL(true);
        }}
      />
      <TestCase.Example itShould="set forceRTL/allowRTL">
        <Button
          label="set forceRTL to be true"
          onPress={() => {
            I18nManager.forceRTL(true);
          }}
        />
        <Button
          label="set forceRTLbto be false"
          onPress={() => {
            I18nManager.forceRTL(false);
          }}
        />
        <Button
          label="set allowRTL to be true"
          onPress={() => {
            I18nManager.allowRTL(false);
          }}
        />
        <Button
          label="set allowRTL to be false"
          onPress={() => {
            I18nManager.allowRTL(false);
          }}
        />
      </TestCase.Example>
      <TestCase.Logical
        itShould="getConstants"
        fn={({expect}) => {
          expect(I18nManager.getConstants().isRTL).to.be.false;
          expect(I18nManager.getConstants().doLeftAndRightSwapInRTL).to.be.true;
          expect(I18nManager.getConstants().localeIdentifier).to.equal('zh-Hans-CN');
        }}
      />
    </TestSuite>
  );
}
