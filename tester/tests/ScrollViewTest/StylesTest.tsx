/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {View, ScrollView, StyleSheet} from 'react-native';
import {TestSuite, TestCase} from '@rnoh/testerino';
import {COMMON_PROPS} from './fixtures';

export function StylesTest() {
  return (
    <TestSuite name="styles">
      <TestCase
        modal
        itShould="render scroll view with different border radii (topLeft, topRight, ...)">
        <View style={styles.wrapperView}>
          <ScrollView
            {...COMMON_PROPS}
            style={{
              borderColor: 'firebrick',
              backgroundColor: 'beige',
              borderWidth: 3,
              borderTopLeftRadius: 10,
              borderTopRightRadius: 20,
              borderBottomRightRadius: 30,
              borderBottomLeftRadius: 40,
            }}
          />
        </View>
      </TestCase>
      <TestCase
        modal
        itShould="render scroll view with different border widths (left, right, top, bottom)">
        <View style={styles.wrapperView}>
          <ScrollView
            {...COMMON_PROPS}
            style={{
              borderColor: 'firebrick',
              backgroundColor: 'beige',
              borderLeftWidth: 3,
              borderTopWidth: 6,
              borderRightWidth: 9,
              borderBottomWidth: 12,
            }}
          />
        </View>
      </TestCase>
      <TestCase
        modal
        itShould="render scroll view with different border colors (left, right, top, bottom)">
        <View style={styles.wrapperView}>
          <ScrollView
            {...COMMON_PROPS}
            style={{
              backgroundColor: 'beige',
              borderWidth: 3,
              borderLeftColor: 'firebrick',
              borderTopColor: 'chocolate',
              borderRightColor: 'black',
              borderBottomColor: 'blue',
            }}
          />
        </View>
      </TestCase>
      <TestCase
        modal
        itShould="render  scroll view with different border radii (start, end)">
        <View style={styles.wrapperView}>
          <ScrollView
            {...COMMON_PROPS}
            style={{
              borderColor: 'firebrick',
              backgroundColor: 'beige',
              borderWidth: 3,
              borderTopStartRadius: 10,
              borderTopEndRadius: 20,
              borderBottomEndRadius: 30,
              borderBottomStartRadius: 40,
            }}
          />
        </View>
      </TestCase>
      <TestCase
        modal
        itShould="render scroll view with different border widths (start, end)">
        <View style={styles.wrapperView}>
          <ScrollView
            {...COMMON_PROPS}
            style={{
              borderColor: 'firebrick',
              backgroundColor: 'beige',
              borderStartWidth: 3,
              borderTopWidth: 6,
              borderEndWidth: 9,
              borderBottomWidth: 12,
            }}
          />
        </View>
      </TestCase>
      <TestCase
        modal
        itShould="render scroll view with different border colors (start, end)">
        <View style={styles.wrapperView}>
          <ScrollView
            {...COMMON_PROPS}
            style={{
              backgroundColor: 'beige',
              borderWidth: 3,
              borderStartColor: 'firebrick',
              borderEndColor: 'black',
              borderTopColor: 'chocolate',
              borderBottomColor: 'blue',
            }}
          />
        </View>
      </TestCase>
    </TestSuite>
  );
}

const styles = StyleSheet.create({
  wrapperView: {
    height: 300,
    width: '60%',
  },
});
