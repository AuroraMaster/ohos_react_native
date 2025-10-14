/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import * as React from 'react';
import { useCallback, useRef, useState } from 'react';
import HomeListPage from './HomeListPage';
import { GoodsProps } from './HomeListPage';
import CategoryButton from './CategoryButton';
import PagerView from 'react-native-pager-view';
import type { PagerViewOnPageSelectedEvent } from 'react-native-pager-view';

import { SafeAreaView, ScrollView, StyleSheet, View } from 'react-native';

function App(): JSX.Element {
  const good0: GoodsProps = {
    name: '关注A',
    onPress: () => { },
  };
  const good1: GoodsProps = {
    name: '品牌A',
    onPress: () => { },
  };
  const good2: GoodsProps = {
    name: '直播A',
    onPress: () => { },
  };
  const categories = ['关注', '推荐', '直播'];
  const goods = [
    [good0, good0, good0],
    [good1, good1, good1],
    [good2, good2, good2],
  ];
  const pagerRef = useRef<PagerView>(null);
  const [selectedIndex, setSelectedIndex] = useState(1);
  const onPageSelected = useCallback((e: PagerViewOnPageSelectedEvent) => {
    setSelectedIndex(e.nativeEvent.position);
  }, []);

  return (
    <View style={styles.container}>
      <View style={styles.topButtonContainer}>
        {categories.map((category, index) => (
          <CategoryButton
            key={index}
            label={category}
            textStyle={{
              fontSize: selectedIndex === index ? 26 : 20,
              fontWeight: selectedIndex === index ? 'bold' : 'normal',
            }}
            onPress={() => {
              setSelectedIndex(index);
              pagerRef.current?.setPage(index);
            }}
          />
        ))}
      </View>

      <PagerView
        style={styles.pagerView}
        initialPage={1}
        ref={pagerRef}
        onPageSelected={onPageSelected}>
        {categories.map((category, index) => (
          <ScrollView
            contentInsetAdjustmentBehavior="automatic"
            style={styles.scrollView}
            key={index}>
            <HomeListPage key={category} goods={goods[index]} />
          </ScrollView>
        ))}
      </PagerView>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    backgroundColor: 'hsl(210, 80%, 50%)',
    height: '100%',
  },
  topButtonContainer: {
    height: 90,
    marginTop: 10,
    flexDirection: 'row',
  },
  topButton: {
    fontSize: 20,
  },
  pagerView: {
    flex: 1,
    backgroundColor: 'black',
  },
  scrollView: {
    backgroundColor: '#E6E6E6',
  },
  highlight: {
    fontWeight: '700',
  },
});

export default App;
