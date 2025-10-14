/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {useState} from 'react';
import {Button} from 'react-native';

export function PressCounter() {
  const [counter, setCounter] = useState(0);
  return (
    <Button
      title={`Click (${counter})`}
      onPress={() => {
        setCounter(prev => prev + 1);
      }}
    />
  );
}
