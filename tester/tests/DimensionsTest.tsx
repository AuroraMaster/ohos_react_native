/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import React, {useState, useEffect} from 'react';
import {Text, StyleSheet, Dimensions, View} from 'react-native';
import {TestCase} from '../components';

export function DimensionsTest() {
  const [windowDimensions, setWindowDimensions] = useState(
    Dimensions.get('window'),
  );
  const [screenDimensions, setScreenDimensions] = useState(
    Dimensions.get('screen'),
  );

  useEffect(() => {
    const windowSubscription = Dimensions.addEventListener(
      'change',
      ({window}) => setWindowDimensions(window),
    );
    const screenSubscription = Dimensions.addEventListener(
      'change',
      ({screen}) => setScreenDimensions(screen),
    );
    return () => {
      windowSubscription.remove();
      screenSubscription.remove();
    };
  }, []);

  return (
    <TestSuite name="Dimensions">
      <TestCase.Logical
        itShould="export dimensions"
        fn={({expect}) => {
          expect(Dimensions).to.not.be.undefined;
          expect(Dimensions.get).to.not.be.undefined;
        }}
      />
      <TestCase.Logical
        itShould="gets window dimensions without throwing"
        fn={({expect}) => {
          expect(Dimensions.get.bind(Dimensions, 'window')).to.not.throw();
        }}
      />
      <TestCase.Logical
        itShould="gets screen dimensions without throwing"
        fn={({expect}) => {
          expect(Dimensions.get.bind(Dimensions, 'screen')).to.not.throw();
        }}
      />
      <TestCase.Example itShould="display window dimensions">
        <Text style={styles.text}>
          Window dimensions: {JSON.stringify(windowDimensions)}
        </Text>
      </TestCase.Example>
      <TestCase.Example itShould="display screen dimensions">
        <Text style={styles.text}>
          Screen dimensions: {JSON.stringify(screenDimensions)}
        </Text>
      </TestCase.Example>
      <TestCase.Example itShould="prevent continuous triggering of onLayout during window drag.">
        <DragingWindowExample />
      </TestCase.Example>
    </TestSuite>
  );
}

const DragingWindowExample = () => {
  const [time, setTime] = useState(0);
  return (
    <View
      style={{width: '100%', height: 20}}
      onLayout={() => {
        setTime(prev => prev + 1);
      }}>
      <Text>onLayout: {time}</Text>
    </View>
  );
};

const styles = StyleSheet.create({
  text: {
    fontSize: 14,
  },
});
