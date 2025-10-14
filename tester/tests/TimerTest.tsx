/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import React, {useEffect} from 'react';
import {AppState, AppStateStatus, Text, View} from 'react-native';
import {Button, TestCase} from '../components';

async function wait(ms: number) {
  return new Promise(resolve => {
    setTimeout(resolve, ms);
  });
}

const PRECISION_IN_MS = 100;

export function TimerTest() {
  return (
    <TestSuite name="Timer">
      <TestCase.Manual
        itShould="take three seconds to finish this test (setTimeout, 3s)"
        initialState={0}
        arrange={({setState}) => {
          return (
            <Button
              label="Start Timeout"
              onPress={() => {
                setState(prev => prev + 1);
              }}
            />
          );
        }}
        assert={async ({expect}) => {
          const waitTimeInMs = 3000;
          const time1 = new Date().getTime();
          await wait(waitTimeInMs);
          const time2 = new Date().getTime();

          expect(time1).to.be.greaterThan(0);
          expect(time2).to.be.greaterThan(0);
          expect(Math.abs(time2 - time1 - waitTimeInMs)).to.be.lessThan(
            PRECISION_IN_MS,
          );
        }}
      />
      <TestCase.Manual
        itShould="take a short time to finish this test (setTimeout, 0ms)"
        initialState={0}
        arrange={({setState}) => {
          return (
            <Button
              label="Start Timeout"
              onPress={() => {
                setState(prev => prev + 1);
              }}
            />
          );
        }}
        assert={async ({expect}) => {
          const waitTimeInMs = 0;
          const time1 = new Date().getTime();
          await wait(waitTimeInMs);
          const time2 = new Date().getTime();

          expect(time1).to.be.greaterThan(0);
          expect(time2).to.be.greaterThan(0);
          expect(Math.abs(time2 - time1 - waitTimeInMs)).to.be.lessThan(
            PRECISION_IN_MS,
          );
        }}
      />
      <TestCase.Manual
        itShould="take three seconds to finish this test (setInterval)"
        initialState={0}
        arrange={({setState}) => {
          return (
            <Button
              label="Start Interval"
              onPress={() => {
                setState(prev => prev + 1);
              }}
            />
          );
        }}
        assert={async ({expect}) => {
          await wait(3000);
          let i = 0;
          const time1 = new Date().getTime();

          await new Promise(resolve => {
            setInterval(() => {
              if (i === 1) {
                resolve(null);
              }
              i++;
            }, 1000);
          });

          const time2 = new Date().getTime();
          expect(Math.abs(time2 - time1 - 2000)).to.be.lessThan(
            PRECISION_IN_MS,
          );
        }}
      />
      <TestCase.Manual
        itShould="cancel the timer before it's triggered"
        initialState={0}
        arrange={({setState}) => {
          return (
            <Button
              label="Start test"
              onPress={() => {
                setState(prev => prev + 1);
              }}
            />
          );
        }}
        assert={async ({expect}) => {
          let finished = 0;
          const timerId = setTimeout(() => {
            finished = 2;
          }, 1000);

          setTimeout(() => {
            clearTimeout(timerId);
            finished = 1;
          }, 500);

          await wait(2000);
          expect(finished).to.be.eq(1);
        }}
      />
      <TestCase.Manual<{date: Date; appStateStatus: AppStateStatus}[]>
        modal
        itShould="not trigger updates when the application is in background"
        initialState={[]}
        arrange={({state, setState}) => {
          return (
            <Effect
              onEffect={() => {
                const interval = setInterval(() => {
                  setState(prev => [
                    ...prev,
                    {
                      date: new Date(),
                      appStateStatus: AppState.currentState,
                    },
                  ]);
                }, 1000);
                return () => clearInterval(interval);
              }}>
              <Text>
                {JSON.stringify(
                  state.reduce(
                    (acc, tick) => {
                      return {
                        ...acc,
                        [tick.appStateStatus]: acc[tick.appStateStatus] + 1,
                      };
                    },
                    {
                      active: 0,
                      background: 0,
                      extension: 0,
                      inactive: 0,
                      unknown: 0,
                    } as Record<AppStateStatus, number>,
                  ),
                )}
              </Text>
            </Effect>
          );
        }}
        assert={({expect, state}) => {
          expect(
            state.filter(tick => tick.appStateStatus !== 'active').length,
          ).to.be.eq(0);
        }}
      />
      <TestCase.Example itShould="display the average delay of timer">
        <DelayExample />
      </TestCase.Example>
    </TestSuite>
  );
}

function DelayExample() {
  const DURATION = 350;
  const [iterations, setIterations] = React.useState(0);
  const [delay, setDelay] = React.useState(0);
  const [lastDelay, setLastDelay] = React.useState(0);

  useEffect(() => {
    const now = new Date().getTime();
    const id = setTimeout(() => {
      const elapsed = new Date().getTime() - now;
      const currentDelay = elapsed - DURATION;
      setLastDelay(currentDelay);
      setDelay(prev => prev + currentDelay);
      setIterations(prev => prev + 1);
    }, DURATION);
    return () => clearTimeout(id);
  }, [iterations]);

  return (
    <View>
      <Text>Iterations: {iterations}</Text>
      <Text>Total delay: {delay}</Text>
      <Text>Last delay: {lastDelay}</Text>
      <Text>Avg. delay: {delay / iterations}</Text>
    </View>
  );
}

function Effect({
  onEffect,
  children,
}: {
  onEffect: () => void | (() => void);
  children: any;
}) {
  useEffect(onEffect, []);
  return children;
}
