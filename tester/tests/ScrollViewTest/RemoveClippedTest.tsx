/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../../components';
import {useState, useMemo, useRef} from 'react';
import {FlatList, ScrollView, Text, View} from 'react-native';

export default function RemoveClippedTest() {
  const [insert, setInsert] = useState(false);
  const [changeSize, setChangeSize] = useState(false);

  const scrollViewRef = useRef<ScrollView>(null);
  const scrollToY = (y: number) => {
    if (scrollViewRef.current) {
      scrollViewRef.current.scrollTo({y, animated: true});
    }
  };

  const rows = useMemo(() => {
    const temp_rows = [];
    for (let i = 0; i < 10; i++) {
      temp_rows.push(i);
    }
    return temp_rows;
  }, []);
  return (
    <TestSuite name="RemoveClipped">
      <TestCase.Example itShould="Render ScrollView with removed clipped subviews">
        <Button onPress={() => setInsert(prev => !prev)} label="Insert" />
        <Button
          onPress={() => setChangeSize(prev => !prev)}
          label="Change Size"
        />
        <Button onPress={() => scrollToY(500)} label="Scroll To 500" />
        <Button onPress={() => scrollToY(100)} label="Scroll To 100" />

        <ScrollView
          ref={scrollViewRef}
          removeClippedSubviews
          style={{height: 200, backgroundColor: 'white'}}>
          {insert && (
            <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
          )}
          <View
            style={{
              width: 100,
              height: changeSize ? 200 : 100,
              backgroundColor: 'red',
            }}
          />
          <View
            style={[
              {
                flexDirection: 'row',
              },
            ]}>
            <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
            <View style={{width: 100, height: 100, backgroundColor: 'blue'}} />
            <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
          </View>
          {rows.map(e => (
            <Text key={e}>
              {e} Lorem ipsum dolor sit amet, consectetur adipiscibhjbng elit.
            </Text>
          ))}
          <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'blue'}} />
          <View style={{width: 100, height: 100, backgroundColor: 'green'}} />
        </ScrollView>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display items correctly when `removeClippedSubviews` property is toggled">
        <ToggleablePropTest />
      </TestCase.Example>
    </TestSuite>
  );
}

const Item = ({item, height}: any) => {
  return (
    <View testID={item}>
      <Text
        style={{
          fontSize: 32,
          backgroundColor: 'pink',
          marginRight: 10,
          height: height ? 40 : 80,
        }}>
        {item}
      </Text>
    </View>
  );
};

const ToggleablePropTest = () => {
  const [removeClippedSubviews, setRemoveClippedSubviews] = useState(false);
  const [height, setHeight] = useState(false);
  const [horizontal, setHorizontal] = useState(false);
  const [data, setData] = useState(false);

  return (
    <View style={{height: 500}}>
      <View style={{height: 220}}>
        <Text>
          removeClippedSubviews: {removeClippedSubviews ? 'true' : 'false'}
        </Text>
        <Button
          onPress={() => {
            setRemoveClippedSubviews(!removeClippedSubviews);
          }}
          label="toggle removeClippedSubviews"
        />
        <Text>horizontal: {horizontal ? 'true' : 'false'}</Text>
        <Button
          onPress={() => {
            setHorizontal(!horizontal);
          }}
          label="toggle horizontal"
        />
        <Text>filter: {data ? 'on' : 'off'}</Text>
        <Button
          onPress={() => {
            setData(!data);
          }}
          label="toggle filter"
        />
        <Text>height: {height ? 40 : 80}</Text>
        <Button
          onPress={() => {
            setHeight(!height);
          }}
          label="toggle height"
        />
      </View>
      <FlatList
        horizontal={horizontal}
        removeClippedSubviews={removeClippedSubviews}
        data={
          data
            ? [
                '1 Item',
                '3 Item',
                '5 Item',
                '7 Item',
                '9 Item',
                '11 Item',
                '13 Item',
              ]
            : [
                '1 Item',
                '2 Item',
                '3 Item',
                '4 Item',
                '5 Item',
                '6 Item',
                '7 Item',
                '8 Item',
                '9 Item',
                '10 Item',
                '11 Item',
                '12 Item',
                '13 Item',
                '14 Item',
              ]
        }
        testID="flat_list"
        keyExtractor={(item, index) => item + index}
        renderItem={({item}) => <Item item={item} height={height} />}
        style={{marginHorizontal: 15}}
      />
    </View>
  );
};
