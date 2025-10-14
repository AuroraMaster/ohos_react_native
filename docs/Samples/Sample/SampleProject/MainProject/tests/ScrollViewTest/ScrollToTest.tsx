/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TestSuite, TestCase } from '@rnoh/testerino';
import { View, ScrollView, StyleSheet, ScrollViewProps } from 'react-native';
import { COMMON_PROPS } from './fixtures';
import React from 'react';
import { Button } from '../../components';

export function ScrollToTest() {
  return (
    <TestSuite name="scrollTo">
      <TestCase modal itShould="scroll down on button press (no animation)">
        <ScrollToTestCase animated={false} />
      </TestCase>
      <TestCase modal itShould="scroll down on button press (with animation)">
        <ScrollToTestCase animated={true} />
      </TestCase>
      <TestCase
        modal
        itShould="call onScroll once when scrolling without animation"
        initialState={0}
        arrange={({ setState }) => (
          <ScrollToTestCase
            animated={false}
            onScroll={() => setState(c => c + 1)}
          />
        )}
        assert={({ state, expect }) => {
          expect(state).to.eq(1);
        }}
      />
      <TestCase
        modal
        itShould="call onScroll multiple times when scrolling with animation"
        initialState={0}
        arrange={AnimatedScrollToEventCountTestCase}
        assert={({ state, expect }) => {
          expect(state).to.be.greaterThan(10);
        }}
      />
    </TestSuite>
  );
}

function ScrollToTestCase({
  animated,
  onScroll,
}: { animated: boolean } & ScrollViewProps) {
  const scrollToOffset = 600;
  const scrollRef = React.useRef<ScrollView>(null);
  const scrollTo = () => {
    scrollRef.current?.scrollTo({ y: scrollToOffset, animated });
  };

  return (
    <View style={styles.wrapperView}>
      <ScrollView
        ref={scrollRef}
        {...COMMON_PROPS}
        onScroll={onScroll}
        scrollEnabled={false}
      />
      <Button label={`Scroll to ${scrollToOffset}`} onPress={scrollTo} />
    </View>
  );
}

function AnimatedScrollToEventCountTestCase({
  setState,
}: {
  setState: (state: number) => void;
}) {
  const counter = React.useRef(0);
  const done = React.useRef(false);

  return (
    <ScrollToTestCase
      animated
      onScroll={e => {
        counter.current += 1;
        if (!done.current && e.nativeEvent.contentOffset.y >= 600) {
          setState(counter.current);
          done.current = true;
        }
      }}
      scrollEnabled={false}
    />
  );
}

const styles = StyleSheet.create({
  wrapperView: {
    height: 300,
    flexDirection: 'row',
  },
});
