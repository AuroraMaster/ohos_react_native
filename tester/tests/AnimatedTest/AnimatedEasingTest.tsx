/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useRef} from 'react';
import {Animated, View, Easing, EasingFunction} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../../components';

export function AnimatedEasingTest() {
  return (
    <TestSuite name="Easing">
      <TestSuite name="linear">
        <TestCase.Example itShould="move square linearly">
          <EasingView easing={Easing.linear} />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="ease">
        <TestCase.Example itShould="move square slowly accelerating">
          <EasingView easing={Easing.ease} />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="cubic">
        <TestCase.Example itShould="move square with backwards cubic function">
          <EasingView easing={Easing.out(Easing.cubic)} />
        </TestCase.Example>
      </TestSuite>
    </TestSuite>
  );
}

const EasingView = (props: {easing: EasingFunction}) => {
  const translateValue = useRef(new Animated.Value(0)).current;
  const animation = Animated.timing(translateValue, {
    toValue: 200,
    duration: 2000,
    easing: props.easing,
    useNativeDriver: true,
  });
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };
  return (
    <View style={{height: 60, width: '100%'}} onTouchEnd={handleAnimation}>
      <Animated.View
        style={{
          height: 40,
          width: 40,
          margin: 10,
          backgroundColor: 'red',
          transform: [
            {
              translateX: translateValue,
            },
          ],
        }}
      />
    </View>
  );
};
