/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {ScrollView, Text, TouchableOpacity, View, ViewProps} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {useState} from 'react';
import {TestCase} from '../components';

export const TouchableOpacityTest = () => {
  const [pressCountOpacity, setPressCountOpacity] = useState(0);
  const [onLayoutTestText, setOnLayoutTestText] = useState('');
  return (
    <TestSuite name="TouchableOpacity">
      <TestCase.Example itShould="make the text less visible on press">
        <TouchableOpacity onPress={() => {}}>
          <PressMe />
        </TouchableOpacity>
      </TestCase.Example>
      <TestCase.Example itShould="make the text slightly less visible on press (activeOpacity)">
        <TouchableOpacity activeOpacity={0.5} onPress={() => {}}>
          <PressMe />
        </TouchableOpacity>
      </TestCase.Example>
      <TestCase.Example itShould="show number of presses on press">
        <TouchableOpacity
          onPress={() => setPressCountOpacity(pressCountOpacity + 1)}>
          <PressMe endLabel={pressCountOpacity} />
        </TouchableOpacity>
      </TestCase.Example>
      <TestCase.Example itShould="render disabled">
        <TouchableOpacity disabled>
          <PressMe endLabel={'disabled'} />
        </TouchableOpacity>
      </TestCase.Example>
      <TestCase.Example itShould="show layout data onLayout">
        <TouchableOpacity
          onLayout={event => {
            setOnLayoutTestText(JSON.stringify(event.nativeEvent.layout));
          }}>
          <PressMe style={{height: 100}} endLabel={onLayoutTestText} />
        </TouchableOpacity>
      </TestCase.Example>
      <TestCase.Example itShould="show square (red background, white border, rounded corners)">
        <TouchableOpacity
          style={{
            backgroundColor: 'rgb(255, 0, 0)',
            width: 100,
            height: 100,
            borderWidth: 3,
            borderColor: 'white',
            borderTopLeftRadius: 10,
            borderTopRightRadius: 20,
            borderBottomRightRadius: 30,
            borderBottomLeftRadius: 40,
          }}>
          <PressMe />
        </TouchableOpacity>
      </TestCase.Example>
      <TestCase.Manual
        itShould="Gray square does not trigger onPress when pressed and dragged; yellow square does not trigger onScroll."
        initialState={{
          pressIn: 0,
          pressed: 0,
          scrolled: 0,
        }}
        arrange={({ setState }) => {
          return (
            <TouchableOpacity
              onPress={() => {
                setState(prev => ({...prev, pressed: prev.pressed + 1}));
              }}>
              <ScrollView
                horizontal
                showsHorizontalScrollIndicator={false}
                onScrollEndDrag={() => {
                  setState(prev => ({...prev, scrolled: prev.scrolled + 1}));
                }}>
                <View
                  style={{
                    flex: 1,
                    justifyContent: 'center',
                    flexDirection: 'row',
                    paddingHorizontal: 8,
                  }}>
                  {[0, 1, 2, 3, 4, 5, 6, 7, 8, 9].map(value => {
                    return value % 2 ? (
                      <View
                        key={value}
                        style={{
                          width: 88,
                          height: 88,
                          marginRight: 8,
                          backgroundColor: '#dddd0' + value,
                        }}>
                        <Text>{value}</Text>
                      </View>
                    ) : (
                      <TouchableOpacity
                        key={value}
                        onPressIn={() => {
                          setState(prev => ({...prev, pressIn: prev.pressIn + 1}));
                        }}
                        style={{
                          width: 88,
                          height: 88,
                          marginRight: 8,
                          backgroundColor: '#ddddd' + value,
                        }}>
                        <Text>{value}</Text>
                      </TouchableOpacity>
                    );
                  })}
                </View>
              </ScrollView>
            </TouchableOpacity>
          );
        }}
        assert={({expect, state}) => {
          expect(state.pressIn).to.be.eq(1);
          expect(state.pressed).to.be.eq(1);
          expect(state.scrolled).to.be.eq(1);
        }}
      />
    </TestSuite>
  );
};

function PressMe(props: ViewProps & {endLabel?: string | number}) {
  return (
    <View {...props} style={[{padding: 16, borderWidth: 1}, props.style]}>
      <Text style={{color: 'blue', height: 'auto', width: '100%'}}>
        Press me{props.endLabel !== undefined ? ` (${props.endLabel})` : ''}
      </Text>
    </View>
  );
}
