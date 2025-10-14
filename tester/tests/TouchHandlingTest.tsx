/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {useEffect, useRef, useState} from 'react';
import {
  Animated,
  FlatList,
  PanResponder,
  Pressable,
  RefreshControl,
  ScrollView,
  StyleSheet,
  Text,
  TextInput,
  TouchableOpacity,
  TouchableWithoutFeedback,
  View,
  ViewStyle,
} from 'react-native';
import {Button, StateKeeper, TestCase} from '../components';
import React from 'react';
import {PALETTE} from '../components/palette';

export function TouchHandlingTest() {
  const [refreshing, setRefreshing] = React.useState(false);

  const onRefresh = React.useCallback(() => {
    setRefreshing(true);
    setTimeout(() => {
      setRefreshing(false);
    }, 10000);
  }, []);
  return (
    <TestSuite name="Touch Handling">
      <TestCase.Manual
        itShould="pass when pressed red rectangle"
        initialState={false}
        arrange={({setState}) => {
          return (
            <TouchIssue1
              onPress={() => {
                setState(true);
              }}
            />
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="pass when pressed red rectangle which is outside its green parent view"
        initialState={false}
        arrange={({setState}) => {
          return (
            <TouchIssue2
              onPress={() => {
                setState(true);
              }}
            />
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="register a touch after native transform animation"
        initialState={false}
        arrange={({setState}) => (
          <RectangleSlider
            onPress={() => {
              setState(prev => !prev);
            }}
          />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="handle press on rotated view"
        initialState={false}
        arrange={({setState}) => (
          <TouchableTransformedTest
            setState={setState}
            transform={[{rotate: '180deg'}, {translateX: 100}]}
          />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="handle press on scaled view"
        initialState={false}
        arrange={({setState}) => (
          <TouchableTransformedTest
            setState={setState}
            transform={[{scaleX: -1}, {translateX: 100}]}
          />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example modal itShould="report transformed touch coordinates">
        <TouchCoordinatesTest
          transform={[
            {rotate: '45deg'},
            {translateY: 50},
            {translateX: -50},
            {scaleY: -1},
            {scale: 0.75},
          ]}
        />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="report transformed touch coordinates (2)">
        <TouchCoordinatesTest
          transform={[
            {rotate: '-45deg'},
            {translateY: 50},
            {translateX: -50},
            {scaleX: -1},
            {scaleY: 1.25},
          ]}
        />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="toggle color on press but not on scroll start">
        <StateKeeper
          initialValue={'red'}
          renderContent={(backgroundColor, setBackgroundColor) => {
            return (
              <ScrollView style={{height: 256}}>
                <TouchableOpacity
                  onPress={() =>
                    setBackgroundColor(prev =>
                      prev === 'red' ? 'green' : 'red',
                    )
                  }>
                  <View style={{width: 256, height: 512, backgroundColor}}>
                    <View
                      style={{
                        width: 256,
                        height: 128,
                        backgroundColor: 'white',
                        opacity: 0.75,
                        marginTop: 128,
                      }}
                    />
                    <View
                      style={{
                        width: 256,
                        height: 128,
                        backgroundColor: 'white',
                        opacity: 0.75,
                        marginTop: 128,
                      }}
                    />
                  </View>
                </TouchableOpacity>
              </ScrollView>
            );
          }}
        />
      </TestCase.Example>
      <TestCase.Manual
        itShould="respond to touches on disabled components when wrapped in Touchables"
        initialState={false}
        arrange={({setState}) => (
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
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example
        modal
        itShould="allow vertical scroll when flinging fast after horizontal swipe on gray area">
        <ScrollViewLockedIssue />
      </TestCase.Example>
      <TestCase.Manual
        itShould="pass after tapping cyan area but not red area (child's hitSlop)"
        initialState={false}
        arrange={({setState}) => {
          return (
            <View style={{alignItems: 'center', backgroundColor: 'black'}}>
              <View
                style={{
                  flexDirection: 'row',
                  padding: 48,
                  backgroundColor: PALETTE.REACT_CYAN_LIGHT,
                  justifyContent: 'center',
                  width: 48 * 3,
                }}>
                <TouchableOpacity
                  onPress={() => {
                    setState(true);
                  }}>
                  <View
                    style={{backgroundColor: 'purple', width: 48, height: 48}}
                    hitSlop={{top: 48, bottom: 48, left: 48, right: 48}}
                  />
                </TouchableOpacity>
                <View
                  style={{
                    backgroundColor: 'red',
                    width: 48,
                    height: 48,
                    position: 'absolute',
                    top: 48,
                  }}
                />
              </View>
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="handle views with scale: 0 correctly"
        initialState={false}
        arrange={({setState, reset}) => {
          return (
            <View>
              <TouchableOpacity onPress={() => setState(true)}>
                <View
                  style={{
                    backgroundColor: 'red',
                    width: 100,
                    height: 100,
                  }}
                />
              </TouchableOpacity>
              <View
                style={{
                  position: 'absolute',
                  transform: [{scale: 0}],
                  backgroundColor: 'green',
                  width: 100,
                  height: 100,
                }}
              />
              <View
                style={{
                  position: 'absolute',
                  transform: [{scaleX: 0}],
                  backgroundColor: 'blue',
                  width: 100,
                  height: 100,
                }}
              />
              <View
                style={{
                  position: 'absolute',
                  transform: [{scaleY: 0}],
                  backgroundColor: 'yellow',
                  width: 100,
                  height: 100,
                }}
              />
              <Button label="Reset" onPress={reset} />
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example itShould="emit touch events with resonable timestamps (event.timeStamp is the UNIX time, nativeEvent.timestamp is the device uptime, both are in ms)">
        <TimestampExample />
      </TestCase.Example>
      <TestCase.Manual
        itShould="report touches to transformed children that exceed parent"
        initialState={false}
        arrange={({setState}) => {
          return (
            <View>
              <TouchableWithoutFeedback onPress={() => setState(false)}>
                <View
                  style={{
                    width: 50,
                    height: 50,
                    backgroundColor: 'red',
                  }}>
                  <TouchableWithoutFeedback onPress={() => setState(true)}>
                    <View
                      style={{
                        width: 50,
                        height: 50,
                        backgroundColor: 'blue',
                        transform: [{translateX: 100}],
                      }}
                    />
                  </TouchableWithoutFeedback>
                </View>
              </TouchableWithoutFeedback>
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        modal
        itShould="take into account offset added by RefreshControl while refreshing - press 0 while refreshing to pass"
        initialState={undefined as number | undefined}
        arrange={({setState}) => {
          const styles = StyleSheet.create({
            item: {
              backgroundColor: '#f9c2ff',
              padding: 10,
              marginVertical: 4,
              marginHorizontal: 8,
            },
            title: {
              fontSize: 32,
            },
          });
          const DATA = [0, 1, 2, 3, 4];

          const Item = ({index}: {index: number}) => (
            <Pressable
              style={styles.item}
              onPress={() => {
                setState(index);
              }}>
              <Text style={styles.title}>{index}</Text>
            </Pressable>
          );
          return (
            <FlatList
              refreshing={false}
              data={DATA}
              style={{flex: 1, backgroundColor: 'yellow', height: 200}}
              renderItem={({index}) => <Item index={index} />}
              keyExtractor={item => `${item}`}
              refreshControl={
                <RefreshControl refreshing={refreshing} onRefresh={onRefresh} />
              }
            />
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.eq(0);
        }}
      />
    </TestSuite>
  );
}

function TimestampExample() {
  const [nativEventTimestamp, setNativeEventTimestamp] = useState<number>(0);
  const [eventTimestamp, setEventTimestamp] = useState<number>(0);

  return (
    <>
      <Text>event.timeStamp: {eventTimestamp}</Text>
      <Text>event.nativeEvent.timestamp: {nativEventTimestamp}</Text>
      <TouchableOpacity
        onPress={event => {
          setEventTimestamp(event.timeStamp);
          setNativeEventTimestamp(event.nativeEvent.timestamp);
          console.log('event.timeStamp: ' + event.timeStamp);
          console.log(
            'event.nativeEvent.timestamp: ' + event.nativeEvent.timestamp,
          );
        }}>
        <Text>Press me to run the example</Text>
      </TouchableOpacity>
    </>
  );
}

function RectangleSlider(props: {onPress: () => void}) {
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
  const [position, setPosition] = React.useState({x: 0, y: 0});

  return (
    <View style={{height: 250}}>
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

const TouchIssue1 = ({onPress}: {onPress: () => void}) => {
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
      <View style={{opacity: 1, marginTop: 50}}>
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
              style={{height: 100, width: 100, backgroundColor: 'red'}}
            />
          </TouchableOpacity>
        </View>
      </View>
    );
  } else {
    return (
      <View style={{opacity: 0, marginTop: 50}}>
        <View collapsable={false}>
          <View style={{height: 100, width: 100}} />
        </View>
      </View>
    );
  }
};

const TouchIssue2 = ({onPress}: {onPress: () => void}) => {
  return (
    <View style={{height: 150}}>
      <View
        style={{opacity: 1, width: 100, height: 50, backgroundColor: 'green'}}
        collapsable={false}>
        <TouchableOpacity
          style={{marginTop: 50}}
          onPress={() => {
            onPress();
          }}>
          <View style={{height: 100, width: 100, backgroundColor: 'red'}} />
        </TouchableOpacity>
      </View>
    </View>
  );
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
        console.log('====================touchCapture=' + touchCapture);
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
      <ScrollView style={{height: '75%'}}>
        <View style={{height: 100}}>
          <Text>this is first part</Text>
        </View>
        <View
          style={{height: 300, backgroundColor: 'green'}}
          {...this._gestureHandlers.panHandlers}>
          <Text>this is second part</Text>
        </View>
        <View
          style={{
            height: 800,
            backgroundColor: 'yellow',
          }}
          {...this._gestureHandlers.panHandlers}>
          <TouchableOpacity onPress={() => console.log('1111111')}>
            <Text style={{backgroundColor: 'gray', height: 100}}>
              SWIPE HORIZONTALLY HERE
            </Text>
          </TouchableOpacity>
        </View>
      </ScrollView>
    );
  }
}
