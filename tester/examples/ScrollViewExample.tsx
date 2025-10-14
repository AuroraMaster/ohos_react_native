/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useRef, useState} from 'react';
import {
  Animated,
  View,
  ScrollView,
  Alert,
  StyleSheet,
  PanResponder,
  Text,
  NativeSyntheticEvent,
  NativeScrollEvent,
  ActivityIndicator,
  Pressable,
} from 'react-native';

const handleClick = () => {
  Alert.alert('Handle Click Event');
};

export const GesturesInScrollViewExample = () => {
  const pan = useRef(new Animated.ValueXY()).current;
  const panResponder = useRef(
    PanResponder.create({
      onMoveShouldSetPanResponder: () => true,
      onPanResponderMove: Animated.event([null, {dx: pan.x, dy: pan.y}], {
        useNativeDriver: false,
      }),
      onPanResponderRelease: () => {
        pan.extractOffset();
      },
    }),
  ).current;

  return (
    <ScrollView
      horizontal={true}
      overScrollMode={'never'}
      style={{
        backgroundColor: 'green'
      }}>
      <View style={{width: 250, height: 200, backgroundColor: 'pink', marginRight: 5}}>
        <Text style={{textAlign: 'center'}} onPress={handleClick}>
          Clicking will trigger a pop-up, while horizontal swiping will not.
        </Text>
        <Animated.View
          style={{
            transform: [{translateX: pan.x}, {translateY: pan.y}],
          }}
          {...panResponder.panHandlers}>
          <View style={styles.box} >
            <Text style={styles.boxText}>You can drag me</Text>
          </View>
        </Animated.View>
      </View>
    </ScrollView>
  );
};

export const ScrollViewNestedInGesturesExample = () => {
  const [refreshing, setRefreshing] = useState(false);
  const [time, setTime] = useState(0);
  const scrollOffsetY = useRef(0);
  const headerHeight = 80;
  const pan = useRef(new Animated.Value(0)).current;
  const panResponder = useRef(
    PanResponder.create({
      onMoveShouldSetPanResponder: (_, gestureState) => {
        return scrollOffsetY.current <= 0 && gestureState.dy >= 0;
      },
      onPanResponderMove: (_, gestureState) => {
        if (!refreshing) {
          const dy = Math.min(gestureState.dy * 0.5, headerHeight * 2.5);
          pan.setValue(dy);
        }
      },
      onPanResponderRelease: (_, gestureState) => {
        if (gestureState.dy > headerHeight && !refreshing) {
          handleRefresh();
        }
        resetAnimation();
      },
      onPanResponderTerminate: () => resetAnimation(),
    })
  ).current;

  const handleRefresh = async (): Promise<void> => {
    if (refreshing) return;
    setRefreshing(true);
    setTimeout(() => {
      setRefreshing(false);
    }, 1000);
  };

  const resetAnimation = (): void => {
    Animated.spring(pan, {
      toValue: 0,
      friction: 8,
      useNativeDriver: true,
    }).start(() => {
      pan.setValue(0);
    });
  };

  const rotate = pan.interpolate({
    inputRange: [0, headerHeight * 2.5],
    outputRange: ['0deg', '180deg'],
  });

  const handleScroll = (event: NativeSyntheticEvent<NativeScrollEvent>): void => {
    scrollOffsetY.current = event.nativeEvent.contentOffset.y;
  }

  return (
    <View style={styles.container}>
      <Animated.View
        style={[
          styles.header,
          {
            height: headerHeight,
            transform: [{ translateY: pan }],
          },
        ]}>
        <Animated.View
          style={{ transform: [{ rotate }], backgroundColor: 'red' }}>
          {refreshing ? (
            <ActivityIndicator size="large" color="#000" />
          ) : (
            <Text>{`⬇️ Pull down to refresh`}</Text>)}
        </Animated.View>
      </Animated.View>
      <Animated.View
        pointerEvents={'box-none'}
        style={{
          transform: [{ translateY: pan }],
          backgroundColor: 'yellow',
        }}
        {...panResponder.panHandlers}>
          <Animated.ScrollView
            onScroll={handleScroll}
            onTouchStart={() => {
              setTime(time + 1)
            }}
            overScrollMode={'never'}
          >
            <Pressable style={{ backgroundColor: 'lightgreen', height: 1000, alignItems: 'center', justifyContent: 'center' }}>
              <Text>onTouchStart: {time}</Text>
            </Pressable>
          </Animated.ScrollView>
      </Animated.View>
    </View>
  );
}

const styles = StyleSheet.create({
  box: {
    height: 150,
    width: 150,
    backgroundColor: 'blue',
    borderRadius: 5,
  },
  boxText: {
    textAlign: 'center',
    color: 'white',
    marginTop: 60,
    fontSize: 16,
  },
  container: { flex: 1, backgroundColor: '#fff' },
  header: {
    position: 'absolute',
    left: 0,
    right: 0,
    justifyContent: 'center',
    alignItems: 'center',
    zIndex: 1,
  },
});

