/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {
  AccessibilityRole,
  Platform,
  Role,
  Text,
  TextInput,
  View,
} from 'react-native';
import {TestCase} from '../../components';
import React from 'react';

const ANNOUNCEMENT_BY_PLATFORM_BY_ACCESSIBILITY_ROLE = new Map<
  AccessibilityRole,
  Record<'harmony' | 'android', string | undefined>
>()
  .set('none', {android: undefined, harmony: undefined})
  .set('button', {android: 'button', harmony: 'button'})
  .set('togglebutton', {
    android: 'switch',
    harmony: 'not ticked, that is(?) button',
  })
  .set('link', {android: 'link', harmony: undefined})
  .set('search', {android: 'edit box', harmony: 'edit box'})
  .set('image', {android: 'image', harmony: 'image'})
  .set('keyboardkey', {android: 'keyboardkey', harmony: undefined})
  .set('text', {android: 'text', harmony: undefined})
  .set('adjustable', {
    android: 'slider',
    harmony: 'null %, double tap and hold with one finger...',
  })
  .set('imagebutton', {android: 'button, image', harmony: 'button'})
  .set('header', {android: 'header, heading', harmony: undefined})
  .set('summary', {android: 'summary', harmony: undefined})
  .set('alert', {android: 'alert', harmony: undefined})
  .set('checkbox', {android: 'checkbox', harmony: 'not ticked, checkbox'})
  .set('combobox', {android: 'combobox', harmony: undefined})
  .set('menu', {android: 'menu', harmony: undefined})
  .set('menubar', {android: 'menubar', harmony: undefined})
  .set('menuitem', {android: 'menuitem', harmony: undefined})
  .set('progressbar', {android: 'progressbar', harmony: 'null %, progressbar'})
  .set('radio', {android: 'radio button', harmony: 'option button'})
  .set('radiogroup', {android: 'radio group', harmony: undefined})
  .set('scrollbar', {android: 'scrollbar', harmony: 'scrollbar'})
  .set('spinbutton', {android: 'spin button', harmony: undefined})
  .set('switch', {android: 'switch', harmony: 'not ticked, that is (?) button'})
  .set('tab', {android: 'tab', harmony: undefined})
  .set('tablist', {android: 'tablist', harmony: undefined})
  .set('timer', {android: 'timer', harmony: undefined})
  .set('list', {android: 'list', harmony: 'list'})
  .set('toolbar', {android: 'toolbar', harmony: undefined});

const ANNOUNCEMENT_BY_PLATFORM_BY_ROLE = new Map<
  Role,
  Record<'harmony' | 'android', string | undefined>
>()
  .set('alert', {android: 'alert', harmony: undefined})
  .set('alertdialog', {android: undefined, harmony: undefined})
  .set('application', {android: undefined, harmony: undefined})
  .set('article', {android: undefined, harmony: undefined})
  .set('banner', {android: undefined, harmony: undefined})
  .set('button', {android: 'button', harmony: 'button'})
  .set('cell', {android: undefined, harmony: undefined})
  .set('checkbox', {android: 'checkbox', harmony: 'not ticked, checkbox'})
  .set('columnheader', {android: undefined, harmony: undefined})
  .set('combobox', {android: 'combobox', harmony: undefined})
  .set('complementary', {android: undefined, harmony: undefined})
  .set('contentinfo', {android: undefined, harmony: undefined})
  .set('definition', {android: undefined, harmony: undefined})
  .set('dialog', {android: undefined, harmony: undefined})
  .set('directory', {android: undefined, harmony: undefined})
  .set('document', {android: undefined, harmony: undefined})
  .set('feed', {android: undefined, harmony: undefined})
  .set('figure', {android: undefined, harmony: undefined})
  .set('form', {android: undefined, harmony: undefined})
  .set('grid', {android: 'grid', harmony: undefined})
  .set('group', {android: undefined, harmony: undefined})
  .set('heading', {android: '<content> heading', harmony: undefined})
  .set('img', {android: 'unlabeled image', harmony: 'image'})
  .set('link', {android: 'link', harmony: undefined})
  .set('list', {android: 'list', harmony: 'list'})
  .set('listitem', {android: undefined, harmony: undefined})
  .set('log', {android: undefined, harmony: undefined})
  .set('main', {android: undefined, harmony: undefined})
  .set('marquee', {android: undefined, harmony: undefined})
  .set('math', {android: undefined, harmony: undefined})
  .set('menu', {android: 'menu', harmony: undefined})
  .set('menubar', {android: 'menubar', harmony: undefined})
  .set('menuitem', {android: 'menuitem', harmony: undefined})
  .set('meter', {android: undefined, harmony: undefined})
  .set('navigation', {android: undefined, harmony: undefined})
  .set('none', {android: undefined, harmony: undefined})
  .set('note', {android: undefined, harmony: undefined})
  .set('option', {android: undefined, harmony: undefined})
  .set('presentation', {android: undefined, harmony: undefined})
  .set('progressbar', {android: 'progressbar', harmony: 'null %, progressbar'})
  .set('radio', {android: 'radiobutton', harmony: 'option button'})
  .set('radiogroup', {android: 'radiogroup', harmony: undefined})
  .set('region', {android: undefined, harmony: undefined})
  .set('row', {android: undefined, harmony: undefined})
  .set('rowgroup', {android: undefined, harmony: undefined})
  .set('rowheader', {android: undefined, harmony: undefined})
  .set('scrollbar', {android: 'scrollbar', harmony: 'scrollbar'})
  .set('searchbox', {android: 'edit box', harmony: 'edit box'})
  .set('separator', {android: undefined, harmony: undefined})
  .set('slider', {
    android: 'slider',
    harmony:
      'null %, double tap and hold with one finger and swipe left or right to adjust the value',
  })
  .set('spinbutton', {android: 'spinbutton', harmony: undefined})
  .set('status', {android: undefined, harmony: undefined})
  .set('summary', {android: 'summary', harmony: undefined})
  .set('switch', {
    android: '<state> switch',
    harmony: 'not ticked, that is button',
  })
  .set('tab', {android: 'tab', harmony: undefined})
  .set('table', {android: undefined, harmony: undefined})
  .set('tablist', {android: 'tablist', harmony: undefined})
  .set('tabpanel', {android: undefined, harmony: undefined})
  .set('term', {android: undefined, harmony: undefined})
  .set('timer', {android: 'timer', harmony: undefined})
  .set('toolbar', {android: 'toolbar', harmony: undefined})
  .set('tooltip', {android: undefined, harmony: undefined})
  .set('tree', {android: undefined, harmony: undefined})
  .set('treegrid', {android: undefined, harmony: undefined})
  .set('treeitem', {android: undefined, harmony: undefined});

export function ViewAccessibilityTest() {
  return (
    <TestSuite name="Accessibility props">
      <TestSuite name="accessible">
        <TestCase.Example itShould="make screen reader gain focus on the cyan container, but not on the pink container">
          <View
            style={{padding: 32, backgroundColor: 'cyan'}}
            accessible={true}>
            <Text style={{backgroundColor: 'red', padding: 16}}>
              The cyan container is accessible...
            </Text>
          </View>
          <View
            style={{padding: 32, backgroundColor: 'pink'}}
            accessible={false}>
            <Text style={{backgroundColor: 'red', padding: 16}}>
              ...and the pink container is not.
            </Text>
          </View>
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="accessibilityLabel">
        <TestCase.Example itShould="make the screen reader announce 'I am reading accessibilityLabel content' after focusing the view below in accessibility mode">
          <View
            accessible={true}
            accessibilityLabel="I am reading accessibilityLabel content"
            style={{width: '100%', padding: 16, backgroundColor: 'lightblue'}}>
            <Text importantForAccessibility="no">1. Enable ScreenReader</Text>
            <Text importantForAccessibility="no">2. Focus on this View</Text>
          </View>
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="aria-label">
        <TestCase.Example itShould="make the screen reader announce 'I am reading aria-label content' after focusing the view below in accessibility mode">
          <View
            accessible={true}
            aria-label="I am reading aria-label content"
            style={{width: '100%', padding: 16, backgroundColor: 'lightblue'}}>
            <Text importantForAccessibility="no">1. Enable ScreenReader</Text>
            <Text importantForAccessibility="no">2. Focus on this View</Text>
          </View>
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="accessibilityState">
        <TestSuite name="checked">
          <TestCase.Example
            itShould={
              Platform.OS === 'harmony'
                ? 'announce "ticked option tick box"'
                : "announce 'checked option'"
            }>
            <View
              style={{padding: 8}}
              accessible={true}
              accessibilityLabel="option"
              accessibilityRole={Platform.select({harmony: 'checkbox'})}
              accessibilityState={{checked: true}}>
              <Text>1. Enable screen reader</Text>
              <Text>2. Focus on this view</Text>
            </View>
          </TestCase.Example>
        </TestSuite>
        <TestSuite name="disabled">
          <TestCase.Example
            itShould={
              Platform.OS === 'harmony'
                ? "announce 'option button disabled'"
                : "announce 'option disabled'"
            }>
            <View
              style={{padding: 8}}
              accessible={true}
              accessibilityLabel="option"
              accessibilityRole={Platform.select({
                harmony: 'button',
              })}
              accessibilityState={{disabled: true}}>
              <Text>1. Enable screen reader</Text>
              <Text>2. Focus on this view</Text>
            </View>
          </TestCase.Example>
        </TestSuite>
        <TestSuite name="selected">
          <TestCase.Example
            itShould={
              Platform.OS === 'harmony'
                ? "announce 'selected option button'"
                : "announce 'selected option'"
            }>
            <View
              style={{padding: 8}}
              accessible={true}
              accessibilityLabel="option"
              accessibilityRole="button"
              accessibilityState={{selected: true}}>
              <Text>1. Enable screen reader</Text>
              <Text>2. Focus on this view</Text>
            </View>
          </TestCase.Example>
        </TestSuite>
      </TestSuite>
      <TestSuite name="aria-disabled">
        <TestCase.Example
          itShould={
            Platform.OS === 'harmony'
              ? "announce 'option button disabled'"
              : "announce 'option disabled'"
          }>
          <View
            style={{padding: 8}}
            accessible={true}
            aria-disabled={true}
            role={Platform.select({
              harmony: 'button',
            })}
            accessibilityLabel="option">
            <Text>1. Enable screen reader</Text>
            <Text>2. Focus on this view</Text>
          </View>
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="aria-hidden">
        <TestCase.Example itShould="only announce 'aria hidden false' after gaining focus on a view wrapping both texts">
          <View accessible={true}>
            <View style={{padding: 16}}>
              <Text>aria-hidden: false</Text>
            </View>
            <View
              style={{padding: 16, backgroundColor: '#EEE'}}
              aria-hidden={true}>
              <Text>aria-hidden: true</Text>
            </View>
          </View>
        </TestCase.Example>
        <TestCase.Example itShould="be able to gain focus only on the second item">
          <Text>View: aria-hidden=true</Text>
          <View
            style={{flex: 1, borderColor: 'silver', borderWidth: 5}}
            accessible={true}
            aria-hidden={true}>
            <Text style={{padding: 10}}>This item shouldn't be focusable</Text>
          </View>
          <Text style={{paddingTop: 16}}>View: aria-hidden=false</Text>
          <View
            style={{flex: 1, borderColor: 'silver', borderWidth: 5}}
            accessible={true}
            aria-hidden={false}>
            <Text style={{padding: 10}}>
              This item should be focusable and announced
            </Text>
          </View>
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="aria-selected">
        <TestCase.Example
          itShould={
            Platform.OS === 'harmony'
              ? "announce 'selected option button'"
              : "announce 'selected option'"
          }>
          <View
            style={{padding: 8}}
            accessible={true}
            accessibilityLabel="option"
            accessibilityRole="button"
            aria-selected={true}>
            <Text>1. Enable screen reader</Text>
            <Text>2. Focus on this view</Text>
          </View>
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="accessibilityActions">
        <TestSuite name="activate">
          <TestCase.Manual
            itShould="pass when view below is activated by accessibility stuff"
            skip={{android: false, harmony: 'Not supported'}}
            initialState={false}
            arrange={({setState}) => {
              return (
                <View
                  accessible={true}
                  accessibilityLabel="activate action tester"
                  role="button"
                  style={{width: '100%', padding: 16}}
                  accessibilityActions={[{name: 'activate'}]}
                  onAccessibilityAction={e => {
                    if (e.nativeEvent.actionName === 'activate') {
                      setState(true);
                    }
                  }}>
                  <Text importantForAccessibility="no">
                    1. Enable ScreenReader
                  </Text>
                  <Text importantForAccessibility="no">
                    2. Focus on this View
                  </Text>
                  <Text importantForAccessibility="no">
                    3. Activate it by double tapping
                  </Text>
                </View>
              );
            }}
            assert={({expect, state}) => {
              expect(state).to.be.true;
            }}
          />
        </TestSuite>
        <TestSuite name="longpress">
          <TestCase.Manual
            skip={{android: "couldn't make it work", harmony: 'Not supported'}}
            itShould="pass when the user places accessibility focus on the component, then double-taps and holds one finger on the screen"
            initialState={false}
            arrange={({setState}) => {
              return (
                <View
                  accessible={true}
                  role="button"
                  style={{width: '100%', padding: 16}}
                  accessibilityActions={[{name: 'longpress'}]}
                  onAccessibilityAction={e => {
                    if (e.nativeEvent.actionName === 'longpress') {
                      setState(true);
                    }
                  }}>
                  <Text importantForAccessibility="no">
                    1. Enable ScreenReader
                  </Text>
                  <Text importantForAccessibility="no">
                    2. Focus on this View
                  </Text>
                  <Text importantForAccessibility="no">
                    3. Activate it by double tapping
                  </Text>
                </View>
              );
            }}
            assert={({expect, state}) => {
              expect(state).to.be.true;
            }}
          />
        </TestSuite>
        <TestSuite name="copy">
          <TestCase.Manual
            skip={{android: true, harmony: false}}
            itShould="pass after double tapping with three fingers when the TextInput is focused"
            initialState={false}
            arrange={({setState}) => {
              return (
                <TextInput
                  value="content to copy"
                  accessible={true}
                  style={{
                    width: '100%',
                    padding: 16,
                    borderColor: 'gray',
                    borderWidth: 2,
                    fontSize: 12,
                  }}
                  accessibilityActions={[{name: 'copy'}]}
                  onAccessibilityAction={e => {
                    if (e.nativeEvent.actionName === 'copy') {
                      setState(true);
                    }
                  }}
                />
              );
            }}
            assert={({expect, state}) => {
              expect(state).to.be.true;
            }}
          />
        </TestSuite>
      </TestSuite>
      <TestSuite name="aria-valuenow/min/max">
        <TestCase.Manual<number[]>
          initialState={[50]}
          skip={{android: false, harmony: 'Not supported'}}
          itShould="announce values only in range from 0 to 100 and stop incrementing or decrementing values if the current value is equal to aria-valuemax or aria-valuemin"
          arrange={({setState, state}) => {
            return (
              <View
                style={{width: '100%', padding: 16}}
                accessible={true}
                accessibilityRole="adjustable"
                accessibilityActions={[
                  {name: 'increment'},
                  {name: 'decrement'},
                ]}
                onAccessibilityAction={event => {
                  switch (event.nativeEvent.actionName) {
                    case 'increment': {
                      setState(prev => {
                        return [...prev, prev[prev.length - 1] + 25];
                      });
                      break;
                    }
                    case 'decrement': {
                      setState(prev => {
                        return [...prev, prev[prev.length - 1] - 25];
                      });
                      break;
                    }
                  }
                }}
                aria-valuemax={100}
                aria-valuemin={0}
                aria-valuenow={state[state.length - 1]}>
                <Text importantForAccessibility="no">
                  1. Enable ScreenReader
                </Text>
                <Text importantForAccessibility="no">
                  2. Focus on this View
                </Text>
                <Text importantForAccessibility="no">
                  3. Swipe up or down to increment or decrement the value
                </Text>
                <Text importantForAccessibility="no">
                  Current Fake Slider value: {state[state.length - 1]}
                </Text>
              </View>
            );
          }}
          assert={({state}) => {
            return new Promise(resolve => {
              if (state.includes(0) && state.includes(100)) {
                resolve(undefined);
              }
            });
          }}
        />
      </TestSuite>
      <TestSuite name="aria-valuetext">
        <TestCase.Example
          skip={{android: false, harmony: 'Not supported'}}
          itShould="render a view with aria-valuetext accessibility prop">
          <View
            aria-valuetext={'announcing value from aria-valuetext'}
            style={{width: '100%', padding: 16}}>
            <Text importantForAccessibility="no">1. Enable ScreenReader</Text>
            <Text importantForAccessibility="no">2. Focus on this View</Text>
          </View>
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="accessibilityRole">
        <TestCase.Example itShould="announce text specified in the appropriate column">
          <View style={{padding: 16, flexDirection: 'row', width: '100%'}}>
            <Text style={{width: '33%', fontWeight: 'bold'}}>Role</Text>
            <Text style={{width: '33%', fontWeight: 'bold'}}>Android</Text>
            <Text style={{width: '33%', fontWeight: 'bold'}}>Harmony</Text>
          </View>
          {Array.from(
            ANNOUNCEMENT_BY_PLATFORM_BY_ACCESSIBILITY_ROLE.entries(),
          ).map(([role, {android, harmony}]) => {
            return (
              <View
                key={role}
                accessible
                accessibilityRole={role}
                style={{
                  padding: 16,
                  flexDirection: 'row',
                  width: '100%',
                  borderBottomWidth: 1,
                  borderColor: 'silver',
                }}>
                <Text style={{width: '33%'}} importantForAccessibility="no">
                  {role}
                </Text>
                <Text style={{width: '33%'}} importantForAccessibility="no">
                  {android}
                </Text>
                <Text style={{width: '33%'}} importantForAccessibility="no">
                  {harmony}
                </Text>
              </View>
            );
          })}
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="role">
        <TestCase.Example itShould="announce text specified in the appropriate column">
          <View style={{padding: 16, flexDirection: 'row', width: '100%'}}>
            <Text style={{width: '33%', fontWeight: 'bold'}}>Role</Text>
            <Text style={{width: '33%', fontWeight: 'bold'}}>Android</Text>
            <Text style={{width: '33%', fontWeight: 'bold'}}>Harmony</Text>
          </View>
          {Array.from(ANNOUNCEMENT_BY_PLATFORM_BY_ROLE.entries()).map(
            ([role, {android, harmony}]) => {
              return (
                <View
                  key={role}
                  accessible
                  role={role}
                  style={{
                    padding: 16,
                    flexDirection: 'row',
                    width: '100%',
                    borderBottomWidth: 1,
                    borderColor: 'silver',
                  }}>
                  <Text style={{width: '33%'}} importantForAccessibility="no">
                    {role}
                  </Text>
                  <Text style={{width: '33%'}} importantForAccessibility="no">
                    {android}
                  </Text>
                  <Text style={{width: '33%'}} importantForAccessibility="no">
                    {harmony}
                  </Text>
                </View>
              );
            },
          )}
        </TestCase.Example>
      </TestSuite>

      <TestSuite name="aria-labelledby">
        <TestCase.Example
          itShould={`announce: '${Platform.OS === 'android' ? 'edit box for phone number' : 'phone number, edit box, double tap to ...'}' after TextInput gains focus`}>
          <View>
            <Text nativeID="testAriaLabelledBy">Phone Number</Text>
            <TextInput
              style={{backgroundColor: 'lightblue', padding: 8}}
              aria-labelledby={'testAriaLabelledBy'}
              value="focus on me"
            />
          </View>
        </TestCase.Example>

        <TestCase.Example itShould="not mention: 'phone number' after the text input gains focus">
          <View collapsable={false}>
            <View accessibilityLabelledBy={'xxx'}>
              <View collapsable={false} nativeID="testAriaLabelledBy2">
                <Text>
                  Phone Number (aria-labelledby points to a view, not a text)
                </Text>
              </View>
              <TextInput
                aria-labelledby={'testAriaLabelledBy2'}
                style={{backgroundColor: 'lightblue', padding: 8}}
                value="focus on me"
              />
            </View>
          </View>
        </TestCase.Example>
      </TestSuite>

      <TestSuite name="accessibilityLabelledBy">
        <TestCase.Example
          itShould={`announce: '${Platform.OS === 'android' ? 'edit box for phone number in accessibility label' : 'phone number in accessibility label, edit box, double tap to ...'}' after TextInput gains focus`}>
          <View>
            <Text
              accessibilityLabel="phone number in accessibilityLabel"
              nativeID="testAccessibilityLabelledBy">
              Phone Number
            </Text>
            <TextInput
              accessibilityLabelledBy={'testAccessibilityLabelledBy'}
              style={{backgroundColor: 'lightblue', padding: 8}}
              value="focus on me"
            />
          </View>
        </TestCase.Example>
      </TestSuite>
    </TestSuite>
  );
}
