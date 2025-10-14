/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {useEffect, useState} from 'react';
import {ActivityIndicator} from 'react-native';
import {TestCase} from '../components';

// https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/290
export function ActivityIndicatorTest() {
  return (
    <TestSuite name="ActivityIndicator">
      <TestCase.Example itShould="display small, gray and animated loading indicator">
        <ActivityIndicator style={{marginVertical: 16}} />
      </TestCase.Example>
      <TestCase.Example itShould="enable and disable loading every 500 ms">
        <Interval
          ms={500}
          render={refreshKey => {
            return (
              <ActivityIndicator
                style={{marginVertical: 16}}
                animating={refreshKey % 2 === 0}
              />
            );
          }}
        />
      </TestCase.Example>
      <TestCase.Example itShould="display red loading indicator">
        <ActivityIndicator style={{marginVertical: 16}} color={'red'} />
      </TestCase.Example>
      <TestCase.Example itShould="display a large loading indicator">
        <ActivityIndicator style={{marginVertical: 16}} size="large" />
      </TestCase.Example>
      <TestCase.Example itShould="display a loading indicator with a size of 222 pixels">
        <ActivityIndicator style={{marginVertical: 16}} size={222} />
      </TestCase.Example>
    </TestSuite>
  );
}

function Interval({
  render,
  ms,
}: {
  ms: number;
  render: (refreshKey: number) => any;
}) {
  const [refreshKey, setRefreshKey] = useState(0);

  useEffect(() => {
    const interval = setInterval(() => {
      setRefreshKey(prev => prev + 1);
    }, ms);
    return () => {
      clearInterval(interval);
    };
  }, [ms]);

  return render(refreshKey);
}
