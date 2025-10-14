/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import React, {useState, useEffect} from 'react';
import {Appearance, ColorSchemeName, Text, useColorScheme} from 'react-native';
import {Button, TestCase} from '../components';

export function AppearanceTest() {
  const [colorScheme, setColorScheme] = useState<ColorSchemeName>(
    Appearance.getColorScheme(),
  );
  const colorSchemeFromHook = useColorScheme();

  const [colorSchemeHistory, setColorSchemeHistory] = useState<
    ColorSchemeName[]
  >([Appearance.getColorScheme()]);

  useEffect(() => {
    const handleColorThemeChange = ({
      colorScheme: newColorScheme,
    }: {
      colorScheme: ColorSchemeName;
    }) => {
      setColorScheme(newColorScheme);
      setColorSchemeHistory(prev => [...prev, newColorScheme]);
    };

    Appearance.addChangeListener(handleColorThemeChange);
  }, []);

  return (
    <TestSuite name="Appearance">
      <TestCase.Logical
        itShould="return sensible value from appearance API"
        fn={({expect}) => {
          expect(Appearance.getColorScheme()).to.oneOf(['light', 'dark', null]);
        }}
      />
      <TestCase.Logical
        itShould="return sensible value from useColorScheme hook"
        fn={({expect}) => {
          expect(colorSchemeFromHook).to.be.oneOf(['light', 'dark', null]);
        }}
      />
      <ColorSchemeTestCase colorScheme="light" />
      <ColorSchemeTestCase colorScheme="dark" />
      <TestCase.Example itShould="show current colorScheme and history">
        <Button
          label="set dark"
          onPress={() => {
            Appearance.setColorScheme('dark');
          }}
        />
        <Button
          label="set light"
          onPress={() => {
            Appearance.setColorScheme('light');
          }}
        />
        <Button
          label="set null (initial colorScheme)"
          onPress={() => {
            Appearance.setColorScheme(null);
          }}
        />
        <Text>{colorScheme}</Text>
        <Text>{'useColorScheme:' + colorSchemeFromHook}</Text>
        <Text>
          {colorSchemeHistory
            .map(oldColorScheme => (oldColorScheme ? oldColorScheme : 'null'))
            .join(', ')}
        </Text>
      </TestCase.Example>
    </TestSuite>
  );
}

const ColorSchemeTestCase = ({colorScheme}: {colorScheme: ColorSchemeName}) => {
  return (
    <TestCase.Manual
      itShould={`set colorScheme to ${colorScheme}`}
      initialState={undefined as ColorSchemeName}
      arrange={({setState}) => (
        <Button
          label={`set ${colorScheme}`}
          onPress={() => {
            Appearance.setColorScheme(colorScheme);
            setState(Appearance.getColorScheme());
          }}
        />
      )}
      assert={({expect, state}) => {
        expect(state).to.be.eq(colorScheme);
      }}
    />
  );
};
