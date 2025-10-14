/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {useWindowDimensions} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../components';

export function UseWindowDimensionsTest() {
  const {height, width, scale, fontScale} = useWindowDimensions();

  return (
    <TestSuite name="useWindowDimensions">
      <TestCase.Logical
        itShould="return plausible width (greater than 0)"
        fn={({expect}) => {
          expect(width).to.be.greaterThan(0);
        }}
      />
      <TestCase.Logical
        itShould="return plausible height (greater than 0)"
        fn={({expect}) => {
          expect(height).to.be.greaterThan(0);
        }}
      />
      <TestCase.Logical
        itShould="return plausible scale (greater than 0)"
        fn={({expect}) => {
          expect(scale).to.be.greaterThan(0);
        }}
      />
      <TestCase.Logical
        itShould="return plausible fontScale (greater than 0)"
        fn={({expect}) => {
          expect(fontScale).to.be.greaterThan(0);
        }}
      />
    </TestSuite>
  );
}
