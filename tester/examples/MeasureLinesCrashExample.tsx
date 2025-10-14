/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

export const MeasureLinesCrashExample = () => {
  return (
    <View style={styles.container}>
      <Text style = {{width: 80}}
       onTextLayout = {(e) => {
        console.log(e.nativeEvent.lines);
       }}>
        <Text>
          <Text>👍 👍</Text>
          <View style = {{width: 5}}></View>
          <View style = {{width: 5}}></View>
          <View style = {{width: 5}}></View>
        </Text>
        <Text>
          <Text>👍 👍</Text>
          <View style = {{width: 5}}></View>
        </Text>
        <Text>
          <Text>👍 👍</Text>
          <View style = {{width: 5}}></View>
        </Text>
      </Text>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
});