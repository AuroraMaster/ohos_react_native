/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Text, StyleSheet, View, Animated, Pressable } from 'react-native';
import React, { useEffect, useRef } from 'react';
import SampleTurboModule2, { ResultModel } from '../basic/SampleTurboModule2';

function AppDetails() {
  const zIndexAnim = useRef(new Animated.Value(0)).current;
  const opacityAnim = useRef(new Animated.Value(0)).current;

  useEffect(() => {
    startAnimation();
  }, []);

  const startAnimation = () => {
    Animated.parallel([
      Animated.timing(opacityAnim, {
        toValue: 1,
        duration: 300,
        useNativeDriver: false,
      }),
    ]).start();
  };

  const opacityInterpolation = opacityAnim.interpolate({
    inputRange: [0, 1],
    outputRange: [0, 1],
  });

  return (
    <View style={styles.container}>
      <Animated.View
        style={[
          styles.textContainer,
          styles.box1,
          { opacity: opacityInterpolation },
        ]}>
        <Text style={styles.apiFontSize}>容器1和文字由RN渲染</Text>
      </Animated.View>
      <Animated.View
        style={[
          styles.textContainer,
          styles.box2,
          { opacity: opacityInterpolation },
        ]}>
        <Text style={styles.apiFontSize}>容器2和文字由RN渲染</Text>
      </Animated.View>
      <Pressable
        style={{ height: 200 }}
        onPress={() => {
          SampleTurboModule2.getRequest()
            .then((result: ResultModel) => {
              console.log(JSON.stringify(result));
              console.log(result.result.note);
            })
            .catch(err => {
              console.log(JSON.stringify(err));
            });
        }}>
        <Text style={[styles.apiFontSize, { backgroundColor: '#FF8080' }]}>
          点击获取网络请求数据
        </Text>
      </Pressable>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    marginTop: 15,
  },
  textContainer: {
    position: 'relative',
    height: 100,
    borderRadius: 10,
    justifyContent: 'center',
    alignItems: 'center',
  },
  box1: {
    backgroundColor: '#31525B',
  },
  box2: {
    backgroundColor: '#B3DEE5',
    zIndex: 100,
    top: -20,
    left: 20,
  },
  apiFontSize: {
    height: 100,
    fontSize: 30,
    color: 'white',
    textAlign: 'center',
    textAlignVertical: 'center',
  },
});

export default AppDetails;
