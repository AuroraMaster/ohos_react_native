/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */


import React, { useEffect, useRef, useState } from 'react';
import { StyleSheet, Text, View, Pressable, UIManager, findNodeHandle, Dimensions } from 'react-native';
import { QDGestureFloat, SampleAnyThreadTurboModule, SampleTurboModule } from 'third-party-library';
import SelectBoxApp from './src/SelectBoxApp'

const ScreenHeight = Dimensions.get('window').height;

const App = () => {
  const floatRef = useRef(null)
  const [addResult, setAddResult] = useState(0)
  const [powerResult, setPowerResult] = useState(0)
  useEffect(() => {
    const result1 = SampleTurboModule.add(2, 4);
    setAddResult(result1);
    SampleAnyThreadTurboModule.power(2, 4).then(result2 => {
      setPowerResult(result2);
    });
  }, [])
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
            <Text style={{ fontSize: 50, color: 'red' }}>点击滚到顶</Text>
          </Pressable>
          <Text style={{ backgroundColor: 'white' }}>2 + 4 = {addResult}, 2 ^ 4 = {powerResult}</Text>
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
