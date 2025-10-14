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
  ScrollView,
  Text,
} from 'react-native';

const DATA: never[] = [];

const getItem = (data: any, index: number) => {
  return {
    id: Math.random().toString(12).substring(0),
    title: `Item ${index + 1}`,
    content: `content`,
    key: `key ${index + 1}`
  }
}
const getItemCount = (data: any) => {
  return 1000;
}

const Item = ({ title, content }) => {
  return (
    <View style={styles.item}>
      <Text style={styles.title}>{title}</Text>
      <View>
        <Text style={styles.content}>{content}</Text>
        <ScrollView
          style={styles.scrollView}
          horizontal={true}>
          <Text style={styles.text}>
            Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
            eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad
            minim veniam, quis nostrud exercitation ullamco laboris nisi ut
            aliquip ex ea commodo consequat. Duis aute irure dolor in
            reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla
            pariatur. Excepteur sint occaecat cupidatat non proident, sunt in
            culpa qui officia deserunt mollit anim id est laborum.
          </Text>
        </ScrollView>
      </View>
    </View>
  );
}

export function NestedVirtualizedListExample() {
  return (
    <SafeAreaView style={styles.container}>
      <VirtualizedList
        data={DATA}
        initialNumToRender={4}
        renderItem={({ item }) =>
          <Item
            title={item.title}
            content={item.content} />
        }
        keyExtractor={item => item.key}
        getItemCount={getItemCount}
        getItem={getItem}
        removeClippedSubviews={true}
      />
    </SafeAreaView>
  );
}
const styles = StyleSheet.create({
  container: {
    flex: 1,
    marginTop: 8,
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
  content: {
    fontSize: 14,
    backgroundColor: 'pink',
  },
  scrollView: {
    backgroundColor: 'pink',
    marginVertical: 8,
    height: 50,
  },
  text: {
    marginLeft: 8,
    fontSize: 11,
  },
  flatItem: {
    backgroundColor: 'pink',
    marginLeft: 8,
  },
});