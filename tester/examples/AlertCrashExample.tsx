/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import { View, Button, Alert, StyleSheet } from 'react-native';

export const AlerCrashExample = () => {
  const showAlert = () => {
    Alert.alert(
      "Alert", 
      "This is a Alert Dialog", 
      [
        {
          text: "FistButton", 
          onPress: () => console.log("Click first button"),
          style: "cancel", 
        },
        {
          text: "SecondButton", 
          onPress: () => console.log("Click second button"),
        },
      ],
      { cancelable: true } 
    );
  };

  return (
    <View style={styles.container}>
      <Button title="showAlert" onPress={showAlert} />
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