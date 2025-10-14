/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {DeviceEventEmitter, Systrace, View, Alert} from 'react-native';

let drawnTss: number[] = []; // timestamps of when this component's native draw event fired

DeviceEventEmitter.addListener('benchmarkComponentDrawn', e => {
  Systrace.beginEvent('benchmarkComponentDrawn');
  // TODO: performance.now only have ms level accuracy, no decimals, sub ms accuracy solutions?
  drawnTss.push(performance.now());
  Systrace.endEvent();
});

export default function MeasureComponent(props: {
  children: React.ReactNode;
  title: string;
  startTime: number;
}): JSX.Element {
  // alert test result
  setTimeout(() => {
    let took = (drawnTss[1] - props.startTime).toFixed(3);
    Systrace.beginEvent(`benchmarkAlerting: ${took}`);
    drawnTss.sort();
    Alert.alert(props.title, took + ' ms', [{text: 'OK'}]);
    drawnTss = [];
    Systrace.endEvent();
  }, 1500);

  return (
    <View>
      <View
        style={{margin: 10, height: 5, width: '100%', backgroundColor: 'black'}}
      />
      <View nativeID={'rnohBenchmarkComponentId'}>{props.children}</View>
    </View>
  );
}
