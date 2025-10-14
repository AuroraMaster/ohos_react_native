/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {
  SafeAreaView,
  View,
  FlatList,
  StyleSheet,
  Text,
  StatusBar,
} from 'react-native';

let DATA = [];

const dataLength = 1000;

for (let index = 0; index < dataLength; index++) {
  DATA.push({
    id: index, title: `Item ${index}`,
  })
}

const Item = ({ title }) => (
  <View style={styles.item}>
    <Text style={styles.title}>{title}</Text>
  </View>
);

export function SimpleFlatListExample() {
  return (
    <SafeAreaView style={styles.container}>
      <View style={styles.wrapper}>
        <FlatList
        removeClippedSubviews={true}
        data={DATA}
        renderItem={({ item }) => <Item title={item.title} />}
        keyExtractor={item => item.id}
        refreshing
        horizontal={true}
      />
      </View>
      
      <FlatList
        removeClippedSubviews={true}
        data={DATA}
        renderItem={({ item }) => <Item title={item.title} />}
        keyExtractor={item => item.id}
        refreshing
      />
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    marginTop: StatusBar.currentHeight || 0,
  },
  item: {
    backgroundColor: '#f9c2ff',
    padding: 20,
    marginVertical: 8,
    marginHorizontal: 16,
  },
  title: {
    fontSize: 32,
  },
  wrapper: {
    height: 200,
  }
});