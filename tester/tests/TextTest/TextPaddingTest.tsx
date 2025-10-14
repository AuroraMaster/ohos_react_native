/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {Platform, Text, View} from 'react-native';
import {TestSuite, TestCase} from '@rnoh/testerino';
import {PressCounter} from '../../components';

export function TextPaddingTest() {
  return (
    <TestSuite name="padding">
      <TestCase itShould="preserve a gap of atleast 24 pixels from each container edge">
        <View style={{width: 300}}>
          <Text style={{padding: 24, borderWidth: 1}}>
            <Text style={{color: 'blue'}}>Fragment1</Text>
            {
              'Fragment2 Fragment2 Fragment2 Fragment2 Fragment2 Fragment2 Fragment2 '
            }
          </Text>
          <Text style={{padding: 24, borderWidth: 1}}>
            Fragment1 Fragment1 Fragment1 Fragment1 Fragment1 Fragment1
            Fragment1 Fragment1
          </Text>
        </View>
      </TestCase>
      <TestCase
        skip={Platform.select({
          android: 'not supported',
          harmony: 'RN problem, also fails on Android',
        })}
        itShould="display the attachment inline with text">
        <Text style={{padding: 20}}>
          Fragment1ii
          <PressCounter />
        </Text>
      </TestCase>
      <TestCase
        skip // Inserting zero-width space in RNSimpleText that fixes this issue breaks wrapping. This problem needs to be fixed on ArkUI's side.
        itShould="insert ellipsis at same location">
        <View style={{width: 300}}>
          <Text
            style={{
              paddingRight: 10,
              paddingLeft: 10,
              fontSize: 16,
              width: 150,
              backgroundColor: 'red',
            }}
            numberOfLines={1}
            ellipsizeMode="tail">
            {'fragment1'} fragment2
          </Text>
          <Text
            style={{
              paddingRight: 10,
              paddingLeft: 10,
              fontSize: 16,
              width: 150,
              backgroundColor: 'red',
            }}
            numberOfLines={1}
            ellipsizeMode="tail">
            fragment1 fragment2
          </Text>
        </View>
      </TestCase>
    </TestSuite>
  );
}
