/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import { TestSuite, TestCase } from '@rnoh/testerino';

export function CustomNativeComponentTest() {
  return (
    <TestSuite name="Custom Native Component">
      <TestCase itShould="platform not supported" fn={() => { }} />
    </TestSuite>
  );
}
