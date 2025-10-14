/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Tester, Filter } from '@rnoh/testerino';
import * as React from 'react';
import { ScrollView, StyleSheet } from 'react-native';
import * as tests from '../tests';

export function TesterExample({ filter }: { filter: Filter }) {
  const scrollRef = React.useRef<ScrollView>(null);

  return (
    <Tester style={{ flex: 1 }} filter={filter}>
      <ScrollView style={styles.container} ref={scrollRef}>
        {Object.keys(tests).map(testSuiteName => {
          const TestSuite = tests[testSuiteName as keyof typeof tests];
          // these tests use SampleTurboModule which is not available in the tester
          if (
            testSuiteName === 'ErrorHandlingTest' ||
            testSuiteName === 'TurboModuleTest'
          ) {
            return null;
          }
          return <TestSuite key={testSuiteName} />;
        })}
      </ScrollView>
    </Tester>
  );
}

const styles = StyleSheet.create({
  container: {
    width: '100%',
    backgroundColor: '#333',
  },
});
