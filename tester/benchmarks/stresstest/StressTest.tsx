/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import {useState} from 'react';
import {
  Systrace,
  ScrollView,
  Pressable,
  StyleSheet,
  Text,
  View,
  Image,
} from 'react-native';
import MeasureComponent from './MeasureComponent';

interface Scenario {
  count: number;
  elementType: string;
}

export function StressTest() {
  const [scenario, setScenario] = useState<Scenario | null>(null);
  const elementTypes: string[] = ['View', 'Text', 'Image'];
  const counts: number[] = [1500, 5000];
  const getTestCaseTitle = (count: number, elementType: string) =>
    `Render ${count} ${elementType}s`;

  return (
    <ScrollView
      contentContainerStyle={{alignItems: 'center'}}
      automaticallyAdjustContentInsets={true}
      style={{backgroundColor: 'white'}}>
      {scenario === null ? (
        elementTypes.map(elementType =>
          counts.map(count => (
            <Button
              title={getTestCaseTitle(count, elementType)}
              onPress={timestamp => {
                Systrace.beginEvent('start_test');
                setScenario({count: count, elementType: elementType});
                Systrace.endEvent();
              }}
              key={`${count}-${elementType}`}
            />
          )),
        )
      ) : (
        <>
          <Button
            onPress={() => {
              setScenario(null);
            }}
            title="Reset"
          />
          <MeasureComponent
            title={getTestCaseTitle(scenario.count, scenario.elementType)}
            startTime={performance.now()}>
            <TestComponents scenario={scenario} />
          </MeasureComponent>
        </>
      )}
    </ScrollView>
  );
}

function TestComponents(props: {scenario: Scenario}): React.ReactNode {
  return Array.from(Array(props.scenario.count).keys()).map((value, index) => {
    switch (props.scenario.elementType) {
      case 'View':
        return (
          <View
            key={index}
            style={{
              width: 100,
              height: 100,
              backgroundColor: 'red',
              margin: 5,
            }}
          />
        );
      case 'Text':
        return <Text key={index}>Nice text: {index}</Text>;
      case 'Image':
        return (
          <Image
            key={index}
            style={{width: 100, height: 100, margin: 5}}
            source={{
              uri: 'https://res6.vmallres.com/pimages/uomcdn/CN/pms/202309/gbom/6942103109584/800_800_D5053BB8A6FECB4906EC5869E78CEEC5mp.png',
            }}
          />
        );
    }
  });
}

function Button(props: {
  onPress: (timestamp: number) => void;
  title: string;
}): JSX.Element {
  return (
    <Pressable
      style={styles.button}
      onPress={event => {
        props.onPress(event.nativeEvent.timestamp);
      }}>
      <Text numberOfLines={2} style={styles.text}>
        {props.title}
      </Text>
    </Pressable>
  );
}

const styles = StyleSheet.create({
  button: {
    alignItems: 'center',
    justifyContent: 'center',
    paddingVertical: 12,
    paddingHorizontal: 32,
    borderRadius: 15,
    backgroundColor: 'grey',
    flex: 1,
    margin: 10,
  },
  text: {
    fontSize: 16,
    lineHeight: 20,
    fontWeight: 'bold',
    letterSpacing: 0.25,
    color: 'white',
  },
});
