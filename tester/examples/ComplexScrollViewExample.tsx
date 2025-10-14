/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {
  StyleSheet,
  Text,
  SafeAreaView,
  ScrollView,
  StatusBar,
  Button,
  Alert,
  View,
  VirtualizedList,
  Image,
  FlatList,
  Pressable,
} from 'react-native';

let flatData = [];

const dataLength = 10;

for (let index = 0; index < dataLength; index++) {
  flatData.push({
    id: index, name: `Item ${index}`,
    uri: 'https://reactnative.dev/img/tiny_logo.png'
  })
}

const renderItem = ({ item }) => (
  <View>
    <Text>{item.name}</Text>
    <Image
      style={styles.image}
      source={{
        uri: item.uri,
      }}
    />
  </View>
);

const keyExtractor = (item) => item.id.toString();

export function ComplexScrollViewExample() {
  return (
    <ScrollView style={styles.scrollView}
      removeClippedSubviews={true}>
      <Text style={styles.text}>
        Lorem ipsum dolor sit amet, consectetur adipiscing elit 1.
      </Text>
      <Text style={styles.text}>
        Lorem ipsum dolor sit amet, consectetur adipiscing elit 2.
      </Text>
      <FlatList
        style={styles.flatList}
        data={flatData}
        renderItem={renderItem}
        keyExtractor={keyExtractor}
      />
      <Button
        title="Button"
        onPress={() => Alert.alert('Simple Button Pressed')}
      />
      <Pressable style={styles.button}
        onPress={() => Alert.alert('Custom Button Pressed')}>
        <Text style={styles.text}>{'Custom Button'}</Text>
      </Pressable>
      <ScrollView horizontal={true} style={styles.scrollViewHorizontal}
        removeClippedSubviews={true}>
        <Text style={styles.text}>
          ScrollView pariatur. Excepteur sint occaecat cupidatat non proident, sunt in
          culpa qui officia deserunt mollit anim id est laborum.
        </Text>
      </ScrollView>
      <ScrollView style={styles.scrollView}
        removeClippedSubviews={true}>
        <FlatList
          style={styles.flatList}
          data={flatData}
          renderItem={renderItem}
          keyExtractor={keyExtractor}
        />
        <Text style={styles.text}>
          ScrollView pariatur. Excepteur sint occaecat cupidatat non proident, sunt in
          culpa qui officia deserunt mollit anim id est laborum.
        </Text>
        <FlatList
          style={styles.flatList}
          horizontal={true}
          data={flatData}
          renderItem={renderItem}
          keyExtractor={keyExtractor}
        />
        <View style={styles.container}>
          <Text style={styles.text}>Scroll down</Text>
          <Text style={styles.text}>Keep scrolling</Text>
          <Text style={styles.text}>Almost there</Text>
          <Text style={styles.text}>You made it!</Text>
        </View>
        {Array.from({ length: 20 }).map((_, index) => (
          <View key={index} style={styles.item}>
            <Text style={styles.text}>Item {index + 1}</Text>
          </View>
        ))}
        <Text style={styles.text}>scrollViewHorizontal</Text>
        <ScrollView horizontal={true} style={styles.scrollViewHorizontal}
          removeClippedSubviews={true}>
          <Text style={styles.text}>
            ScrollView pariatur.
          </Text>
          <FlatList
            style={styles.flatList}
            data={flatData}
            renderItem={renderItem}
            keyExtractor={keyExtractor}
          />
          <Text style={styles.text}>
            ScrollView pariatur.
          </Text>
        </ScrollView>
        <Text style={styles.text}>FlatListHorizontal</Text>
        <FlatList
          style={styles.flatList}
          horizontal={true}
          data={flatData}
          renderItem={renderItem}
          keyExtractor={keyExtractor}
        />
        <Text style={styles.text}>FlatList</Text>
        <FlatList
          style={styles.flatList}
          data={flatData}
          renderItem={renderItem}
          keyExtractor={keyExtractor}
        />
      </ScrollView>
    </ScrollView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingTop: StatusBar.currentHeight,
  },
  scrollViewHorizontal: {
    backgroundColor: 'pink',
    marginHorizontal: 20,
    height: 500,
  },
  scrollView: {
    backgroundColor: 'rgba(255, 192, 203, 0.5)',
    marginHorizontal: 20,
    height: 800,
  },
  text: {
    fontSize: 22,
  },
  View: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  item: {
    backgroundColor: 'skyblue',
    margin: 10,
    padding: 10,
  },
  image: {
    width: 100,
    height: 100,
    margin: 10,
    backgroundColor: 'skyblue',
  },
  button: {
    alignItems: 'center',
    justifyContent: 'center',
    paddingVertical: 12,
    paddingHorizontal: 32,
    marginHorizontal: 20,
    borderRadius: 20,
    elevation: 3,
    backgroundColor: 'lightblue',
  },
  flatList: {
    backgroundColor: 'lightyellow'
  }
});