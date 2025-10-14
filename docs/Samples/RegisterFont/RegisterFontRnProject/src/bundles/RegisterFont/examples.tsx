/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import { Text, View, StyleSheet } from 'react-native';

const styles = StyleSheet.create({
  //创建样式
  text: {
    margin: 5,
  },
});

const RegisterFont = () => {
  return (
    <View
      style={{
        marginTop: 20,
        backgroundColor: '#def5ee',
        width: '90%',
        borderRadius: 10,
        marginLeft: 'auto',
        marginRight: 'auto',
      }}>
      <Text style={styles.text}>
        文字测试---默认字体
      </Text>
      <Text style={[styles.text, { fontFamily: "Pacifico-Regular" }]}>
        文字测试---Pacifico-Regular
      </Text>
      <Text style={[styles.text, { fontFamily: "StintUltraCondensed-Regular" }]}>
        文字测试---StintUltraCondensed-Regular
      </Text>
    </View>
  );
};

export default RegisterFont;
