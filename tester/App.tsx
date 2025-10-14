/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {
  SafeAreaView,
  ScrollView,
  StatusBar,
  StyleSheet,
  View,
} from 'react-native';
import * as exampleByName from './examples';
import {NavigationContainer, Page} from './components';
import {
  Benchmarker,
  DeepTree,
  SierpinskiTriangle,
  StressTest,
} from './benchmarks';
import {PortalHost, PortalProvider} from '@gorhom/portal';
import * as testSuiteByName from './tests';
import {Tester} from '@rnoh/testerino';

const {TesterExample, ...remainingExampleByName} = exampleByName;

function App() {
  return (
    //<Environment>
    <View style={{backgroundColor: 'black'}}>
      <StatusBar barStyle="light-content" />
      <SafeAreaView>
        <NavigationContainer>
          <PortalProvider>
            <View id="__harmony::ready" />
            <Page name="BENCHMARK: Stress Tests">
              <StressTest />
            </Page>
            <Page name="_DEV Tests">
              <TesterExample filter={{tags: ['dev']}} />
            </Page>
            <Page name="AUTOMATED & MANUAL Tests">
              <TesterExample filter={{types: ['automated', 'manual']}} />
            </Page>
            {Object.keys(testSuiteByName).map(testSuiteName => {
              const TestSuite =
                testSuiteByName[testSuiteName as keyof typeof testSuiteByName];
              return (
                <Page
                  key={testSuiteName}
                  name={`${testSuiteName.replace('Test', '')}`}>
                  <Tester style={{flex: 1}}>
                    <ScrollView style={{flex: 1}}>
                      <TestSuite key={testSuiteName} />
                    </ScrollView>
                  </Tester>
                </Page>
              );
            })}
            <Page name="BENCHMARK: DEEP TREE">
              <Benchmarker
                samplesCount={0}
                renderContent={refreshKey =>
                  refreshKey % 2 === 0 ? (
                    <DeepTree depth={9} breadth={2} id={0} wrap={1} />
                  ) : null
                }
              />
            </Page>
            <Page name="BENCHMARK: DEEP TREE (20 samples)">
              <Benchmarker
                samplesCount={20}
                renderContent={refreshKey =>
                  refreshKey % 2 === 0 ? (
                    <DeepTree depth={6} breadth={2} id={0} wrap={1} />
                  ) : null
                }
              />
            </Page>
            <Page name="BENCHMARK: UPDATING COLORS">
              <Benchmarker
                samplesCount={100}
                renderContent={refreshKey => (
                  <SierpinskiTriangle
                    s={150}
                    x={150}
                    y={75}
                    depth={1}
                    renderCount={refreshKey}
                  />
                )}
              />
            </Page>
            <Page name="BENCHMARK: UPDATING LAYOUT">
              <Benchmarker
                samplesCount={200}
                renderContent={refreshKey => (
                  <SierpinskiTriangle
                    s={refreshKey}
                    x={160}
                    y={75}
                    depth={1}
                    renderCount={refreshKey}
                  />
                )}
              />
            </Page>
            {Object.entries(remainingExampleByName).map(
              ([exampleName, Example]) => {
                return (
                  <Page key={exampleName} name={`EXAMPLE: ${exampleName}`}>
                    <Example />
                  </Page>
                );
              },
            )}
            <View
              style={[
                StyleSheet.absoluteFill,
                {zIndex: 100, pointerEvents: 'box-none'},
              ]}>
              <PortalHost name="ModalHost" />
            </View>
          </PortalProvider>
        </NavigationContainer>
      </SafeAreaView>
    </View>
    //</Environment>
  );
}

export default App;
