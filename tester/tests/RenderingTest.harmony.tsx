/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import React from 'react';
import {useState} from 'react';
import {
  View,
  registerViewConfig,
  ReactNativeViewAttributes,
  TouchableOpacity,
} from 'react-native';
import {Button, TestCase} from '../components';

const PropsDisplayer: typeof View = registerViewConfig('PropsDisplayer', () => {
  return {
    uiViewClassName: 'PropsDisplayer',
    bubblingEventTypes: {},
    directEventTypes: {},
    validAttributes: {
      ...ReactNativeViewAttributes.UIView,
    },
  };
});

export function RenderingTest() {
  return (
    <TestSuite name="Rendering">
      <TestCase.Example itShould="change the rectangle's color every second">
        <Timeout
          ms={1000}
          renderItem={refreshKey => (
            <Rectangle
              backgroundColor={refreshKey % 2 === 0 ? 'red' : 'green'}
            />
          )}
        />
      </TestCase.Example>
      <TestCase.Example itShould="show and hide rectangle every second">
        <Timeout
          ms={1000}
          renderItem={refreshKey => (
            <View style={{height: 64}}>
              {refreshKey % 2 === 0 ? (
                <Rectangle backgroundColor={'red'} />
              ) : null}
            </View>
          )}
        />
      </TestCase.Example>
      <TestCase.Example itShould="display green view over blue after pressing">
        <RenderOrderExample />
      </TestCase.Example>
      <TestCase.Example itShould="display all props after pressing the button, not only the recently updated one">
        <Timeout
          ms={0}
          renderItem={refreshColor => {
            return (
              <PropsDisplayer
                style={{
                  backgroundColor: refreshColor % 2 ? 'red' : 'blue',
                  opacity: 0.9,
                  borderWidth: 4,
                  borderTopWidth: 4,
                  borderColor: 'orange',
                  borderTopColor: 'red',
                  borderRadius: 4,
                  borderBottomRightRadius: 8,
                  transform: [{translateX: 1}],
                  pointerEvents: 'auto',
                  shadowOffset: {width: 0, height: 0},
                  shadowOpacity: 0.9,
                  shadowRadius: 9,
                  flexDirection: 'row',
                  overflow: 'hidden',
                  backfaceVisibility: 'visible',
                  width: '100%',
                  height: 2000,
                }}
              />
            );
          }}
        />
      </TestCase.Example>
    </TestSuite>
  );
}

function Rectangle({backgroundColor}: {backgroundColor: string}) {
  return (
    <View
      style={{
        width: 64,
        height: 64,
        backgroundColor,
      }}
    />
  );
}

function Timeout({
  renderItem,
  ms,
}: {
  renderItem: (refreshKey: number) => any;
  ms: number;
}) {
  const [refreshKey, setRefreshKey] = useState(0);

  return (
    <React.Fragment>
      <Button
        label="Run"
        onPress={() => setTimeout(() => setRefreshKey(prev => (prev += 1)), ms)}
      />
      {renderItem(refreshKey)}
    </React.Fragment>
  );
}

function RenderOrderExample() {
  const [show, setShow] = React.useState(false);
  return (
    <TouchableOpacity
      style={{
        backgroundColor: 'red',
        height: 100,
        width: 100,
      }}
      onPress={() => {
        setShow(s => !s);
      }}>
      {show ? (
        <View
          style={{
            position: 'absolute',
            backgroundColor: 'blue',
            height: 90,
            width: 90,
          }}
        />
      ) : null}
      <View style={{backgroundColor: 'green', width: 80, height: 80}} />
    </TouchableOpacity>
  );
}
