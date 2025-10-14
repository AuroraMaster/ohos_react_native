/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import { Pressable, Text, StyleSheet } from 'react-native';

const GoodsButton = ({ onPress, buttonText }) => {
  return (
    <Pressable
      style={styles.textContainer}
      hitSlop={{ top: 10, bottom: 10, left: 10, right: 10 }}
      onPress={onPress}>
      <Text style={styles.titleTextFont}>{buttonText}</Text>
    </Pressable>
  );
};

const styles = StyleSheet.create({
  textContainer: {
    justifyContent: 'center',
    backgroundColor: 'white',
    width: '90%',
    marginVertical: 5,
    marginHorizontal: 50,
    borderRadius: 10,
    padding: 15,
  },
  titleTextFont: {
    color: 'grey',
    fontWeight: '300',
    fontSize: 18,
    textAlign: 'left',
  },
});

export default GoodsButton;
