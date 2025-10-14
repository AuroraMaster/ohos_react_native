/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  Dimensions,
  Keyboard,
  KeyboardAvoidingView,
  ScrollView,
  StyleProp,
  StyleSheet,
  TextInput,
  TouchableWithoutFeedback,
  View,
  ViewStyle,
} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import React from 'react';
import {Modal, TestCase} from '../components';
import {PALETTE} from '../components/palette';

export const KeyboardAvoidingViewTest = () => {
  return (
    <TestSuite name="KeyboardAvoidingView">
      <TestCase.Example itShould="don't avoid keyboard">
        <KeyboardAvoidingViewTestCase
          behavior="height"
          justifyContent="space-around"
          enabled={false}
        />
      </TestCase.Example>
      <TestCase.Example itShould="avoid keyboard by changing height">
        <KeyboardAvoidingViewTestCase
          behavior="height"
          justifyContent="space-around"
        />
      </TestCase.Example>
      <TestCase.Example itShould="avoid keyboard by changing position. The firebrick border should move, the green one shouldn't">
        <KeyboardAvoidingViewTestCase
          behavior="position"
          justifyContent="flex-end"
          contentContainerStyle={{borderWidth: 4, borderColor: 'firebrick'}}
          style={{borderWidth: 4, borderColor: 'green'}}
        />
      </TestCase.Example>
      <TestCase.Example itShould="avoid keyboard by changing padding">
        <KeyboardAvoidingViewTestCase
          behavior="padding"
          justifyContent="flex-end"
        />
      </TestCase.Example>
      <TestCase.Example itShould="keep TextInput visible after tapping TextInput">
        <Modal contentContainerStyle={{height: '100%', width: '50%'}}>
          <KeyboardAvoidingView behavior={'height'}>
            <ScrollView>
              <View
                style={{
                  height: Dimensions.get('window').height - 128,
                  backgroundColor: 'silver',
                }}
              />
              <TextInput
                style={{
                  width: '100%',
                  backgroundColor: PALETTE.REACT_CYAN_LIGHT,
                  paddingHorizontal: 8,
                  paddingVertical: 16,
                  marginBottom: 64,
                }}
                placeholder="CLICK ME"
              />
            </ScrollView>
          </KeyboardAvoidingView>
        </Modal>
      </TestCase.Example>
    </TestSuite>
  );
};

type KeyboardAvoidingViewTestCaseProps = {
  behavior?: 'height' | 'position' | 'padding' | undefined;
  justifyContent?:
    | 'flex-start'
    | 'flex-end'
    | 'center'
    | 'space-between'
    | 'space-around'
    | 'space-evenly'
    | undefined;
  contentContainerStyle?: StyleProp<ViewStyle> | undefined;
  enabled?: boolean | undefined;
  style?: StyleProp<ViewStyle> | undefined;
};
function KeyboardAvoidingViewTestCase(
  props: KeyboardAvoidingViewTestCaseProps,
) {
  return (
    <>
      <Modal contentContainerStyle={{height: '100%'}}>
        <KeyboardAvoidingView
          behavior={props.behavior}
          keyboardVerticalOffset={75} //magic value, corresponding to the height of the wrapper etc.
          enabled={props.enabled}
          contentContainerStyle={props.contentContainerStyle}
          style={[styles.container, props.style]}>
          <TouchableWithoutFeedback onPress={Keyboard.dismiss}>
            <View
              style={{height: '100%', justifyContent: props.justifyContent}}>
              <TextInput placeholder="TextInput" style={styles.textInput} />
              <View
                style={{height: 150, width: 150, backgroundColor: 'firebrick'}}
              />
              <View
                style={{height: 150, width: 150, backgroundColor: 'beige'}}
              />
            </View>
          </TouchableWithoutFeedback>
        </KeyboardAvoidingView>
      </Modal>
    </>
  );
}
const styles = StyleSheet.create({
  container: {
    height: '100%',
  },
  textInput: {
    height: 40,
    borderColor: '#000000',
    borderWidth: 1,
    marginBottom: 36,
  },
});
