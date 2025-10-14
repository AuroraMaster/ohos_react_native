/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestCase, TestSuite} from '@rnoh/testerino';
import {
  Platform,
  ScrollView,
  StyleSheet,
  Text,
  View,
  SectionList,
} from 'react-native';
import {getScrollViewContent} from './fixtures';
import React from 'react';

export function StickyHeadersTest() {
  return (
    <TestSuite
      name="sticky headers" /* (sticky headers fail on Android when Fabric is enabled) */
    >
      <TestCase
        modal
        itShould="stick item 1 and 4 (stickyHeaderIndices)"
        skip={Platform.OS === 'android'}>
        <View style={styles.wrapperView}>
          <ScrollView stickyHeaderIndices={[0, 3]} nestedScrollEnabled>
            {getScrollViewContent({})}
          </ScrollView>
        </View>
      </TestCase>
      <TestCase
        modal
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
        modal
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
      <TestCase
        modal
        itShould="display Text 'custom sticky header' in the place of components 1 and 4 (shouldn't stick) (StickyHeaderComponent)">
        <View style={styles.wrapperView}>
          <ScrollView
            stickyHeaderIndices={[0, 3]}
            nestedScrollEnabled
            StickyHeaderComponent={CustomStickyHeader}>
            {getScrollViewContent({})}
          </ScrollView>
        </View>
      </TestCase>
      <TestCase
        modal
        itShould="make the 'Main dishes' header stick to the top when the foldable phone is expanded or folded">
        <View style={styles.wrapperView}>
          <SectionList
            sections={DATA}
            stickySectionHeadersEnabled={true}
            keyExtractor={(item, index) => item + index}
            renderItem={() => (
              <View style={styles.item}>
                <Text style={{fontSize: 16}}>888888888888888888888888</Text>
              </View>
            )}
            renderSectionHeader={({section: {title}}) => (
              <Text style={[{backgroundColor: 'white'}, styles.title]}>
                {title}
              </Text>
            )}
          />
        </View>
      </TestCase>
    </TestSuite>
  );
}

const CustomStickyHeader = React.forwardRef(() => (
  <Text>custom sticky header</Text>
));

interface SectionData {
  id: string;
  title: string;
  data: string[];
}

const DATA: SectionData[] = [
  {
    id: '0',
    title: 'Main dishes',
    data: [...Array(10).fill('1')],
  },
];

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
  item: {
    backgroundColor: '#f9c2ff',
    padding: 20,
    marginVertical: 8,
    marginHorizontal: 16,
  },
  title: {
    fontSize: 22,
    height: 40,
  },
});
