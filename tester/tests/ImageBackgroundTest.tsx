/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {ImageBackground, Text} from 'react-native';
import {TestCase} from '../components';

const LOCAL_IMAGE_ASSET_ID = require('../assets/pravatar-131.jpg');

export const ImageBackgroundTest = () => {
  return (
    <TestSuite name="ImageBackground">
      <TestCase.Example itShould="show centered text on image background with red border">
        <ImageBackground
          source={LOCAL_IMAGE_ASSET_ID}
          style={{width: '100%', height: 100, justifyContent: 'center'}}
          imageStyle={{borderWidth: 4, borderRadius: 50, borderColor: 'red'}}
          resizeMode="cover">
          <Text
            style={{
              backgroundColor: 'rgba(0,0,0,0.8)',
              color: 'white',
              textAlign: 'center',
            }}>
            Image background
          </Text>
        </ImageBackground>
      </TestCase.Example>
    </TestSuite>
  );
};
