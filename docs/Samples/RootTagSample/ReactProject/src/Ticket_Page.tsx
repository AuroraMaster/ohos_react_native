/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, { useContext, useState } from 'react';
import { Text, View, DeviceEventEmitter, RootTagContext } from 'react-native';

const Ticket_Page = () => {
  const rootTag = useContext(RootTagContext);
  const [msg, setMsg] = useState('');
  DeviceEventEmitter.addListener('clickEvent', e => {
    setMsg(`The message was ${e.params.rootTag === rootTag ? '' : 'not'} sent to me.`);
    const timer = setTimeout(() => {
      setMsg('');
      clearTimeout(timer);
    }, 1000)
  })
  return (
    <View style={{ flex: 1, backgroundColor: 'green', alignItems: 'center', justifyContent: 'center' }}>
      <Text style={{ fontSize: 20, color: 'white' }}>Here is the flight ticket page.</Text>
      <Text style={{ fontSize: 20, color: 'white' }}>{msg}</Text>
    </View>
  );
};

export default Ticket_Page;
