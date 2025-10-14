/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useState} from 'react';
import {View, Switch, StyleSheet, Text} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../components';

export function SwitchTest() {
  const [isEnabled, setIsEnabled] = useState(false);
  const [event, setEvent] = useState('');

  return (
    <TestSuite name="Switch">
      <TestCase.Example itShould="Render a working switch and display it's state and SwitchChangeEvent details">
        <View style={styles.container}>
          <Text style={{height: 30}}>
            Switch isEnabled: {isEnabled.toString()}
          </Text>
          <Text style={{height: 30}}>OnChange event: {event}</Text>
          <Switch
            trackColor={{false: 'green', true: 'firebrick'}}
            thumbColor={'beige'}
            onValueChange={setIsEnabled}
            value={isEnabled}
            onChange={e => setEvent(JSON.stringify(e.nativeEvent))}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="Not override value set by prop when clicked">
        <ValuePropExample />
      </TestCase.Example>
      <TestCase.Example itShould="Not send an event when switch value changes via props">
        <ValueEventExample />
      </TestCase.Example>
      <TestCase.Example itShould="Render a disabled switch">
        <View style={styles.container}>
          <Switch
            trackColor={{false: 'green', true: 'firebrick'}}
            thumbColor={'beige'}
            disabled
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="Render a disabled switch with true value">
        <View style={styles.container}>
          <Switch
            trackColor={{false: 'green', true: 'firebrick'}}
            thumbColor={'beige'}
            value={true}
            disabled
          />
        </View>
      </TestCase.Example>
    </TestSuite>
  );
}

const ValuePropExample = () => {
  const [value, setValue] = useState(false);

  return (
    <View style={styles.container}>
      <Switch value={value} />
      <Button onPress={() => setValue(!value)} label="Toggle" />
    </View>
  );
};

const ValueEventExample = () => {
  const [value, setValue] = useState(false);
  const [eventCount, setEventCount] = useState(0);

  return (
    <View style={styles.container}>
      <Text>Event count: {eventCount}</Text>
      <Switch
        value={value}
        onValueChange={v => {
          setEventCount(c => c + 1);
          setValue(v);
        }}
      />
      <Button onPress={() => setValue(!value)} label="Toggle" />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
});
