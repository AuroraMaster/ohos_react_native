/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  Animated,
  PanResponder,
  RefreshControl,
  ScrollView,
  StyleSheet,
  View,
} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {useRef} from 'react';
import {TestCase} from '../components';
import React from 'react';

export const PanResponderTest = () => {
  return (
    <TestSuite name="PanResponder">
      <TestCase.Logical
        itShould="create PanResponder"
        fn={({expect}) => {
          expect(PanResponder.create({})).to.be.not.empty;
        }}
      />
      <TestCase.Example modal itShould="allow panning inside ScrollView">
        <PanResponderInScrollView />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="allow panning inside ScrollView with refreshControl">
        <PanResponderInScrollViewWithRefresh />
      </TestCase.Example>
    </TestSuite>
  );
};

const PanResponderInScrollView = () => {
  const pan = useRef(new Animated.ValueXY()).current;

  const panResponder = useRef(
    PanResponder.create({
      onStartShouldSetPanResponder: () => {
        return true;
      },
      onMoveShouldSetPanResponder: () => true,
      onPanResponderMove: Animated.event([null, {dx: pan.x, dy: pan.y}], {
        useNativeDriver: false,
      }),
      onPanResponderRelease: () => {
        pan.extractOffset();
      },
      onShouldBlockNativeResponder: () => {
        return true;
      },
    }),
  ).current;

  return (
    <ScrollView style={styles.scrollview} horizontal pagingEnabled>
      <View style={[styles.base, styles.view1]}>
        <Animated.View
          style={{
            transform: [{translateX: pan.x}, {translateY: pan.y}],
          }}
          {...panResponder.panHandlers}>
          <View style={styles.box} />
        </Animated.View>
      </View>
      <View style={[styles.base, styles.view2]} />
      <View style={[styles.base, styles.view1]} />
      <View style={[styles.base, styles.view2]} />
    </ScrollView>
  );
};

const PanResponderInScrollViewWithRefresh = () => {
  const pan = useRef(new Animated.ValueXY()).current;

  const [refreshing, setRefreshing] = React.useState(false);

  const onRefresh = React.useCallback(() => {
    setRefreshing(true);
    setTimeout(() => {
      setRefreshing(false);
    }, 2000);
  }, []);

  const panResponder = useRef(
    PanResponder.create({
      onStartShouldSetPanResponder: () => {
        return true;
      },
      onMoveShouldSetPanResponder: () => true,
      onPanResponderMove: Animated.event([null, {dx: pan.x, dy: pan.y}], {
        useNativeDriver: false,
      }),
      onPanResponderRelease: () => {
        pan.extractOffset();
      },
      onShouldBlockNativeResponder: () => {
        return true;
      },
    }),
  ).current;

  return (
    <ScrollView
      style={styles.scrollview}
      refreshControl={
        <RefreshControl refreshing={refreshing} onRefresh={onRefresh} />
      }
      pagingEnabled>
      <View style={[styles.base, styles.view1]}>
        <Animated.View
          style={{
            transform: [{translateX: pan.x}, {translateY: pan.y}],
          }}
          {...panResponder.panHandlers}>
          <View style={styles.box} />
        </Animated.View>
      </View>
      <View style={[styles.base, styles.view2]} />
      <View style={[styles.base, styles.view1]} />
      <View style={[styles.base, styles.view2]} />
    </ScrollView>
  );
};

const styles = StyleSheet.create({
  view1: {
    backgroundColor: 'pink',
  },
  view2: {
    backgroundColor: 'powderblue',
  },
  base: {
    height: 400,
    width: 300,
  },
  scrollview: {
    borderWidth: 2,
    borderColor: 'black',
    height: 400,
    width: 300,
  },
  box: {
    height: 100,
    width: 100,
    backgroundColor: 'blue',
    borderRadius: 5,
  },
});
