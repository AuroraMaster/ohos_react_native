/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {Animated} from 'react-native-harmony';
import {TestCase} from '../../components';
import {AnimatedValueTestCore as AnimatedValueTestCore} from './AnimatedValueTestCore';
import {SimpleText} from 'react-native-harmony-sample-package';
import {useEffect, useRef} from 'react';

const AnimatedSimpleText = Animated.createAnimatedComponent(SimpleText);

export const AnimatedValueTest = () => {
  return (
    <AnimatedValueTestCore
      renderInterpolateTests={() => {
        return (
          <TestCase.Manual
            itShould="interpolate numbers detected in a string"
            initialState={0}
            arrange={({setState, state}) => {
              return (
                <StringInterpolationExample
                  onTextChange={text => {
                    const value = parseFloat(text.replace('foo', ''));
                    if (state < 50) {
                      // condition stops updating test all the time
                      setState(value);
                    }
                  }}
                />
              );
            }}
            assert={async ({state}) => {
              await new Promise(resolve => {
                if (state > 25 && state < 75) {
                  resolve(state);
                }
              });
            }}
          />
        );
      }}
    />
  );
};

const StringInterpolationExample = ({
  onTextChange,
}: {
  onTextChange: (text: string) => void;
}) => {
  const animValueRef = useRef(new Animated.Value(0));
  const interpolatedAnimValueRef = useRef(
    animValueRef.current.interpolate({
      inputRange: [0, 1],
      outputRange: ['0foo', '100foo'],
    }),
  );

  useEffect(() => {
    Animated.loop(
      Animated.sequence([
        Animated.timing(animValueRef.current, {
          toValue: 1,
          duration: 1000,
          useNativeDriver: true,
        }),
        Animated.timing(animValueRef.current, {
          toValue: 0,
          duration: 1000,
          useNativeDriver: true,
        }),
      ]),
    ).start();
  }, [animValueRef]);

  return (
    <Animated.View
      style={{
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
      }}>
      <AnimatedSimpleText
        style={{height: 64, width: '64%'}}
        text={interpolatedAnimValueRef.current}
        onTextChange={onTextChange}
      />
    </Animated.View>
  );
};
