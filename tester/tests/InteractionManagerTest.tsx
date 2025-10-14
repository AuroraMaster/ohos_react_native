/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {InteractionManager} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../components';

export function InteractionManagerTest() {
  return (
    <TestSuite name="InteractionManager">
      <TestCase.Logical
        itShould="execute callback to be executed without crashing"
        fn={async () => {
          return new Promise(resolve => {
            InteractionManager.runAfterInteractions(resolve);
          });
        }}
      />
    </TestSuite>
  );
}
