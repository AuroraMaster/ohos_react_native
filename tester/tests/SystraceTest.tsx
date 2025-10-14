/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import React from 'react';
import {StyleSheet, Systrace, View} from 'react-native';
import {Button, TestCase} from '../components';

// Refer to docs/performace-analysis-instructions.md how to use Systrace
export function SystraceTest() {
  const traceCookie = React.useRef<number | null>(null);
  return (
    <TestSuite name="Systrace">
      <TestCase.Example itShould="Syncronous systrace tracing">
        <View style={styles.container}>
          <Button
            label="Start Systrace"
            onPress={() => {
              Systrace.beginEvent('Test trace');
            }}
          />
          <Button
            label="Stop Systrace"
            onPress={() => {
              Systrace.endEvent();
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="Asyncronous systrace tracing">
        <View style={styles.container}>
          <Button
            label="Start Async Systrace"
            onPress={() => {
              if (traceCookie.current !== null) {
                return;
              }
              traceCookie.current =
                Systrace.beginAsyncEvent('Test async trace');
            }}
          />
          <Button
            label="Stop Async Systrace"
            onPress={() => {
              if (traceCookie.current === null) {
                return;
              }
              Systrace.endAsyncEvent('Test async trace', traceCookie.current); // both label and cookie must match
              traceCookie.current = null;
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="Systrace counterEvent">
        <View style={styles.container}>
          <Button
            label="Trace variable 'MY_SUPER_VARIABLE' - set to 123"
            onPress={() => {
              Systrace.counterEvent('MY_SUPER_VARIABLE', 123);
            }}
          />
          <Button
            label="Trace variable 'MY_SUPER_VARIABLE' - set to 42"
            onPress={() => {
              Systrace.counterEvent('MY_SUPER_VARIABLE', 42);
            }}
          />
        </View>
      </TestCase.Example>
    </TestSuite>
  );
}

const styles = StyleSheet.create({
  container: {
    backgroundColor: 'red',
  },
  button: {
    height: 50,
    flex: 1,
    backgroundColor: 'red',
  },
  buttonText: {
    width: '100%',
    fontWeight: 'bold',
    color: 'white',
  },
});
