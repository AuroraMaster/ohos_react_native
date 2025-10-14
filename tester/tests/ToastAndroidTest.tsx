/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {View, ToastAndroid, StyleSheet} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../components';

export const ToastAndroidTest = () => {
  return (
    <TestSuite name="ToastAndroid">
      <TestCase.Logical
        itShould="have constants defined"
        fn={({expect}) => {
          expect(ToastAndroid.BOTTOM).to.not.be.undefined;
          expect(ToastAndroid.CENTER).to.not.be.undefined;
          expect(ToastAndroid.TOP).to.not.be.undefined;
          expect(ToastAndroid.SHORT).to.not.be.undefined;
          expect(ToastAndroid.LONG).to.not.be.undefined;
        }}
      />
      <TestCase.Example itShould="show Toast with message (show)">
        <ToastExample options="basic" />
      </TestCase.Example>
      <TestCase.Example
        skip={'`showWithGravity` fallbacks to `show` on Harmony and Android ^R'}
        itShould="show Toast with message (showWithGravity)">
        <ToastExample options="withGravity" />
      </TestCase.Example>
      <TestCase.Example
        skip={
          '`showWithGravityAndOffset` fallbacks to `show` on Harmony and Android ^R'
        }
        itShould="show Toast with message (showWithGravityAndOffset)">
        <ToastExample options="withGravityAndOffset" />
      </TestCase.Example>
      <TestCase.Example itShould="show two toasts one short, one long">
        <ToastExample
          options="basic"
          message="I am a SHORT toast!"
          duration={ToastAndroid.SHORT}
        />
        <ToastExample
          options="basic"
          message="I am a LONG toast!"
          duration={ToastAndroid.LONG}
        />
      </TestCase.Example>
    </TestSuite>
  );
};

type ToastOptions = 'basic' | 'withGravity' | 'withGravityAndOffset';

const ToastExample = (props: {
  options: ToastOptions;
  duration?: number;
  message?: string;
}) => {
  const show = (options: ToastOptions) => {
    if (options === 'basic') {
      ToastAndroid.show(
        props.message ?? 'I am a Toast!',
        props.duration ?? 1000,
      );
    } else if (options === 'withGravity') {
      ToastAndroid.showWithGravity(
        'I am a Toast with gravity!',
        1000,
        ToastAndroid.TOP,
      );
    } else {
      ToastAndroid.showWithGravityAndOffset(
        'I am a Toast with gravity and offset!',
        100,
        ToastAndroid.BOTTOM,
        500,
        150,
      );
    }
  };
  return (
    <View style={styles.container}>
      <Button label="Toggle Toast" onPress={() => show(props.options)} />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    height: 100,
    justifyContent: 'center',
    alignItems: 'center',
  },
});
