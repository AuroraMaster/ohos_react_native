/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import * as React from 'react';
import {ScrollView, Text, TextInput, View} from 'react-native';

const ScrollViewContentItem = ({children}: {children?: React.ReactNode}) => {
  return (
    <View
      style={{
        height: 48,
        width: '100%',
        borderBottomWidth: 1,
      }}>
      {children}
    </View>
  );
};

export function KeyboardShouldPersistTapsExample() {
  return (
    <View>
      <ScrollViewContentItem>
        <TextInput
          style={{padding: 16, backgroundColor: 'lightgray'}}
          placeholder="Input outside scroll views..."
        />
      </ScrollViewContentItem>
      <ScrollView
        style={{backgroundColor: 'white'}}
        keyboardShouldPersistTaps="never">
        <ScrollViewContentItem>
          <TextInput
            style={{padding: 16}}
            placeholder="Input inside scroll with white background..."
          />
        </ScrollViewContentItem>
        <ScrollViewContentItem>
          <Text style={{padding: 16}}>On click hides the keyboard</Text>
        </ScrollViewContentItem>
        <ScrollViewContentItem>
          <Text style={{padding: 16}}>On click hides the keyboard</Text>
        </ScrollViewContentItem>
      </ScrollView>
      <ScrollView
        style={{backgroundColor: 'lightblue'}}
        keyboardShouldPersistTaps={'always'}>
        <ScrollViewContentItem>
          <Text style={{padding: 16}}>On click keyboard remains open</Text>
        </ScrollViewContentItem>
        <ScrollViewContentItem>
          <Text style={{padding: 16}}>On click keyboard remains open</Text>
        </ScrollViewContentItem>
        <ScrollViewContentItem>
          <TextInput
            style={{padding: 16}}
            placeholder="Input inside scroll view with lightblue background..."
          />
        </ScrollViewContentItem>
        <ScrollViewContentItem>
          <Text style={{padding: 16}}>On click keyboard remains open</Text>
        </ScrollViewContentItem>
      </ScrollView>
    </View>
  );
}
