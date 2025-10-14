/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {useRef} from 'react';
import {ScrollView, Animated, View, Text} from 'react-native';

export const ScrollToEndOnLayoutExample = () => {
  const ref = useRef<ScrollView>(null);
  return (
    <Animated.ScrollView
      ref={ref}
      pagingEnabled={false}
      horizontal={true}
      showsHorizontalScrollIndicator={false}
      onLayout={() => {
        ref.current?.scrollToEnd({animated: true});
      }}
      style={[{backgroundColor: 'pink'}, {width: '100%'}]}>
      <View style={{paddingTop: 50}}>
        <Text style={{height: 70, fontSize: 30}}>
          1234567891234567812345678123456789123456781234567scroll end
        </Text>
      </View>
    </Animated.ScrollView>
  );
};
