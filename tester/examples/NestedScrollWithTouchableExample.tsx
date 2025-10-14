/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useState, useEffect} from 'react';
import {
  View,
  Text,
  ScrollView,
  StyleSheet,
  TouchableOpacity,
} from 'react-native';

const COMMON_PROPS = {
  style: {
    borderWidth: 3,
    borderColor: 'firebrick',
    height: 200,
  },
  contentContainerStyle: {
    alignItems: 'center' as 'center',
    justifyContent: 'center' as 'center',
  },
  children: getScrollViewContent(),
};

export const NestedScrollWithTouchableExample = () => {
  const [num, setNum] = useState(1);
  let scrollToOffset = 0;
  const scrollRef1 = React.useRef<ScrollView>(null);

  useEffect(() => {
    const intervalId = setInterval(() => {
      console.log(`RNOH: lhy Scroll is begin ${scrollToOffset}`);
      scrollToOffset = (scrollToOffset + 50) % 600;
      scrollRef1.current?.scrollTo({x: scrollToOffset, animated: true});
    }, 2000);
    return () => clearInterval(intervalId);
  }, []);

  return (
    <View style={styles.wrapperView}>
      <ScrollView>
        <ScrollView ref={scrollRef1} horizontal={true} {...COMMON_PROPS} />
        <TouchableOpacity
          style={{
            paddingVertical: 6,
            paddingHorizontal: 12,
            backgroundColor: 'hsl(210,80%,50%)',
            borderWidth: 2,
            borderColor: 'hsl(210,50%,35%)',
            position: 'relative',
          }}
          onPress={() => {
            setNum(num + 1);
          }}>
          <Text style={{fontWeight: 'bold', fontSize: 30}}>
            TestTouch={num}
          </Text>
        </TouchableOpacity>
        <View style={{width: '100%', height: 100, backgroundColor: 'red'}} />
        <View style={{width: '100%', height: 100, backgroundColor: 'blue'}} />
        <View style={{width: '100%', height: 100, backgroundColor: 'red'}} />
        <View style={{width: '100%', height: 100, backgroundColor: 'blue'}} />
        <View style={{width: '100%', height: 100, backgroundColor: 'red'}} />
        <View style={{width: '100%', height: 100, backgroundColor: 'blue'}} />
      </ScrollView>
    </View>
  );
};

const styles = StyleSheet.create({
  wrapperView: {
    height: 600,
  },
});

function getScrollViewContent() {
  const amountOfChildren = 20;
  return new Array(amountOfChildren).fill(0).map((_, idx) => {
    return (
      <View
        key={idx}
        style={[
          {
            width: 50,
            height: '100%',
            backgroundColor: idx % 2 ? 'pink' : 'beige',
            justifyContent: 'center',
          },
        ]}>
        <Text style={{textAlign: 'center', height: 15}}>{idx + 1}</Text>
      </View>
    );
  });
}
