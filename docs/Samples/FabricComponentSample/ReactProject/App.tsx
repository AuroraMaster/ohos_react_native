/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */


import React, { useRef } from 'react';
import { StyleSheet, Text, View, Pressable, UIManager, findNodeHandle, Dimensions } from 'react-native';
import QDGestureFloat from 'fabric-component-sample-package/src/specs/v2/QDGestureFloatNativeComponent';
import SelectBoxApp from './src/SelectBoxApp'

const ScreenHeight = Dimensions.get('window').height;

// 测试用例
const App = () => {
  const floatRef = useRef(null)
  return (
    <View style={styles.container}>
      <QDGestureFloat
        ref={floatRef}
        style={styles.container}
        stopPercent={0.5}
        stopPercentMax={0.75}
        onScroll={(event) => {
          console.log(event.nativeEvent.offsetY)
        }}
      >
        <View style={{ width: '100%', height: 1000, backgroundColor: 'yellow' }}>
          <Pressable onPress={() => {
            if (floatRef.current) {
              // RN向原生发送消息
              UIManager.dispatchViewManagerCommand(
                findNodeHandle(floatRef.current),
                'scrollTo',
                [ScreenHeight, true]
              );
            }
          }}>
            <Text style={{ fontSize: 50, color: 'red' }}>点我滚到顶</Text>
          </Pressable>
          <SelectBoxApp />
        </View>
      </QDGestureFloat>
    </View>
  )
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  title: {
    backgroundColor: 'green',
    color: 'white',
    fontSize: 20,
    padding: 8,
    textAlign: 'center'
  }
});

export default App;
