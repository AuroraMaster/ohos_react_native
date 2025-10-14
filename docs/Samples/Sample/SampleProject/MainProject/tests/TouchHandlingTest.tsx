/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TestCase, TestSuite } from '@rnoh/testerino';
import { useEffect, useRef, useState } from 'react';
import {
  Animated,
  PanResponder,
  ScrollView,
  Text,
  TextInput,
  TouchableOpacity,
  View,
  ViewStyle,
} from 'react-native';
import { Button } from '../components';
import React from 'react';

export function TouchHandlingTest() {
  return (
    <TestSuite name="Touch Handling">
      <TestCase
        itShould="pass when pressed red rectangle"
        initialState={false}
        arrange={({ setState }) => {
          return (
            <TouchIssue1
              onPress={() => {
                setState(true);
              }}
            />
          );
        }}
        assert={({ expect, state }) => {
          expect(state).to.be.true;
        }}
      />

      <TestCase
        itShould="register a touch after native transform animation"
        initialState={false}
        arrange={({ setState }) => (
          <RectangleSlider
            onPress={() => {
              setState(prev => !prev);
            }}
          />
        )}
        assert={({ expect, state }) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase
        itShould="handle press on rotated view"
        initialState={false}
        arrange={({ setState }) => (
          <TouchableTransformedTest
            setState={setState}
            transform={[{ rotate: '180deg' }, { translateX: 100 }]}
          />
        )}
        assert={({ expect, state }) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase
        itShould="handle press on scaled view"
        initialState={false}
        arrange={({ setState }) => (
          <TouchableTransformedTest
            setState={setState}
            transform={[{ scaleX: -1 }, { translateX: 100 }]}
          />
        )}
        assert={({ expect, state }) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase itShould="report transformed touch coordinates">
        <TouchCoordinatesTest
          transform={[
            { rotate: '45deg' },
            { translateY: 50 },
            { translateX: -50 },
            { scaleY: -1 },
            { scale: 0.75 },
          ]}
        />
      </TestCase>
      <TestCase itShould="report transformed touch coordinates">
        <TouchCoordinatesTest
          transform={[
            { rotate: '-45deg' },
            { translateY: 50 },
            { translateX: -50 },
            { scaleX: -1 },
            { scaleY: 1.25 },
          ]}
        />
      </TestCase>
      <TestCase
        itShould="respond to touches on disabled components when wrapped in Touchables"
        initialState={false}
        arrange={({ setState }) => (
          <TouchableOpacity
            onPress={() => {
              setState(true);
            }}>
            <TextInput
              editable={false}
              style={{
                borderWidth: 2,
                borderColor: 'blue',
              }}
              value={'Non-editable TextInput'}
            />
          </TouchableOpacity>
        )}
        assert={({ expect, state }) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase
        modal
        itShould="allow vertical scroll when flinging fast after horizontal swipe on gray area">
        <ScrollViewLockedIssue />
      </TestCase>
    </TestSuite>
  );
}

function RectangleSlider(props: { onPress: () => void }) {
  const square1Anim = useRef(new Animated.Value(0)).current;
  const animation = Animated.timing(square1Anim, {
    toValue: 64,
    duration: 1000,
    useNativeDriver: true,
  });
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <>
      <Animated.View
        onTouchEnd={() => {
          props.onPress();
        }}
        style={{
          height: 64,
          width: 64,
          backgroundColor: 'red',
          transform: [
            {
              translateX: square1Anim,
            },
          ],
        }}
      />
      <Button label="Animate" onPress={handleAnimation} />
    </>
  );
}

function TouchableTransformedTest({
  setState,
  transform,
}: {
  setState: (v: boolean) => void;
  transform: ViewStyle['transform'];
}) {
  return (
    <View
      style={{
        alignSelf: 'center',
        width: 75,
        backgroundColor: 'red',
        transform,
      }}>
      <TouchableOpacity onPress={() => setState(true)}>
        <Text>Press me!</Text>
      </TouchableOpacity>
    </View>
  );
}

function TouchCoordinatesTest({
  transform,
}: {
  transform?: ViewStyle['transform'];
}) {
  const [position, setPosition] = React.useState({ x: 0, y: 0 });

  return (
    <View style={{ height: 250 }}>
      <Text>Touch coordinates: {JSON.stringify(position)}</Text>
      <View
        style={{
          alignSelf: 'center',
          width: 150,
          height: 150,
          backgroundColor: 'red',
          transform,
          opacity: 0.5,
        }}
        onTouchStart={e => {
          setPosition({
            x: Math.round(e.nativeEvent.locationX),
            y: Math.round(e.nativeEvent.locationY),
          });
        }}
        onTouchMove={e => {
          setPosition({
            x: Math.round(e.nativeEvent.locationX),
            y: Math.round(e.nativeEvent.locationY),
          });
        }}>
        <Text>Top left</Text>
      </View>
    </View>
  );
}

const TouchIssue1 = ({ onPress }: { onPress: () => void }) => {
  const nPressesRef = useRef(0);
  const [nRenders, setNRenders] = useState(0);
  const [label, setLabel] = useState('hello');

  useEffect(() => {
    const timeout = setTimeout(() => {
      setNRenders(1);
    }, 2000);
    return () => {
      clearTimeout(timeout);
    };
  }, []);

  if (nRenders > 0) {
    return (
      <View style={{ opacity: 1, marginTop: 50 }}>
        <View collapsable={false}>
          <TouchableOpacity
            onPress={() => {
              onPress();
              setLabel(`${label}+${nPressesRef.current}`);
              nPressesRef.current++;
            }}>
            <Text>{label}</Text>
            <View
              id="foo"
              style={{ height: 100, width: 100, backgroundColor: 'red' }}
            />
          </TouchableOpacity>
        </View>
      </View>
    );
  } else {
    return (
      <View style={{ opacity: 0, marginTop: 50 }}>
        <View collapsable={false}>
          <View style={{ height: 100, width: 100 }} />
        </View>
      </View>
    );
  }
};

class ScrollViewLockedIssue extends React.Component {
  textInput: any;
  blur = () => {
    this.textInput.blur();
  };
  _gestureHandlers: any;
  componentWillMount() {
    this._gestureHandlers = PanResponder.create({
      onStartShouldSetPanResponder: () => true,
      onStartShouldSetPanResponderCapture: () => false,
      onMoveShouldSetPanResponder: (event, gestureState) => {
        const touchCapture =
          Math.abs(gestureState.dx) > 10 || Math.abs(gestureState.dy) > 10;
        return touchCapture;
      },
      onMoveShouldSetPanResponderCapture: () => false,
      onPanResponderMove: (_evt, _gestureState) => {
        console.warn('move');
      },
    });
  }

  render(): React.ReactNode {
    return (
      <ScrollView style={{ height: '75%' }}>
        <View style={{ height: 100 }}>
          <Text>this is first part</Text>
        </View>
        <View
          style={{ height: 300, backgroundColor: 'green' }}
          {...this._gestureHandlers.panHandlers}>
          <Text>this is second part</Text>
        </View>
        <View
          style={{
            height: 800,
            backgroundColor: 'yellow',
          }}
          {...this._gestureHandlers.panHandlers}>
          <TouchableOpacity>
            <Text style={{ backgroundColor: 'gray', height: 100 }}>
              SWIPE HORIZONTALLY HERE
            </Text>
          </TouchableOpacity>
        </View>
      </ScrollView>
    );
  }
}
