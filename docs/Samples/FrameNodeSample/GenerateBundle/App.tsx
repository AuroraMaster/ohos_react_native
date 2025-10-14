/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, { useState } from 'react';
import { View, Text, FlatList, TouchableOpacity, StyleSheet } from 'react-native';

const App = () => {
  // 左侧列表的数据
  const categories = ['Category 1', 'Category 2', 'Category 3', 'Category 4'];

  // 右侧列表的数据 (基于左侧选中的分类)
  const data = {
    'Category 1': ['Item 1-1', 'Item 1-2', 'Item 1-3'],
    'Category 2': ['Item 2-1', 'Item 2-2'],
    'Category 3': ['Item 3-1', 'Item 3-2', 'Item 3-3', 'Item 3-4'],
    'Category 4': ['Item 4-1', 'Item 4-2', 'Item 4-3']
  };

  // 当前选中的分类
  const [selectedCategory, setSelectedCategory] = useState(categories[0]);

  return (
    <View style={styles.container}>
      {/* 左侧列表 */}
      <FlatList
        style={styles.leftList}
        data={categories}
        keyExtractor={(item) => item}
        renderItem={({ item }) => (
          <TouchableOpacity
            onPress={() => setSelectedCategory(item)}
            style={[
              styles.item,
              selectedCategory === item && styles.selectedItem
            ]}
          >
            <Text style={styles.itemText}>{item}</Text>
          </TouchableOpacity>
        )}
      />

      {/* 右侧列表 */}
      <FlatList
        style={styles.rightList}
        data={data[selectedCategory]}
        keyExtractor={(item) => item}
        renderItem={({ item }) => (
          <View style={styles.item}>
            <Text style={styles.itemText}>{item}</Text>
          </View>
        )}
      />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: 'row',
    padding: 10
  },
  leftList: {
    width: '40%',
    marginRight: 10
  },
  rightList: {
    width: '60%'
  },
  item: {
    padding: 20,
    borderBottomWidth: 1,
    borderBottomColor: '#ccc'
  },
  selectedItem: {
    backgroundColor: '#dcdcdc'
  },
  itemText: {
    fontSize: 16
  }
});

export default App;
