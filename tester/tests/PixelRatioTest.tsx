/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {PixelRatio, Text} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../components';

export const PixelRatioTest = () => {
  return (
    <TestSuite name="PixelRatio">
      <TestCase.Logical
        itShould="return plausible pixel ratio (greater than or equal  1, less than 4)"
        fn={({expect}) => {
          expect(PixelRatio.get())
            .to.be.greaterThanOrEqual(1)
            .and.to.be.lessThan(4);
        }}
      />
      <TestCase.Logical
        itShould="return plausible fontScale (greater than 0, less than 4)"
        fn={({expect}) => {
          expect(PixelRatio.getFontScale())
            .to.be.greaterThan(0)
            .and.to.be.lessThan(4);
        }}
      />
      <TestCase.Logical
        itShould="convert layout size to pixel size correctly"
        fn={({expect}) => {
          expect(PixelRatio.getPixelSizeForLayoutSize(10)).to.be.closeTo(
            PixelRatio.get() * 10,
            0.51,
          );
        }}
      />
      <TestCase.Logical
        itShould="round to nearest pixel correctly"
        fn={({expect}) => {
          const ratio = PixelRatio.get();
          const nearestPixel = Math.round(8.4 * ratio) / ratio;
          expect(PixelRatio.roundToNearestPixel(8.4)).to.be.equal(nearestPixel);
        }}
      />
      <TestCase.Example itShould="display PixelRatio results">
        <Text>
          {JSON.stringify(
            {
              get: PixelRatio.get(),
              getFontScale: PixelRatio.getFontScale(),
              getPixelSizeForLayoutSize:
                PixelRatio.getPixelSizeForLayoutSize(10),
              roundToNearestPixel: PixelRatio.roundToNearestPixel(16.5),
            },
            null,
            2,
          )}
        </Text>
      </TestCase.Example>
    </TestSuite>
  );
};
