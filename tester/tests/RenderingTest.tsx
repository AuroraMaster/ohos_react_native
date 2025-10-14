/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {TestSuite} from '@rnoh/testerino';
import {Text} from 'react-native';

export function RenderingTest() {
  return (
    <TestSuite name="Rendering">
      <Text style={{height: 20, color: 'white'}}>Platform not supported</Text>
    </TestSuite>
  );
}
