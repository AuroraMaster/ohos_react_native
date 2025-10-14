/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TestCase, TestSuite } from '@rnoh/testerino';
import {
  COMMON_PROPS,
  getScrollViewContent,
  getScrollViewContentHorizontal,
} from './fixtures';
import { ScrollView, StyleSheet, View } from 'react-native';
import React from 'react';
import { Button } from '../../components';

export function ScrollBarsTest() {
  return (
    <TestSuite name="scroll indicators / scrollbar">
      <TestCase modal itShould="have persistent scrollbar">
        <View style={styles.wrapperView}>
          <ScrollView persistentScrollbar={true} {...COMMON_PROPS} />
        </View>
      </TestCase>
      <TestCase modal itShould="shows white vertical scroll indicator">
        <View style={styles.wrapperView}>
          <ScrollView {...COMMON_PROPS} indicatorStyle={'white'} />
        </View>
      </TestCase>
      <TestCase modal itShould="show vertical scroll indicator">
        <View style={styles.wrapperView}>
          <ScrollView {...COMMON_PROPS} showsVerticalScrollIndicator={true} />
        </View>
      </TestCase>
      <TestCase modal itShould="hide vertical scroll indicator">
        <View style={styles.wrapperView}>
          <ScrollView showsVerticalScrollIndicator={false} {...COMMON_PROPS} />
        </View>
      </TestCase>
      <TestCase modal itShould="show horizontal scroll indicator">
        <View style={styles.wrapperView}>
          <ScrollView
            showsHorizontalScrollIndicator={true}
            horizontal
            {...COMMON_PROPS}>
            {getScrollViewContentHorizontal({})}
          </ScrollView>
        </View>
      </TestCase>
      <TestCase modal itShould="hide horizontal scroll indicator">
        <View style={styles.wrapperView}>
          <ScrollView
            showsHorizontalScrollIndicator={false}
            horizontal
            {...COMMON_PROPS}>
            {getScrollViewContentHorizontal({})}
          </ScrollView>
        </View>
      </TestCase>
      <TestCase modal itShould="flash scroll indicators">
        <FlashIndicatorsTest />
      </TestCase>
      <TestCase
        modal
        skip
        itShould="[FAILS on Harmony/Android] It should move the scroll bar by 50px from the right and by 200px from the top and bottom">
        <ScrollViewScrollIndicatorInsetsTest />
      </TestCase>
      <TestCase
        modal
        skip
        itShould="[FAILS on Harmony/Android] It should move the scroll bar and the scroll content by 50px from the left and by 100px from the left and right">
        <ScrollViewContentInsetsTest />
      </TestCase>
    </TestSuite>
  );
}

function FlashIndicatorsTest() {
  const scrollRef = React.useRef<ScrollView>(null);
  return (
    <View style={styles.wrapperView}>
      <Button
        label={'Flash Indicators'}
        onPress={() => {
          scrollRef.current?.flashScrollIndicators();
        }}
      />
      <ScrollView
        style={{ flex: 1 }}
        scrollEnabled={true}
        showsVerticalScrollIndicator={false}
        ref={scrollRef}>
        {getScrollViewContent({})}
      </ScrollView>
    </View>
  );
}

function ScrollViewScrollIndicatorInsetsTest() {
  return (
    <View style={styles.wrapperView}>
      <ScrollView
        style={{
          ...styles.wrapperView,
        }}
        scrollIndicatorInsets={{ right: 50, bottom: 200, top: 200 }}>
        {getScrollViewContent({})}
      </ScrollView>
    </View>
  );
}

function ScrollViewContentInsetsTest() {
  return (
    <View style={styles.wrapperView}>
      <ScrollView
        style={{
          ...styles.wrapperView,
        }}
        contentInset={{ left: 50, bottom: 100, top: 100 }}
        scrollIndicatorInsets={{ left: 50, bottom: 100, top: 100 }}>
        {getScrollViewContent({})}
      </ScrollView>
    </View>
  );
}

const styles = StyleSheet.create({
  wrapperView: {
    height: 300,
    width: '60%',
  },
});
