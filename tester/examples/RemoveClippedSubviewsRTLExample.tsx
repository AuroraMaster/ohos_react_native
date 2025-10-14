/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {
  Button,
  I18nManager,
  SafeAreaView,
  ScrollView,
  StyleSheet,
  Text,
} from 'react-native';

const COLORS = ['red', 'blue', 'green'];

export function RemoveClippedSubviewsRTLExample() {
  const [isRTL, setRTL] = React.useState(I18nManager.isRTL);

  const toggleRTL = React.useCallback(() => {
    setRTL(v => !v);
  }, []);

  React.useEffect(() => {
    I18nManager.forceRTL(isRTL);
  }, [isRTL]);

  React.useEffect(() => {
    return () => I18nManager.forceRTL(false);
  }, []);

  return (
    <SafeAreaView style={{flex: 1}}>
      <Text>{isRTL ? 'RTL' : 'LTR'}</Text>
      <Button title="Toggle RTL" onPress={toggleRTL} />
      <ScrollView
        style={{flexGrow: 1}}
        horizontal
        removeClippedSubviews
        onScrollEndDrag={ev => {
          console.log(
            `Content offset: ${JSON.stringify(ev.nativeEvent.contentOffset)}`,
          );
        }}>
        {Array.from({length: 26}, (_v, idx) => (
          <Text
            key={idx}
            style={[
              styles.box,
              {backgroundColor: COLORS[idx % COLORS.length]},
            ]}>
            Index: {idx}
          </Text>
        ))}
      </ScrollView>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  box: {
    height: '100%',
    width: 80,
  },
});
