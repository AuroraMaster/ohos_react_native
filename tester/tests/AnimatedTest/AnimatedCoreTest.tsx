/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {Animated} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../../components';

export function AnimatedCoreTest() {
  return (
    <TestSuite name="Core">
      <TestCase.Example itShould="animate width">
        <AnimatedRectangle />
      </TestCase.Example>
    </TestSuite>
  );
}

function AnimatedRectangle() {
  const animWidth = React.useRef(new Animated.Value(100)).current;

  const animation = React.useMemo(() => {
    const expand = Animated.timing(animWidth, {
      toValue: 300,
      duration: 1000,
      useNativeDriver: false,
    });
    const contract = Animated.timing(animWidth, {
      toValue: 100,
      duration: 1000,
      useNativeDriver: false,
    });
    return Animated.sequence([expand, contract]);
  }, []);

  return (
    <Animated.View
      style={{
        height: 100,
        width: animWidth,
        backgroundColor: 'red',
      }}
      onTouchEnd={() => {
        animation.reset();
        animation.start();
      }}
    />
  );
}
