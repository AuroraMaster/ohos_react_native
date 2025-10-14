/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {StyleSheet, Text, View} from 'react-native';
import {TestSuite, TestCase} from '@rnoh/testerino';
import {SAMPLE_PARAGRAPH_TEXT} from './fixtures';

export function TextAccessibilityTest() {
  return (
    <TestSuite name="text accessibility">
      <TestCase
        modal
        itShould='make the screen reader say "This is accessible text"'>
        <Text accessible={true}>This is accessible text</Text>
      </TestCase>
      <TestCase
        modal
        skip // no screen reader to test it - issue not created
        itShould='make the screen reader say "Important Text"'>
        <View>
          <Text accessible importantForAccessibility={'yes'}>
            Important Text
          </Text>
          <Text accessible importantForAccessibility="no-hide-descendants">
            Ignored Text
          </Text>
        </View>
      </TestCase>
      <TestCase
        modal
        skip // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/585
        itShould='make the screen reader say "Selected, First Text" and "Second Text"'>
        <View>
          <Text accessible={true} aria-selected={true}>
            First Text
          </Text>
          <Text accessible={true} aria-selected={false}>
            Second Text
          </Text>
        </View>
      </TestCase>
      <TestCase
        modal
        skip // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/582
        itShould='make the screen reader say "Checked, First Text" and "Second Text"'>
        <View>
          <Text accessible={true} aria-checked={true}>
            First Text
          </Text>
          <Text accessible={true} aria-checked={false}>
            Second Text
          </Text>
        </View>
      </TestCase>
      <TestCase
        modal
        skip // no screen reader to test it (but implemented) - issue not created
        itShould='make the screen reader say "Hello World" even if the text is "Hello User"'>
        <Text accessible={true} accessibilityLabel="Hello World">
          Hello User
        </Text>
      </TestCase>
      <TestCase
        modal
        skip // no screen reader to test it (but implemented) - issue not created
        itShould='make the screen reader say hint "There is a hint"'>
        <Text accessible={true} accessibilityHint="There is a hint">
          Normal Text
        </Text>
      </TestCase>
      <TestCase
        modal
        skip // no screen reader to test it (but implemented) - https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/584
        itShould='make the screen reader say "Hello World" even if the text is "Hello User" - aria-label'>
        <Text accessible={true} aria-label="Hello World">
          Hello User
        </Text>
      </TestCase>
      <TestCase
        modal
        skip // no screen reader to test it (not implemented) - https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/581
        itShould='make the screen reader say/display "busy" after clicking on Loading Text'>
        <Text accessible aria-busy={true}>
          Loading
        </Text>
      </TestCase>
      <TestCase
        modal
        skip // no screen reader to test it (not implemented) - not supported yet on ArkUI (issue not created yet)
        itShould='make the screen reader say/display "disabled"'>
        <Text accessible={true} aria-disabled={true}>
          Disabled Text
        </Text>
        <Text accessible={true} aria-disabled={false}>
          Normal Text
        </Text>
      </TestCase>
      <TestCase
        modal
        skip // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/586
        itShould="render Text with different text decoration styles">
        <View>
          <Text style={[styles.styledBox, {textDecorationStyle: 'dashed'}]}>
            Text decoration style dashed
          </Text>
          <Text style={[styles.styledBox, {textDecorationStyle: 'dotted'}]}>
            Text decoration style dotted
          </Text>
          <Text style={[styles.styledBox, {textDecorationStyle: 'double'}]}>
            Text decoration style double
          </Text>
          <Text style={[styles.styledBox, {textDecorationStyle: 'solid'}]}>
            Text decoration style solid
          </Text>
        </View>
      </TestCase>
      <TestCase modal itShould="adjust text size to fit the container">
        <TextAdjustsFontSizeToFitTest />
      </TestCase>
      <TestCase
        modal
        itShould="adjust text size with max height to fit the container">
        <TextAdjustsFontSizeToFitIssueTest />
      </TestCase>
      <TestCase modal itShould="render text with allowFontScaling">
        <Text allowFontScaling>Scaled</Text>
        <Text allowFontScaling={false}>Not scaled</Text>
        <Text style={{fontSize: 40}} allowFontScaling>
          Scaled big
        </Text>
        <Text style={{fontSize: 40}} allowFontScaling={false}>
          Not scaled big
        </Text>
      </TestCase>
    </TestSuite>
  );
}

const TextAdjustsFontSizeToFitTest = () => {
  const testCases = [
    {adjustsFontSizeToFit: true, minimumFontScale: 0.1},
    {adjustsFontSizeToFit: true, minimumFontScale: 0.5},
    {adjustsFontSizeToFit: true, minimumFontScale: 0.9},
    {adjustsFontSizeToFit: false, minimumFontScale: 0.2},
  ];
  return (
    <View style={{height: 400, marginTop: 50}}>
      {testCases.map((testCase, index) => (
        <View key={index} style={{backgroundColor: 'purple'}}>
          <Text {...testCase} style={{fontSize: 50}} numberOfLines={1}>
            4x Very long text Very long text Very long text Very long text
          </Text>
        </View>
      ))}
    </View>
  );
};

const TextAdjustsFontSizeToFitIssueTest = () => {
  // this case crashed app when RNSimpleText used TextHeightAdaptivePolicy.LAYOUT_CONSTRAINT_FIRST
  return (
    <View style={{height: 150, marginTop: 50}}>
      <Text
        adjustsFontSizeToFit
        style={{maxHeight: 50, backgroundColor: 'blue'}}>
        {SAMPLE_PARAGRAPH_TEXT}
      </Text>
      <Text
        adjustsFontSizeToFit
        minimumFontScale={0.99}
        style={{maxHeight: 50, backgroundColor: 'green'}}>
        {SAMPLE_PARAGRAPH_TEXT}
      </Text>
    </View>
  );
};

const styles = StyleSheet.create({
  styledBox: {
    width: '100%',
    padding: 10,
    marginTop: 16,
    marginBottom: 16,
    textDecorationColor: 'red',
    textDecorationLine: 'underline',
  },
});
