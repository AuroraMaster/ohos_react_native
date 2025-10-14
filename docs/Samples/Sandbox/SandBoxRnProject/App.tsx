/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import * as React from 'react';
import { Text, View, StyleSheet, Pressable } from 'react-native';//注入组件
// import { CustomNativeComponentTest } from './views/CustomNativeComponentTest.harmony';

const App = (): React.Node => {
  return (
    <View>
      <Pressable onPress={() => {
        console.log("Hello World~")
      }}>
        <View style={styles.textContainer}>
          <Text style={styles.title}>
            Hello World~
          </Text>
          <Text style={styles.content}>
            在 VSCode 的控制台上输入d，便可看到手机上弹出了React Native Dev Menu对话框。
            开发者可通过Element Inspector查看RN元素的盒子模型及样式。
          </Text>
        </View>
      </Pressable>

      {/* <CustomNativeComponentTest></CustomNativeComponentTest> */}
    </View>
  );
};

export default App;
const styles = StyleSheet.create({//创建样式
  textContainer: {
    backgroundColor: 'white',
    height: 400,
    justifyContent: 'center',
    padding: 20,
  },
  title: {
    fontWeight: 'bold',
    textAlign: 'left',
    fontSize: 40
  },
  content: {
    marginTop: 10,
    fontSize: 16,
    textAlign: 'left'
  }
});
