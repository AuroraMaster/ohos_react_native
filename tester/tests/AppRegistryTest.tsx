/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import React from 'react';
import {AppParamsContext} from '../contexts';
import {Button, TestCase} from '../components';
import {AppRegistry, Text} from 'react-native';
import {name as appName} from '../app.json';

export function AppRegistryTest() {
  const {initialProps, ...rest} = React.useContext<any>(AppParamsContext);

  const [registry, setRegistry] = React.useState(AppRegistry.getRegistry());

  return (
    <TestSuite name="AppRegistry">
      <TestCase.Logical
        itShould="receive initialParams from the native side"
        fn={({expect}) => {
          expect(initialProps).not.to.be.undefined;
        }}
      />
      <TestCase.Example itShould="set surface props">
        <Button
          label="set surface props"
          onPress={() => {
            const newProps = {
              ...rest,
              initialProps: {...initialProps, foo: 'baz'},
            };
            AppRegistry.setSurfaceProps(appName, newProps);
          }}
        />
      </TestCase.Example>
      <TestCase.Example itShould="display registry">
        <Text>{JSON.stringify(registry, null, 2)}</Text>
      </TestCase.Example>
      <TestCase.Manual
        itShould="register section when button is pressed"
        initialState={false}
        arrange={({setState}) => (
          <Button
            label="add section"
            onPress={() => {
              AppRegistry.registerSection('foo', () => () => null);
              setRegistry(AppRegistry.getRegistry());
              setState(true);
            }}
          />
        )}
        assert={({expect}) => {
          const sectionKeys = AppRegistry.getSectionKeys();
          expect(sectionKeys).to.include('foo');
        }}
      />
      <TestCase.Manual
        itShould="register runnable when button is pressed"
        initialState={false}
        arrange={({setState}) => (
          <Button
            label="add runnable"
            onPress={() => {
              AppRegistry.registerRunnable('foo', _ => {});
              setRegistry(AppRegistry.getRegistry());
              setState(true);
            }}
          />
        )}
        assert={({expect}) => {
          const runnable = AppRegistry.getRunnable('foo');
          expect(runnable).to.not.be.undefined;
        }}
      />
      <TestCase.Manual
        itShould="execute runnable when button is pressed"
        initialState={undefined}
        arrange={({setState}) => (
          <Button
            label="register and execute runnable"
            onPress={() => {
              AppRegistry.registerRunnable('bar', appParams => {
                setState(appParams);
              });
              setRegistry(AppRegistry.getRegistry());
              AppRegistry.runApplication('bar', {foo: 'bar'});
            }}
          />
        )}
        assert={({state, expect}) => {
          expect(state).to.not.be.undefined;
          expect(state).to.deep.eq({foo: 'bar'});
        }}
      />
      <TestCase.Logical
        itShould="return app keys"
        fn={({expect}) => {
          expect(AppRegistry.getAppKeys()).to.include(appName);
        }}
      />
    </TestSuite>
  );
}
