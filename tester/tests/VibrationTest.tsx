/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {Vibration} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../components';

export const VibrationTest = () => {
  const ONE_SECOND_IN_MS = 1000;
  const PATTERN = [
    1 * ONE_SECOND_IN_MS,
    1 * ONE_SECOND_IN_MS,
    1 * ONE_SECOND_IN_MS,
    2 * ONE_SECOND_IN_MS,
    1 * ONE_SECOND_IN_MS,
    3 * ONE_SECOND_IN_MS,
    1 * ONE_SECOND_IN_MS,
    4 * ONE_SECOND_IN_MS,
  ];

  return (
    <TestSuite name="Button">
      <TestCase.Example itShould="vibrate once for 400ms">
        <Button
          onPress={() => {
            Vibration.vibrate();
          }}
          label="Vibrate once"
        />
      </TestCase.Example>
      <TestCase.Example itShould="vibrate for five seconds after button press">
        <Button
          onPress={() => {
            Vibration.vibrate(ONE_SECOND_IN_MS * 5);
          }}
          label="Vibrate for five seconds"
        />
      </TestCase.Example>
      <TestCase.Example itShould="vibrate with pattern in increasing multiples of one second with a one second pause">
        <Button
          onPress={() => {
            Vibration.vibrate(PATTERN);
          }}
          label="vibrate with pattern"
        />
      </TestCase.Example>
      <TestCase.Example itShould="vibrate until cancelled">
        <Button
          onPress={() => {
            Vibration.vibrate([500, ONE_SECOND_IN_MS], true);
          }}
          label="vibrate for one second with a 0,5 s gap between each repetition until cancelled"
        />
        <Button
          onPress={() => {
            Vibration.cancel();
          }}
          label="cancel vibration"
        />
      </TestCase.Example>
    </TestSuite>
  );
};
