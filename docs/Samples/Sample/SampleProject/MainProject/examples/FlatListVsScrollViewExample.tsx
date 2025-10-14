/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { FlatList, ScrollView, Text, View } from 'react-native';
import { randomizeColor } from '../components';
import React from 'react';

const ITEMS_COUNT = 1000;

export function FlatListVsScrollViewExample() {
  return (
    <View style={{ flex: 1, backgroundColor: '#ddd' }}>
      <Text style={{ height: 20 }}>FlatList</Text>
      <FlatList
        style={{ height: '50%' }}
        data={new Array(ITEMS_COUNT)}
        renderItem={item => {
          return <Item id={item.index} />;
        }}
      />
      <Text style={{ height: 20 }}>ScrollView</Text>
      <ScrollView style={{ width: '100%', height: '50%' }}>
        {new Array(ITEMS_COUNT).fill(0).map((_, itemId) => {
          return <Item id={itemId} key={itemId} />;
        })}
      </ScrollView>
    </View>
  );
}

function Item({ id }: { id: number }) {
  const [bgColor] = React.useState(randomizeColor());

  return (
    <View
      style={{
        width: '100%',
        height: 100,
        backgroundColor: bgColor,
      }}>
      <Text style={{ height: 24 }}>{id}</Text>
    </View>
  );
}
