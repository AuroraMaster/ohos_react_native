/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TestCase, TestSuite } from '@rnoh/testerino';
import { Platform, ScrollView, StyleSheet, Text, View } from 'react-native';
import { getScrollViewContent } from './fixtures';

export function StickyHeadersTest() {
  return (
    <TestSuite
      name="sticky headers" /* (sticky headers fail on Android when Fabric is enabled) */
    >
      <TestCase
        itShould="stick item 1 and 4 (stickyHeaderIndices)"
        skip={Platform.OS === 'android'}>
        <View style={styles.wrapperView}>
          <ScrollView stickyHeaderIndices={[0, 3]} nestedScrollEnabled>
            {getScrollViewContent({})}
          </ScrollView>
        </View>
      </TestCase>
      <TestCase
        skip={Platform.OS === 'android'}
        itShould="hide sticked item 1 or 4 when scrolling down (stickyHeaderHiddenOnScroll)">
        <View style={styles.wrapperView}>
          <ScrollView
            stickyHeaderIndices={[0, 3]}
            nestedScrollEnabled
            stickyHeaderHiddenOnScroll>
            {getScrollViewContent({})}
          </ScrollView>
        </View>
      </TestCase>
      <TestCase
        skip={Platform.OS === 'android'}
        itShould="stick item 13 and 20 to the bottom (invertStickyHeaders)"
      //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/309
      >
        <View style={styles.wrapperView}>
          <ScrollView
            stickyHeaderIndices={[12, 19]}
            nestedScrollEnabled
            invertStickyHeaders>
            {getScrollViewContent({})}
          </ScrollView>
        </View>
      </TestCase>
      <TestCase itShould="display Text 'custom sticky header' in the place of components 1 and 4 (shouldn't stick) (StickyHeaderComponent)">
        <View style={styles.wrapperView}>
          <ScrollView
            stickyHeaderIndices={[0, 3]}
            nestedScrollEnabled
            StickyHeaderComponent={() => <Text>custom sticky header</Text>}>
            {getScrollViewContent({})}
          </ScrollView>
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
  button: {
    width: 160,
    height: 36,
    backgroundColor: 'hsl(190, 50%, 70%)',
    paddingHorizontal: 16,
    paddingVertical: 8,
    borderRadius: 8,
  },
});
