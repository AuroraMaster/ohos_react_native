/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TestCase, TestSuite } from '@rnoh/testerino';
import React from 'react';
import { StyleSheet, View, DevSettings } from 'react-native';
import { Button } from '../components';

export const DevSettingsTest = () => {
  return (
    <TestSuite name="DevSettings">
      <TestCase
        itShould="add custom DevMenu item and react to presses after opening DevMenu"
        initialState={false}
        arrange={({ setState }) => <AddMenuItemTest setState={setState} />}
        assert={({ state, expect }) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase itShould="reload the app after pressing the button">
        <View style={styles.container}>
          <Button
            label="Reload application"
            onPress={() => DevSettings.reload('Test reload')}
          />
        </View>
      </TestCase>
    </TestSuite>
  );
};

const AddMenuItemTest = (props: {
  setState: React.Dispatch<React.SetStateAction<boolean>>;
}) => {
  const customMenuItemOnPress = () => {
    props.setState(true);
  };
  const addCustomMenuItem = () => {
    DevSettings.addMenuItem('Custom menu item', customMenuItemOnPress);
  };
  return (
    <View style={styles.container}>
      <Button label="Add custom menu item" onPress={addCustomMenuItem} />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    height: 50,
    justifyContent: 'center',
  },
});
