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
  VirtualizedList,
  StyleSheet,
  Text,
  StatusBar,
} from 'react-native';

type ItemData = {
  id: string;
  title: string;
};

const getItem = (_data: unknown, index: number): ItemData => ({
  id: Math.random().toString(12).substring(0),
  title: `Item ${index + 1}`,
});

const getItemCount = (_data: unknown) => 1000;

type ItemProps = {
  title: string;
};

const Item = ({ title }: ItemProps) => (
  <View style={styles.item}>
    <Text style={styles.title}>{title}</Text>
  </View>
);

export function SimpleVirtualistExample() {
  return (
    <SafeAreaView style={styles.container}>
      <View style={styles.wrapper}>
        <VirtualizedList
          horizontal={true}
          removeClippedSubviews={true}
          initialNumToRender={4}
          renderItem={({ item }) => <Item title={item.title} />}
          keyExtractor={item => item.id}
          getItemCount={getItemCount}
          getItem={getItem}
        />
      </View>
      <VirtualizedList
        removeClippedSubviews={true}
        initialNumToRender={4}
        renderItem={({ item }) => <Item title={item.title} />}
        keyExtractor={item => item.id}
        getItemCount={getItemCount}
        getItem={getItem}
      />
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    marginTop: StatusBar.currentHeight,
  },
  item: {
    backgroundColor: '#f9c2ff',
    justifyContent: 'center',
    marginVertical: 8,
    marginHorizontal: 16,
    padding: 20,
  },
  title: {
    fontSize: 24,
  },
  wrapper: {
    height: 200,
  }
});