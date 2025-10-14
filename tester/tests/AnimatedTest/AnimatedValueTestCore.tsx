/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useEffect, useRef, useState} from 'react';
import {Animated, View, Text, PanResponder, Pressable} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {Button, Effect, TestCase} from '../../components';

export function AnimatedValueTestCore({
  renderInterpolateTests,
}: {
  renderInterpolateTests?: () => React.ReactNode;
}) {
  return (
    <>
      <TestSuite name="Animated.Value">
        <TestSuite name="addListener + removeListener + removeAllListeners + hasListeners">
          <TestCase.Manual
            itShould="move square 200px to the right and stop animation on pressing setValue"
            initialState={0}
            arrange={({setState}) => (
              <SetValueView singular setState={setState} />
            )}
            assert={({expect, state}) => {
              expect(state).to.be.eq(200);
            }}
          />
          <TestCase.Example itShould="add and remove listeners on click">
            <ListenerView singular={true} />
          </TestCase.Example>
        </TestSuite>

        <TestSuite name="interpolate">{renderInterpolateTests?.()}</TestSuite>

        <TestSuite name="setOffset">
          <TestCase.Example itShould="move square 200px to the right on pressing setOffset">
            <SetOffsetView singular={true} />
          </TestCase.Example>
        </TestSuite>

        <TestSuite name="event">
          <TestCase.Example itShould="move red square horizontally relatively to the scroll offset">
            <AnimatedScrollViewTestCase />
          </TestCase.Example>
        </TestSuite>

        <TestSuite name="tracking">
          <TestCase.Example
            skip="tracking value doesn't seem to work anywhere"
            itShould="move both squares, with blue square following the red with a spring">
            <TrackingValue />
          </TestCase.Example>
        </TestSuite>
      </TestSuite>

      <TestSuite name="Animated.ValueXY">
        <TestSuite name="core">
          <TestCase.Example itShould="move square both vertically and horizontally">
            <ValueXY />
          </TestCase.Example>
          <TestCase.Example itShould="move the blue square on drag">
            <AnimatedDraggableView />
          </TestCase.Example>
        </TestSuite>

        <TestSuite name="addListener + removeListener + removeAllListeners + hasListeners">
          <TestCase.Manual
            itShould="move square 100px to the right and stop animation on pressing setValue"
            initialState={0}
            arrange={({setState}) => <SetValueView setState={setState} />}
            assert={({expect, state}) => {
              expect(state).to.be.eq(100);
            }}
          />
          <TestCase.Example itShould="add and remove listeners on click">
            <ListenerView singular={false} />
          </TestCase.Example>
        </TestSuite>

        <TestSuite name="setOffset">
          <TestCase.Example itShould="move square 100px to the right on pressing setOffset">
            <SetOffsetView singular={false} />
          </TestCase.Example>
        </TestSuite>

        <TestSuite name="getLayout">
          <TestCase.Manual<Object>
            itShould="get layout of animated value"
            initialState={{}}
            arrange={({setState}) => {
              const animValue = new Animated.ValueXY({x: 1, y: 1});
              return (
                <Effect
                  onMount={() => {
                    setState(animValue.getLayout());
                  }}
                />
              );
            }}
            assert={({state, expect}) => {
              expect(JSON.stringify(state)).to.be.eq(
                JSON.stringify({left: 1, top: 1}),
              );
            }}
          />
          <TestCase.Example itShould="blue square should move the same as red square">
            <AnimatedGetLayout />
          </TestCase.Example>
        </TestSuite>

        <TestSuite name="extractOffset">
          <TestCase.Example itShould="move square to the right after extract offset">
            <ExtractOffsetView />
          </TestCase.Example>
        </TestSuite>

        <TestSuite name="flattenOffset">
          <TestCase.Example itShould="move square to the left after flatten offset">
            <FlattenOffsetView />
          </TestCase.Example>
        </TestSuite>
      </TestSuite>
    </>
  );
}

const ValueXY = () => {
  const square1Anim = useRef(new Animated.ValueXY({x: 50, y: 50})).current;

  const animation = Animated.sequence([
    Animated.timing(square1Anim, {
      toValue: {x: 0, y: 0},
      duration: 1000,
      useNativeDriver: true,
    }),
    Animated.timing(square1Anim, {
      toValue: {x: 0, y: 50},
      duration: 1000,
      useNativeDriver: true,
    }),
    Animated.timing(square1Anim, {
      toValue: {x: 50, y: 0},
      duration: 1000,
      useNativeDriver: true,
    }),
    Animated.timing(square1Anim, {
      toValue: {x: 50, y: 50},
      duration: 1000,
      useNativeDriver: true,
    }),
  ]);
  const handleAnimation = () => {
    animation.reset();
    animation.start();
  };

  return (
    <Pressable style={{height: 120, width: '100%'}} onPress={handleAnimation}>
      <View style={{height: 100}}>
        <Animated.View
          style={{
            height: 40,
            width: 40,
            backgroundColor: 'red',
            transform: [
              {
                translateX: square1Anim.x,
              },
              {
                translateY: square1Anim.y,
              },
            ],
          }}
        />
      </View>
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};

const ExtractOffsetView = () => {
  const value = useRef(new Animated.Value(0)).current;
  const animation = Animated.loop(
    Animated.sequence([
      Animated.timing(value, {
        toValue: 100,
        duration: 1000,
        useNativeDriver: true,
      }),
      Animated.timing(value, {
        toValue: 0,
        duration: 1000,
        useNativeDriver: true,
      }),
    ]),
  );
  return (
    <View style={{width: '100%'}}>
      <Animated.View
        style={{
          height: 20,
          width: 20,
          margin: 10,
          backgroundColor: 'red',
          transform: [
            {
              translateX: value,
            },
          ],
        }}
      />
      <View style={{flexDirection: 'row', flexWrap: 'wrap'}}>
        <Button
          label="start"
          onPress={() => {
            animation.reset();
            animation.start();
          }}
        />
        <Button label="stop" onPress={() => animation.stop()} />
        <Button label="extract offset" onPress={() => value.extractOffset()} />
      </View>
    </View>
  );
};
const FlattenOffsetView = () => {
  const value = useRef(new Animated.Value(0)).current;
  const animation = Animated.loop(
    Animated.sequence([
      Animated.timing(value, {
        toValue: 100,
        duration: 1000,
        useNativeDriver: true,
      }),
      Animated.timing(value, {
        toValue: 0,
        duration: 1000,
        useNativeDriver: true,
      }),
    ]),
  );
  value.setOffset(100);
  return (
    <View style={{width: '100%'}}>
      <Animated.View
        style={{
          height: 20,
          width: 20,
          margin: 10,
          backgroundColor: 'red',
          transform: [
            {
              translateX: value,
            },
          ],
        }}
      />
      <View style={{flexDirection: 'row', flexWrap: 'wrap'}}>
        <Button
          label="start"
          onPress={() => {
            animation.reset();
            animation.start();
          }}
        />
        <Button label="stop" onPress={() => animation.stop()} />
        <Button
          label="flatten offset"
          onPress={() => {
            value.flattenOffset();
          }}
        />
      </View>
    </View>
  );
};
const SetOffsetView = (props: {singular: boolean}) => {
  const animValue = useRef(new Animated.Value(0)).current;
  const animValueXY = useRef(new Animated.ValueXY({x: 0, y: 0})).current;
  if (props.singular) {
    return (
      <MovingSquare
        animValue={animValue}
        labels={['set offset']}
        onPresses={[() => animValue.setOffset(200)]}
      />
    );
  } else {
    return (
      <MovingSquareXY
        animValueXY={animValueXY}
        labels={['set offset']}
        onPresses={[() => animValueXY.setOffset({x: 100, y: 0})]}
      />
    );
  }
};

const ListenerView = (props: {singular: boolean}) => {
  const [text, setText] = useState('');
  const [listeners, setListeners] = useState<string[]>([]);
  const animValue = useRef(new Animated.Value(0)).current;
  const animValueXY = useRef(new Animated.ValueXY({x: 0, y: 0})).current;
  const listener = () => {};
  const addListener = () => {
    listeners.push(animValue.addListener(listener));
    setListeners(listeners);
    checkListener();
  };
  const removeListener = () => {
    const lastListener = listeners.pop();
    if (lastListener) {
      animValue.removeListener(lastListener);
    }
    checkListener();
  };
  const removeAll = () => {
    animValue.removeAllListeners();
    checkListener();
  };
  const checkListener = () => {
    setText(
      animValue.hasListeners()
        ? `listener: ${listeners}`
        : 'no listener is attached',
    );
  };
  return (
    <>
      <Text>{text}</Text>
      {props.singular ? (
        <MovingSquare
          animValue={animValue}
          labels={['add', 'remove', 'removeAll']}
          onPresses={[addListener, removeListener, removeAll]}
        />
      ) : (
        <MovingSquareXY
          animValueXY={animValueXY}
          labels={['add', 'remove', 'removeAll']}
          onPresses={[addListener, removeListener, removeAll]}
        />
      )}
    </>
  );
};

const SetValueView = (props: {
  singular?: boolean;
  setState: React.Dispatch<React.SetStateAction<number>>;
}) => {
  const animValue = useRef(new Animated.Value(0)).current;
  const animValueXY = useRef(new Animated.ValueXY({x: 0, y: 0})).current;

  useEffect(() => {
    if (props.singular) {
      animValue.addListener(({value}) => {
        if (value === 200) {
          props.setState(value);
        }
      });
    } else {
      animValueXY.addListener(({x}) => {
        if (x === 100) {
          props.setState(x);
        }
      });
    }
  }, []);

  if (props.singular) {
    return (
      <MovingSquare
        animValue={animValue}
        labels={['set value']}
        onPresses={[() => animValue.setValue(200)]}
      />
    );
  } else {
    return (
      <MovingSquareXY
        animValueXY={animValueXY}
        labels={['set valueXY']}
        onPresses={[() => animValueXY.setValue({x: 100, y: 0})]}
      />
    );
  }
};

const MovingSquare = (props: {
  animValue: Animated.Value;
  labels: string[];
  onPresses: (() => void)[];
}) => {
  const [isRunning, setIsRunning] = useState(false);
  const animation = Animated.loop(
    Animated.sequence([
      Animated.timing(props.animValue, {
        toValue: 100,
        duration: 500,
        useNativeDriver: true,
      }),
      Animated.timing(props.animValue, {
        toValue: 0,
        duration: 500,
        useNativeDriver: true,
      }),
    ]),
  );
  const animate = () => {
    if (isRunning) {
      animation.stop();
      animation.reset();
      props.animValue.setOffset(0);
      setIsRunning(false);
    } else {
      setIsRunning(true);
      animation.start();
    }
  };
  const buttons = props.labels.map((value, index) => (
    <Button label={value} onPress={props.onPresses[index]} key={index} />
  ));
  return (
    <View style={{width: '100%'}}>
      <Animated.View
        style={{
          height: 20,
          width: 20,
          margin: 10,
          backgroundColor: 'red',
          transform: [
            {
              translateX: props.animValue,
            },
          ],
        }}
      />
      <View style={{flexDirection: 'row', flexWrap: 'wrap'}}>
        <Button label="animate" onPress={animate} />
        {buttons}
      </View>
    </View>
  );
};

const MovingSquareXY = (props: {
  animValueXY: Animated.ValueXY;
  labels: string[];
  onPresses: (() => void)[];
}) => {
  const [isRunning, setIsRunning] = useState(false);
  const animation = Animated.loop(
    Animated.sequence([
      Animated.timing(props.animValueXY, {
        toValue: {x: 25, y: 25},
        duration: 500,
        useNativeDriver: true,
      }),
      Animated.timing(props.animValueXY, {
        toValue: {x: 0, y: 0},
        duration: 500,
        useNativeDriver: true,
      }),
    ]),
  );
  const animate = () => {
    if (isRunning) {
      animation.stop();
      props.animValueXY.setOffset({x: 0, y: 0});
      setIsRunning(false);
    } else {
      setIsRunning(true);
      animation.start();
    }
  };
  const buttons = props.labels.map((value, index) => (
    <Button label={value} onPress={props.onPresses[index]} key={index} />
  ));
  return (
    <View style={{width: '100%', height: 100}}>
      <Animated.View
        style={{
          height: 20,
          width: 20,
          margin: 10,
          backgroundColor: 'red',
          transform: [
            {
              translateX: props.animValueXY.x,
            },
            {translateY: props.animValueXY.y},
          ],
        }}
      />
      <View style={{flexDirection: 'row', flexWrap: 'wrap', marginTop: 20}}>
        <Button label="animate" onPress={animate} />
        {buttons}
      </View>
    </View>
  );
};

const AnimatedGetLayout = () => {
  const [isRunning, setIsRunning] = useState(false);
  const animValueXY = useRef(new Animated.ValueXY({x: 0, y: 0})).current;

  const animation = Animated.loop(
    Animated.sequence([
      Animated.timing(animValueXY, {
        toValue: {x: 80, y: 80},
        duration: 500,
        useNativeDriver: false,
      }),
      Animated.timing(animValueXY, {
        toValue: {x: 0, y: 0},
        duration: 500,
        useNativeDriver: false,
      }),
    ]),
  );

  const animate = () => {
    if (isRunning) {
      animation.stop();
      animValueXY.setOffset({x: 0, y: 0});
      setIsRunning(false);
    } else {
      setIsRunning(true);
      animation.start();
    }
  };

  return (
    <View>
      <View style={{width: '100%', height: 100, flexDirection: 'row'}}>
        <View style={{flex: 1}}>
          <Animated.View
            style={{
              height: 20,
              width: 20,
              backgroundColor: 'red',
              transform: animValueXY.getTranslateTransform(),
            }}
          />
        </View>
        <View style={{flex: 1, position: 'relative'}}>
          <Animated.View
            style={[
              animValueXY.getLayout(),
              {
                position: 'absolute',
                height: 20,
                width: 20,
                backgroundColor: 'blue',
              },
            ]}
          />
        </View>
      </View>
      <View style={{flexDirection: 'row', flexWrap: 'wrap'}}>
        <Button label="animate" onPress={animate} />
      </View>
    </View>
  );
};

const AnimatedDraggableView = () => {
  const pan = useRef(new Animated.ValueXY()).current;

  const panResponder = PanResponder.create({
    onStartShouldSetPanResponder: () => true,
    onPanResponderMove: Animated.event(
      [
        null,
        {
          dx: pan.x,
          dy: pan.y,
        },
      ],
      {
        useNativeDriver: false,
      },
    ),
    onPanResponderRelease: () => {
      Animated.spring(pan, {
        toValue: {x: 0, y: 0},
        useNativeDriver: false,
      }).start();
    },
  });

  return (
    <View
      style={{
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        height: 400,
      }}>
      <Animated.View
        {...panResponder.panHandlers}
        style={[
          pan.getLayout(),
          {backgroundColor: '#61dafb', width: 80, height: 80, borderRadius: 4},
        ]}
      />
    </View>
  );
};

const AnimatedScrollViewTestCase = () => {
  const scrollY = new Animated.Value(0);
  const translation = scrollY.interpolate({
    inputRange: [0, 200],
    outputRange: [0, 200],
    extrapolate: 'clamp',
  });

  return (
    <View
      style={{
        width: '100%',
        height: 100,
        position: 'relative',
        overflow: 'hidden',
      }}>
      <Animated.ScrollView
        style={{width: '100%', height: '100%'}}
        contentContainerStyle={{alignItems: 'center', justifyContent: 'center'}}
        scrollEventThrottle={16}
        onScroll={Animated.event(
          [{nativeEvent: {contentOffset: {y: scrollY}}}],
          {
            useNativeDriver: true,
          },
        )}>
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

const TrackingValue = () => {
  const square1Anim = useRef(new Animated.Value(50)).current;
  const square2Anim = useRef(new Animated.Value(50)).current;

  const animation = Animated.sequence([
    Animated.timing(square1Anim, {
      toValue: 0,
      duration: 1000,
      useNativeDriver: true,
    }),
    Animated.timing(square1Anim, {
      toValue: 50,
      duration: 1000,
      useNativeDriver: true,
    }),
    Animated.timing(square1Anim, {
      toValue: 0,
      duration: 1000,
      useNativeDriver: true,
    }),
    Animated.timing(square1Anim, {
      toValue: 50,
      duration: 1000,
      useNativeDriver: true,
    }),
  ]);

  const handleAnimation = () => {
    animation.reset();
    animation.start();

    const tracking = Animated.spring(square2Anim, {
      toValue: square1Anim,
      useNativeDriver: true,
      mass: 10,
    });
    tracking.start();
  };

  return (
    <Pressable style={{height: 120, width: '100%'}} onPress={handleAnimation}>
      <View style={{height: 100}}>
        <Animated.View
          style={{
            height: 40,
            width: 40,
            backgroundColor: 'red',
            transform: [{translateX: square1Anim}],
          }}
        />
        <Animated.View
          style={{
            height: 40,
            width: 40,
            backgroundColor: 'blue',
            transform: [{translateX: square2Anim}],
          }}
        />
      </View>
      <Text style={{height: 20}}>Press me to start animation</Text>
    </Pressable>
  );
};
