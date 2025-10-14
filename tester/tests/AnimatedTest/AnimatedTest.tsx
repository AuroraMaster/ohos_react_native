/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useEffect, useRef, useState} from 'react';

import {
  Animated,
  NativeScrollEvent,
  NativeSyntheticEvent,
  Pressable,
  ScrollView,
  Text,
  View,
} from 'react-native';

import {TestSuite} from '@rnoh/testerino';
import {Button, Effect, TestCase} from '../../components';
import {AnimatedCoreTest} from './AnimatedCoreTest';
import {AnimatedEasingTest} from './AnimatedEasingTest';
import {AnimatedValueTest} from './AnimatedValueTest';

export function AnimatedTest() {
  return (
    <TestSuite name="Animated">
      <AnimatedCoreTest />
      <AnimatedEasingTest />
      <AnimatedValueTest />
      <TestSuite name="timing">
        <TestSuite name="start">
          <TestCase.Manual
            itShould="call start callback"
            initialState={-1}
            arrange={({setState, state}) => {
              return (
                <Effect
                  onMount={() => {
                    setState(0);
                    Animated.timing(new Animated.Value(20), {
                      toValue: 0,
                      useNativeDriver: true,
                      duration: 1000,
                    }).start(() => {
                      setState(c => c + 1);
                    });
                  }}>
                  <Text>{state}</Text>
                </Effect>
              );
            }}
            assert={async ({expect, state}) => {
              const result = await new Promise(resolve => {
                if (state > 0) {
                  resolve(state);
                }
              });
              expect(result).to.be.greaterThan(0);
            }}
          />
        </TestSuite>
      </TestSuite>
      <TestSuite name="animating various style properties">
        <TestSuite name="opacity">
          <TestCase.Example itShould="fade in and out when clicked">
            <FadeInOut />
            <FadeInOut nativeDriver />
          </TestCase.Example>
        </TestSuite>
        <TestSuite name="perspective">
          <TestCase.Example itShould="move red square closer">
            <Perspective />
          </TestCase.Example>
        </TestSuite>
      </TestSuite>
      <TestSuite name="delay">
        <TestCase.Example itShould="rotate grey square after red square with 0.5 second delay">
          <Delay />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="loop">
        <TestCase.Example itShould="rotate red square in a loop">
          <Loop />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="parallel">
        <TestCase.Example itShould="rotate both squares in parallel">
          <Parallel />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="createAnimatedComponent">
        <TestCase.Example itShould="rotate button on press">
          <AnimatedPressableView />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="spring">
        <TestCase.Example itShould="rotate squares with different stiffness/mass">
          <Spring />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="decay">
        <TestCase.Example itShould="move squares with different initial velocity and deceleration values">
          <Decay />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="diffClamp">
        <TestCase.Example itShould="move square immediately after pressing button">
          <DiffClamp />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="multiply">
        <TestCase.Example itShould="move grey square 2x further horizontally than red">
          <Multiply />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="modulo">
        <TestCase.Example itShould="move grey twice but half the total distance of red">
          <Modulo />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="event">
        <TestCase.Example
          modal
          itShould="gradually change color from green to red when scrolling down (color interpolation)">
          <ColorInterpolationExample />
        </TestCase.Example>
      </TestSuite>
      <TestCase.Example itShould="test event animated crash when detach native event">
          <AnimatedDetachNativeEventCrashTest />
        </TestCase.Example>
      <TestSuite name="forkEvent + unforkEvent">
        <TestCase.Example itShould="stop updating current offset after detaching listener (fork/unfork event)">
          <AnimatedForkUnforkEventTest />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="attachNativeEvent">
        <TestCase.Example itShould="move red square horizontally relatively to the scroll offset (attachNativeEvent)">
          <AnimatedAttachNativeEventTest />
        </TestCase.Example>
      </TestSuite>
    </TestSuite>
  );
}

const AnimatedDetachNativeEventCrashTest = () => {
  const ref = useRef<ScrollView>(null);
  const animatedValue = new Animated.Value(0);
  const translation = animatedValue.interpolate({
    inputRange: [0, 200],
    outputRange: [0, 200],
    extrapolate: 'clamp',
  });
  let detach: any;
  useEffect(() => {
    if (ref !== null) {
      // @ts-ignore
      
      detach = Animated.attachNativeEvent(ref.current, 'onScroll', [
        {nativeEvent: {contentOffset: {y: animatedValue, x: animatedValue}}},
      ]);
    }
  }, [ref]);
  const detachNativeEvent = () => {
    if (ref !== null) {
      detach.detach();
    }
  }

  return (
    <View
      style={{
        width: '100%',
        height: 100,
        position: 'relative',
        overflow: 'hidden',
      }}>
      <Button
        label={'detach Native Event'}
        onPress={detachNativeEvent}
      />
      <ScrollView
        ref={ref}
        style={{width: '100%', height: '100%'}}
        contentContainerStyle={{alignItems: 'center', justifyContent: 'center'}}
        scrollEventThrottle={16}>
        {new Array(3).fill(0).map((_, idx) => {
          return (
            <View
              key={idx}
              style={{
                width: '100%',
                height: 50,
                backgroundColor: 'gray',
                marginBottom: 50,
              }}
            />
          );
        })}
      </ScrollView>
      <Animated.View
        style={[
          {
            position: 'absolute',
            bottom: 0,
            transform: [{translateX: translation}],
            width: 32,
            height: 32,
            backgroundColor: 'red',
          },
        ]}
      />
    </View>
  );
};

const AnimatedAttachNativeEventTest = () => {
  const ref = useRef<ScrollView>(null);
  const animatedValue = new Animated.Value(0);
  const translation = animatedValue.interpolate({
    inputRange: [0, 200],
    outputRange: [0, 200],
    extrapolate: 'clamp',
  });
  useEffect(() => {
    if (ref !== null) {
      // @ts-ignore
      Animated.attachNativeEvent(ref.current, 'onScroll', [
        {nativeEvent: {contentOffset: {y: animatedValue}}},
      ]);
    }
  }, [ref]);

  return (
    <View
      style={{
        width: '100%',
        height: 100,
        position: 'relative',
        overflow: 'hidden',
      }}>
      <ScrollView
        ref={ref}
        style={{width: '100%', height: '100%'}}
        contentContainerStyle={{alignItems: 'center', justifyContent: 'center'}}
        scrollEventThrottle={16}>
        {new Array(3).fill(0).map((_, idx) => {
          return (
            <View
              key={idx}
              style={{
                width: '100%',
                height: 50,
                backgroundColor: 'gray',
                marginBottom: 50,
              }}
            />
          );
        })}
      </ScrollView>
      <Animated.View
        style={[
          {
            position: 'absolute',
            bottom: 0,
            transform: [{translateX: translation}],
            width: 32,
            height: 32,
            backgroundColor: 'red',
          },
        ]}
      />
    </View>
  );
};

type ScrollViewScrollEvent = NativeSyntheticEvent<NativeScrollEvent>;

const AnimatedForkUnforkEventTest = () => {
  const [offset, setOffset] = useState(0);
  const scrollY = new Animated.Value(0);
  const translation = scrollY.interpolate({
    inputRange: [0, 200],
    outputRange: [0, 200],
    extrapolate: 'clamp',
  });

  const animatedEvent = Animated.event(
    [{nativeEvent: {contentOffset: {y: scrollY}}}],
    {
      useNativeDriver: true,
    },
  );
  const handleScroll = (event: ScrollViewScrollEvent) => {
    setOffset(event.nativeEvent.contentOffset.y);
  };

  const detachOffsetListener = () => {
    // @ts-ignore
    Animated.unforkEvent(animatedEvent, handleScroll);
  };

  return (
    <View
      style={{
        width: '100%',
        height: 100,
        position: 'relative',
        overflow: 'hidden',
      }}>
      <Text>{`Offset: ${offset}`}</Text>
      <Button
        label={'detach offset listener from onScroll'}
        onPress={detachOffsetListener}
      />
      <Animated.ScrollView
        style={{width: '100%', height: '100%'}}
        contentContainerStyle={{alignItems: 'center', justifyContent: 'center'}}
        scrollEventThrottle={16}
        // @ts-ignore
        onScroll={Animated.forkEvent(animatedEvent, handleScroll)}>
        {new Array(3).fill(0).map((_, idx) => {
          return (
            <View
              key={idx}
              style={{
                width: '100%',
                height: 50,
                backgroundColor: 'gray',
                marginBottom: 50,
              }}
            />
          );
        })}
      </Animated.ScrollView>
      <Animated.View
        style={[
          {
            position: 'absolute',
            bottom: 0,
            transform: [{translateX: translation}],
            width: 32,
            height: 32,
            backgroundColor: 'red',
          },
        ]}
      />
    </View>
  );
};

function FadeInOut({nativeDriver = false}) {
  const fadeAnim = React.useRef(new Animated.Value(1)).current;

  const handleFadePress = () => {
    Animated.sequence([
      Animated.timing(fadeAnim, {
        toValue: 0,
        duration: 1000,
        useNativeDriver: nativeDriver,
      }),
      Animated.timing(fadeAnim, {
        toValue: 1,
        duration: 1000,
        useNativeDriver: nativeDriver,
      }),
    ]).start();
  };

  return (
    <Pressable onPress={handleFadePress}>
      <Animated.View
        style={{
          marginTop: 24,
          height: 100,
          width: 100,
          opacity: fadeAnim,
          backgroundColor: 'red',
        }}>
        <Text style={{width: 100, height: 48}}>
          Press me to fade{nativeDriver && ' using native driver'}
        </Text>
      </Animated.View>
    </Pressable>
  );
}

const Delay = () => {
  const square1Anim = useRef(new Animated.Value(0)).current;
  const square2Anim = useRef(new Animated.Value(0)).current;
  const animation = Animated.sequence([
    Animated.timing(square1Anim, {
      toValue: 1,
      duration: 500,
      useNativeDriver: true,
    }),
    Animated.delay(500),
    Animated.timing(square2Anim, {
      toValue: 1,
      duration: 500,
      useNativeDriver: true,
    }),
  ]);

  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <AnimatedRotatingSquaresView
      handleAnimation={handleAnimation}
      square1Anim={square1Anim}
      square2Anim={square2Anim}
    />
  );
};
const Loop = () => {
  const squareAnim = useRef(new Animated.Value(0)).current;
  const [isRunning, setIsRunning] = useState(false);
  const animation = Animated.loop(
    Animated.sequence([
      Animated.timing(squareAnim, {
        toValue: 1,
        duration: 1000,
        useNativeDriver: true,
      }),
      Animated.timing(squareAnim, {
        toValue: 0,
        duration: 1000,
        useNativeDriver: true,
      }),
    ]),
  );

  const handleAnimation = () => {
    if (isRunning) {
      animation.stop();
      setIsRunning(false);
    } else {
      animation.reset();
      animation.start();
      setIsRunning(true);
    }
  };

  return (
    <AnimatedRotatingSquaresView
      handleAnimation={handleAnimation}
      square1Anim={squareAnim}
      square2Anim={new Animated.Value(0)}
    />
  );
};

const Parallel = () => {
  const square1Anim = useRef(new Animated.Value(0)).current;
  const square2Anim = useRef(new Animated.Value(0)).current;

  const animation = Animated.parallel([
    Animated.timing(square1Anim, {
      toValue: 1,
      duration: 1000,
      useNativeDriver: true,
    }),
    Animated.timing(square2Anim, {
      toValue: 1,
      duration: 1000,
      useNativeDriver: true,
    }),
  ]);
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <AnimatedRotatingSquaresView
      handleAnimation={handleAnimation}
      square1Anim={square1Anim}
      square2Anim={square2Anim}
    />
  );
};

const AnimatedRotatingSquaresView = (props: {
  square1Anim: Animated.Value;
  square2Anim: Animated.Value;
  handleAnimation: () => void;
}) => {
  return (
    <Pressable
      style={{height: 100, width: '100%'}}
      onPress={props.handleAnimation}>
      <View style={{flexDirection: 'row'}}>
        <Animated.View
          style={{
            height: 50,
            width: 50,
            margin: 10,
            backgroundColor: 'red',
            transform: [
              {
                rotateZ: props.square1Anim.interpolate({
                  inputRange: [0, 1],
                  outputRange: ['0deg', '360deg'],
                }),
              },
            ],
          }}
        />
        <Animated.View
          style={{
            height: 50,
            width: 50,
            margin: 10,
            backgroundColor: 'grey',
            transform: [
              {
                rotateZ: props.square2Anim.interpolate({
                  inputRange: [0, 1],
                  outputRange: ['0deg', '360deg'],
                }),
              },
            ],
          }}
        />
      </View>
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};

const AnimatedPressableView = () => {
  const pressableAnim = useRef(new Animated.Value(0)).current;
  const AnimatedPressable = Animated.createAnimatedComponent(Pressable);
  const animation = Animated.timing(pressableAnim, {
    toValue: 1,
    duration: 500,
    useNativeDriver: true,
  });
  const onPress = () => {
    animation.reset();
    animation.start();
  };
  return (
    <View style={{height: 80, width: '100%'}}>
      <AnimatedPressable
        style={{
          width: 100,
          margin: 20,
          backgroundColor: 'red',
          transform: [
            {
              rotateZ: pressableAnim.interpolate({
                inputRange: [0, 1],
                outputRange: ['0deg', '360deg'],
              }),
            },
          ],
        }}
        onPress={onPress}>
        <Text style={{height: 40}}>Press me to start animation</Text>
      </AnimatedPressable>
    </View>
  );
};

const Spring = () => {
  const square1Anim = useRef(new Animated.Value(0)).current;
  const square2Anim = useRef(new Animated.Value(0)).current;

  const animation = Animated.parallel([
    Animated.spring(square1Anim, {
      toValue: 1,
      stiffness: 1,
      useNativeDriver: true,
    }),
    Animated.spring(square2Anim, {
      toValue: 1,
      mass: 20,
      useNativeDriver: true,
    }),
  ]);
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <Pressable style={{height: 120, width: '100%'}} onPress={handleAnimation}>
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'red',
          transform: [
            {
              rotateZ: square1Anim.interpolate({
                inputRange: [0, 1],
                outputRange: ['0deg', '360deg'],
              }),
            },
          ],
        }}
      />
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'grey',
          transform: [
            {
              rotateZ: square2Anim.interpolate({
                inputRange: [0, 1],
                outputRange: ['0deg', '360deg'],
              }),
            },
          ],
        }}
      />
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};

const Decay = () => {
  const square1Anim = useRef(new Animated.Value(0)).current;
  const square2Anim = useRef(new Animated.Value(0)).current;

  const animation = Animated.parallel([
    Animated.decay(square1Anim, {
      velocity: 0.5,
      useNativeDriver: true,
    }),
    Animated.decay(square2Anim, {
      velocity: 0.25,
      // a deceleration value empirically determined to be _nice_
      deceleration: 0.99875,
      useNativeDriver: true,
    }),
  ]);
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <Pressable style={{height: 120, width: '100%'}} onPress={handleAnimation}>
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'red',
          transform: [
            {
              translateX: square1Anim,
            },
          ],
        }}
      />
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'grey',
          transform: [
            {
              translateX: square2Anim,
            },
          ],
        }}
      />
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};

const Multiply = () => {
  const square1Anim = useRef(new Animated.Value(0)).current;
  const multValue = new Animated.Value(2);
  const squareMultAnim = Animated.multiply(square1Anim, multValue);

  const animation = Animated.timing(square1Anim, {
    toValue: 100,
    duration: 1000,
    useNativeDriver: true,
  });
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <Pressable style={{height: 120, width: '100%'}} onPress={handleAnimation}>
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'red',
          transform: [
            {
              translateX: square1Anim,
            },
          ],
        }}
      />
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'grey',
          transform: [
            {
              translateX: squareMultAnim,
            },
          ],
        }}
      />
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};

const DiffClamp = () => {
  const square1Anim = useRef(new Animated.Value(0)).current;
  const clamped = Animated.diffClamp(square1Anim, 0, 50);
  const [direction, setDirection] = useState('left');

  const animation = React.useRef<Animated.CompositeAnimation>();

  const handleAnimation = () => {
    animation.current?.stop();
    animation.current = Animated.timing(square1Anim, {
      toValue: direction === 'left' ? 400 : 0,
      duration: 4000,
      useNativeDriver: true,
    });
    setDirection(direction === 'left' ? 'right' : 'left');
    animation.current.start();
  };

  return (
    <Pressable style={{width: '100%'}} onPress={handleAnimation}>
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'red',
          transform: [
            {
              translateX: clamped,
            },
          ],
        }}
      />
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};

const Modulo = () => {
  const square1Anim = useRef(new Animated.Value(0)).current;
  const squareModuloAnim = Animated.modulo(square1Anim, 100);

  const animation = Animated.timing(square1Anim, {
    toValue: 199,
    duration: 1000,
    useNativeDriver: true,
  });
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <Pressable style={{height: 120, width: '100%'}} onPress={handleAnimation}>
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'red',
          transform: [
            {
              translateX: square1Anim,
            },
          ],
        }}
      />
      <Animated.View
        style={{
          height: 50,
          width: 50,
          backgroundColor: 'grey',
          transform: [
            {
              translateX: squareModuloAnim,
            },
          ],
        }}
      />
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};

const Perspective = () => {
  const square1Anim = useRef(new Animated.Value(500)).current;

  const animation = Animated.timing(square1Anim, {
    toValue: 50,
    duration: 1000,
    useNativeDriver: true,
  });
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <Pressable style={{height: 120, width: '100%'}} onPress={handleAnimation}>
      <View style={{height: 100, justifyContent: 'center'}}>
        <Animated.View
          style={{
            height: 50,
            width: 50,
            backgroundColor: 'red',
            transform: [
              {
                rotateY: '60deg',
              },
              {
                perspective: square1Anim,
              },
            ],
          }}
        />
      </View>
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};

const ColorInterpolationExample: React.FC = () => {
  const animatedValue = new Animated.Value(0);

  const backgroundColor = animatedValue.interpolate({
    inputRange: [0, 3000],
    outputRange: ['rgb(0, 255, 0)', 'rgb(255, 0, 0)'],
  });

  return (
    <View
      style={{
        height: 512,
        width: 256,
        justifyContent: 'center',
        alignItems: 'center',
      }}>
      <Animated.FlatList
        style={{
          height: '100%',
        }}
        data={new Array(20).fill(0).map((_, idx) => idx)}
        renderItem={({}) => {
          return (
            <Animated.View
              style={{
                width: 256,
                marginTop: 24,
                height: 256,
                backgroundColor: backgroundColor,
              }}
            />
          );
        }}
        keyExtractor={item => String(item)}
        onScroll={Animated.event(
          [{nativeEvent: {contentOffset: {y: animatedValue}}}],
          {
            useNativeDriver: true,
          },
        )}
      />
    </View>
  );
};
