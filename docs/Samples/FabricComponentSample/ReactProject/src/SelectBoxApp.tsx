/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */


import React, { useRef, useState } from 'react';
import {
  Text,
  StyleSheet,
  Pressable,
  UIManager,
  findNodeHandle,
  View,
} from 'react-native';
import SelectBox from "fabric-component-sample-package/src/specs/v1/SelectBox";

const DATA = [
  {
    id: 0,
    text: '选项1',
  },
  {
    id: 1,
    text: '选项2',
  },
  {
    id: 2,
    text: '选项3',
  },
];

const App = () => {
  const [value, setValue] = useState([])
  const [mutil, setMutil] = useState(false)
  const boxRef = useRef(null)

  return (
    <View style={styles.container}>
      <Pressable onPress={() => {
        if (boxRef.current) {
          // RN向原生发送消息
          UIManager.dispatchViewManagerCommand(
            findNodeHandle(boxRef.current),
            'emitNativeEvent',
            [!mutil]
          );
          setMutil(!mutil)
        }
      }}>
        <Text style={styles.title}>{`现在是${mutil ? '多选' : '单选'}`}</Text>
      </Pressable>
      <SelectBox
        ref={boxRef}
        style={{
          flex: 1
        }}
        value={value}
        data={DATA}
        onChange={(event: any) => {
          setValue(event.nativeEvent.value)
          console.log(event.nativeEvent)
        }}
      />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    backgroundColor: 'white',
    height: 250,
    justifyContent: 'center',
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
