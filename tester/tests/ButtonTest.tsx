/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {View, Button, StyleSheet, Text, findNodeHandle} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../components';

export const ButtonTest = () => {
  return (
    <TestSuite name="Button">
      <TestCase.Example itShould="render a button with a title ">
        <ButtonView />
      </TestCase.Example>
      <TestCase.Example itShould="render a button that should be disabled">
        <ButtonDisabled />
      </TestCase.Example>
      <TestCase.Example
        itShould="render a button with touchSoundDisabled"
        skip={{
          android: false,
          harmony: {
            arkTS: "prop doesn't exist on native side",
            cAPI: "prop doesn't exist on native side",
          },
        }} // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/476
      >
        <ButtonTouchSoundDisabled />
      </TestCase.Example>
      <TestCase.Example itShould="render a button with accessibility label">
        <ButtonAccessibilityLabel />
      </TestCase.Example>
      <TestCase.Example
        skip={{android: false, harmony: {arkTS: true, cAPI: true}}}
        modal
        itShould="allow user to move with keyboard's arrows keys between buttons">
        <ButtonNextFocus />
      </TestCase.Example>
    </TestSuite>
  );
};
function ButtonView() {
  const [pressCounter, setPressCounter] = React.useState(0);
  const incrementPressCounter = () => {
    setPressCounter(count => count + 1);
  };

  return (
    <View>
      <View style={styles.buttonContainer}>
        <Button
          title="Press me!"
          color="#FF0000"
          onPress={incrementPressCounter}
        />
      </View>
      <Text style={styles.text}>Pressed {pressCounter} times</Text>
    </View>
  );
}

function ButtonTouchSoundDisabled() {
  const [pressCounter, setPressCounter] = React.useState(0);
  const [touchSoundDisabled, setTouchSoundDisabled] = React.useState(false);

  const incrementPressCounter = () => {
    setPressCounter(count => count + 1);
  };

  const toggle = () => {
    setTouchSoundDisabled(!touchSoundDisabled);
  };

  return (
    <View>
      <View style={styles.buttonsContainer}>
        <Button
          title={'Press me! ' + (touchSoundDisabled ? '(no sound)' : '')}
          color="#FF0000"
          onPress={incrementPressCounter}
          touchSoundDisabled={touchSoundDisabled}
        />
        <Button
          title={
            touchSoundDisabled ? 'Enable Touch Sound' : 'Disable Touch Sound'
          }
          color="#A4A4A4"
          onPress={toggle}
          touchSoundDisabled={false}
        />
      </View>
      <Text style={styles.text}>Pressed {pressCounter} times</Text>
    </View>
  );
}

function ButtonDisabled() {
  const [pressCounter, setPressCounter] = React.useState(0);
  const [disabled, setDisabled] = React.useState(false);

  const incrementPressCounter = () => {
    setPressCounter(count => count + 1);
  };

  const toggle = () => {
    setDisabled(!disabled);
  };

  return (
    <View>
      <View style={styles.buttonsContainer}>
        <Button
          title={disabled ? '(disabled)' : 'Press Me'}
          color="#FF0000"
          onPress={incrementPressCounter}
          disabled={disabled}
        />
        <Button
          title={disabled ? 'Enable button click' : 'Disable button click'}
          color="#A4A4A4"
          onPress={toggle}
        />
      </View>
      <Text style={styles.text}>Pressed {pressCounter} times</Text>
    </View>
  );
}

function ButtonAccessibilityLabel() {
  const [pressCounter, setPressCounter] = React.useState(0);

  const incrementPressCounter = () => {
    setPressCounter(count => count + 1);
  };

  return (
    <View>
      <View style={styles.buttonsContainer}>
        <Button
          title={'Increment the count'}
          color="#FF0000"
          onPress={incrementPressCounter}
          accessibilityLabel="Increment the count"
        />
      </View>
      <Text style={styles.text}>Pressed {pressCounter} times</Text>
    </View>
  );
}

function ButtonNextFocus() {
  const [pressCounter, setPressCounter] = React.useState(0);

  const topLeftButtonRef = React.useRef(null);
  const topRightButtonRef = React.useRef(null);
  const botLeftButtonRef = React.useRef(null);
  const botRightButtonRef = React.useRef(null);

  const topLeftNode = findNodeHandle(topLeftButtonRef.current);
  const topRightNode = findNodeHandle(topRightButtonRef.current);
  const botLeftNode = findNodeHandle(botLeftButtonRef.current);
  const botRightNode = findNodeHandle(botRightButtonRef.current);

  const incrementPressCounter = () => {
    setPressCounter(count => count + 1);
  };

  return (
    <View>
      <View style={styles.buttonNextFocusContainer}>
        <Button
          ref={topLeftButtonRef}
          title={'Top Left'}
          onPress={incrementPressCounter}
          nextFocusDown={botLeftNode ?? undefined}
          nextFocusUp={botLeftNode ?? undefined}
          nextFocusRight={topRightNode ?? undefined}
          nextFocusLeft={topRightNode ?? undefined}
        />
        <Button title={'Skip'} color="#A4A4A4" onPress={() => {}} />
        <Button
          ref={topRightButtonRef}
          title={'Top Right'}
          onPress={incrementPressCounter}
          nextFocusDown={botRightNode ?? undefined}
          nextFocusUp={botRightNode ?? undefined}
          nextFocusLeft={topLeftNode ?? undefined}
          nextFocusRight={botLeftNode ?? undefined}
        />
      </View>
      <View style={styles.buttonNextFocusContainer}>
        <Button title={'SkipLeft'} color="#A4A4A4" onPress={() => {}} />
        <Button title={'Skip'} color="#A4A4A4" onPress={() => {}} />
        <Button title={'SkipRight'} color="#A4A4A4" onPress={() => {}} />
      </View>
      <View style={styles.buttonNextFocusContainer}>
        <Button
          ref={botLeftButtonRef}
          title={'Bot Left'}
          onPress={incrementPressCounter}
          nextFocusDown={topLeftNode ?? undefined}
          nextFocusUp={topLeftNode ?? undefined}
          nextFocusRight={botRightNode ?? undefined}
          nextFocusLeft={topRightNode ?? undefined}
        />
        <Button title={'Skip'} color="#A4A4A4" onPress={() => {}} />
        <Button
          ref={botRightButtonRef}
          title={'Bot Right'}
          onPress={incrementPressCounter}
          nextFocusDown={topRightNode ?? undefined}
          nextFocusUp={topRightNode ?? undefined}
          nextFocusLeft={botLeftNode ?? undefined}
          nextFocusRight={botLeftNode ?? undefined}
        />
      </View>
      <Text style={styles.text}>Count: {pressCounter}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  buttonContainer: {
    width: 80,
    height: 80,
  },
  buttonsContainer: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 20,
  },
  text: {
    height: 20,
    width: 200,
    fontSize: 14,
  },
  buttonNextFocusContainer: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 10,
    gap: 10,
  },
  buttonNextFocus: {
    width: 50,
    height: 20,
  },
});
