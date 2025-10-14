/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {useEffect, useState} from 'react';
import {Text, TouchableOpacity, View} from 'react-native';

export function Benchmarker({
  samplesCount,
  renderContent,
}: {
  renderContent: (refreshKey: number) => any;
  samplesCount: number;
}) {
  const [status, setStatus] = useState<'READY' | 'RUNNING' | 'FINISHED'>(
    'READY',
  );
  const [refreshKey, setRefreshKey] = useState(0);
  const [_startDateTime, setStartDateTime] = useState<Date>();
  const [_endDateTime, setEndDateTime] = useState<Date>();

  const durationInMs =
    _startDateTime && _endDateTime
      ? _endDateTime.getTime() - _startDateTime.getTime()
      : 0;

  function start() {
    setStartDateTime(new Date());
    setStatus('RUNNING');
    setRefreshKey(0);
  }

  useEffect(() => {
    if (status === 'READY') {
      return;
    }
    setTimeout(() => {
      setRefreshKey(prevKey => {
        if (prevKey >= samplesCount) {
          setStatus('FINISHED');
          setEndDateTime(new Date());
          return prevKey;
        }
        setStatus('RUNNING');
        return prevKey + 1;
      });
    }, 0);
  }, [refreshKey, status, samplesCount]);

  return (
    <View style={{height: '100%', padding: 16, backgroundColor: 'white'}}>
      <TouchableOpacity onPress={start}>
        <Text
          style={{
            width: 200,
            height: 32,
            fontWeight: 'bold',
            color: status !== 'RUNNING' ? 'blue' : 'black',
          }}>
          {status === 'RUNNING' ? 'Running...' : 'Start'}
        </Text>
      </TouchableOpacity>

      <View>
        <Text style={{width: 128, height: 32}}>Duration {durationInMs} ms</Text>
      </View>

      {status !== 'READY' && (
        <View style={{height: 600}}>{renderContent(refreshKey)}</View>
      )}
    </View>
  );
}
