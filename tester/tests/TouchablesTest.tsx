/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  Text,
  Touchable,
  TouchableHighlight,
  TouchableNativeFeedback,
  TouchableWithoutFeedback,
  View,
  ViewProps,
} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {useState} from 'react';
import {TestCase} from '../components';

export const TouchablesTest = () => {
  const [pressCountHighlight, setPressCountHighlight] = useState(0);
  return (
    <TestSuite name="Touchables">
      <TestCase.Logical
        itShould="export Touchable"
        fn={({expect}) => {
          expect(Touchable).to.be.not.undefined;
        }}
      />
      <TestCase.Example itShould="use cyan background on press in (TouchableHighlight)">
        <TouchableHighlight
          activeOpacity={1}
          underlayColor="cyan"
          onPress={() => {}}>
          <PressMe />
        </TouchableHighlight>
      </TestCase.Example>
      <TestCase.Example itShould="use cyan background, displayed as if pressed (TouchableHighlight)">
        <TouchableHighlight
          activeOpacity={1}
          underlayColor="cyan"
          onPress={() => {}}
          //@ts-ignore
          testOnly_pressed>
          <PressMe />
        </TouchableHighlight>
      </TestCase.Example>
      <TestCase.Example itShould="show number of presses on press in (TouchableHighlight)">
        <TouchableHighlight
          onPress={() => setPressCountHighlight(pressCountHighlight + 1)}>
          <PressMe endLabel={pressCountHighlight} />
        </TouchableHighlight>
      </TestCase.Example>
      <TestCase.Logical
        itShould="export TouchableNativeFeedback (Android only)"
        fn={({expect}) => {
          expect(TouchableNativeFeedback).to.be.not.undefined;
        }}
      />
      <TestCase.Example itShould="handle press without showing feedback">
        <TouchableWithoutFeedbackDemo />
      </TestCase.Example>
      <TestCase.Manual
        itShould="handle presses on empty views"
        initialState={false}
        arrange={({setState}) => {
          return (
            <View style={{height: 100, backgroundColor: 'red'}}>
              <TouchableWithoutFeedback onPress={() => setState(true)}>
                <View style={{height: '100%', width: '100%'}} />
              </TouchableWithoutFeedback>
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="pass when blue background is pressed"
        initialState={false}
        arrange={({setState}) => (
          <View
            style={{
              backgroundColor: 'blue',
              alignSelf: 'center',
              position: 'relative',
            }}>
            <TouchableWithoutFeedback
              hitSlop={{top: 48, left: 48, bottom: 48, right: 48}}
              onPress={() => {
                setState(true);
              }}>
              <View style={{width: 48, height: 48, margin: 48}} />
            </TouchableWithoutFeedback>
            <View
              style={{
                width: 48,
                height: 48,
                backgroundColor: 'red',
                position: 'absolute',
                top: 48,
                left: 48,
              }}
              onTouchEnd={e => {
                e.stopPropagation();
              }}
            />
          </View>
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
    </TestSuite>
  );
};

function TouchableWithoutFeedbackDemo() {
  const [counter, setCounter] = useState(0);
  return (
    <TouchableWithoutFeedback onPressIn={() => setCounter(prev => prev + 1)}>
      <PressMe endLabel={counter} />
    </TouchableWithoutFeedback>
  );
}

function PressMe(props: ViewProps & {endLabel?: string | number}) {
  return (
    <View {...props} style={{padding: 16, borderWidth: 1}}>
      <Text style={{color: 'blue', height: 24, width: '100%'}}>
        Press me{props.endLabel !== undefined ? ` (${props.endLabel})` : ''}
      </Text>
    </View>
  );
}
