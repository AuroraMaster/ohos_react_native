/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {useEffect, useRef, useState} from 'react';
import {
  AccessibilityChangeEventName,
  AccessibilityEventTypes,
  AccessibilityInfo,
  findNodeHandle,
  Text,
  TextProps,
  View,
} from 'react-native';
import {Button, Ref, TestCase} from '../components';
import {PALETTE} from '../components/palette';

export function AccessibilityInfoTest() {
  const [skipMsgIfReaderDisabled, setSkipMsgIfReaderDisabled] = useState<
    undefined | string
  >('Determining screen reader status...');

  useEffect(() => {
    const onScreenReaderChange = (isScreenReaderEnabled: boolean) => {
      if (isScreenReaderEnabled) {
        setSkipMsgIfReaderDisabled(undefined);
      } else {
        setSkipMsgIfReaderDisabled(
          'ScreenReader must be enabled to run this test',
        );
      }
    };

    AccessibilityInfo.isScreenReaderEnabled().then(onScreenReaderChange);
    const listener = AccessibilityInfo.addEventListener(
      'screenReaderChanged',
      onScreenReaderChange,
    );

    return () => {
      listener.remove();
    };
  }, []);

  return (
    <TestSuite name="AccessibilityInfo">
      <Text
        style={{
          color: 'black',
          padding: 8,
          borderRadius: 8,
          fontSize: 12,
          backgroundColor: PALETTE.YELLOW_LIGHT,
        }}>
        {
          'To enable screen reader on: HarmonyOS 1) go to Settings > Accessibility features > Accessibility shortcut 2) Assign ScreenReader in "Select feature" option 3) Hold volume up and down for 3 sec.'
        }
      </Text>
      <TestSuite name="addEventListener">
        <TestSuite name="accessibilityServiceChanged">
          <TestCase.Example itShould="display red background if Accessibility Service is changed">
            <AccessibilityInfoAccessibilityServiceChanged />
          </TestCase.Example>
        </TestSuite>
        <TestSuite name="screenReaderChanged">
          <TestCase.Manual
            initialState={false}
            itShould="pass when screen reader is enabled (enable screen reader)"
            arrange={({setState}) => {
              return (
                <AccessibilityInfoScreenReaderStatus
                  onScreenReaderChanged={isEnabled => {
                    if (isEnabled) {
                      setState(true);
                    }
                  }}
                />
              );
            }}
            assert={({expect, state}) => {
              expect(state).to.be.eq(true);
            }}
          />
        </TestSuite>
      </TestSuite>
      <TestSuite name="isScreenReaderEnabled">
        <AccessibilityFeatureStatusTestCase
          itShould="return true when the screen reader is enabled and false when disabled (test passes when call history includes both states)"
          onChangeAccessibilityFeature={() =>
            AccessibilityInfo.isScreenReaderEnabled()
          }
        />
      </TestSuite>

      <TestSuite name="announceForAccessibility">
        <TestCase.Example
          skip={skipMsgIfReaderDisabled}
          itShould="announce 'Button pressed' after pressing the button">
          <Button
            label="Read text"
            onPress={() => {
              AccessibilityInfo.announceForAccessibility('Button pressed');
            }}
          />
        </TestCase.Example>
      </TestSuite>

      <TestSuite name="isAccessibilityServiceEnabled">
        <AccessibilityFeatureStatusTestCase
          itShould="return true when the screen reader (or some other third party accessibility service) is enabled and false when disabled (test passes when call history includes both states)"
          onChangeAccessibilityFeature={() =>
            AccessibilityInfo.isAccessibilityServiceEnabled()
          }
        />
      </TestSuite>

      <TestSuite name="sendAccessibilityEvent">
        <TestSuite name="click">
          <TestCase.Example
            itShould="announce 'checked' after pressing the button"
            skip={
              skipMsgIfReaderDisabled ?? {
                android: false,
                harmony:
                  'Connected but this test fails probably because aria-checked is not connected.',
              }
            }>
            <SendAccessibilityEventExample
              accessibilityEvent="click"
              textProps={{'aria-checked': true}}
            />
          </TestCase.Example>
          <TestCase.Example
            itShould="announce 'selected' after pressing the button"
            skip={
              skipMsgIfReaderDisabled ?? {
                android: false,
                harmony:
                  'Connected but this test fails probably because aria-selected is not connected.',
              }
            }>
            <SendAccessibilityEventExample
              accessibilityEvent="click"
              textProps={{'aria-selected': true}}
            />
          </TestCase.Example>
        </TestSuite>
        <TestSuite name="focus">
          <TestCase.Example
            itShould="read the text content in the square"
            skip={skipMsgIfReaderDisabled}>
            <SendAccessibilityEventExample accessibilityEvent="focus" />
          </TestCase.Example>
        </TestSuite>
        <TestSuite name="viewHoverEnter">
          <TestCase.Example
            itShould="read the text content in the square when hovering over an element"
            skip={
              skipMsgIfReaderDisabled ?? {
                android: false,
                harmony:
                  "Hovering in general behaves differently to Android and the text isn't read.",
              }
            }>
            <SendAccessibilityEventExample accessibilityEvent="viewHoverEnter" />
          </TestCase.Example>
        </TestSuite>
      </TestSuite>
      <TestSuite name="setAccessibilityFocus">
        <TestCase.Example
          itShould="read 'target view' after pressing the button and switch focus to that view"
          skip={skipMsgIfReaderDisabled}>
          <Ref<Text>
            render={ref => {
              return (
                <>
                  <Text ref={ref}>target view</Text>
                  <Button
                    label="setAccessibilityFocus on target view"
                    onPress={() => {
                      AccessibilityInfo.setAccessibilityFocus(
                        findNodeHandle(ref.current)!,
                      );
                    }}
                  />
                </>
              );
            }}
          />
        </TestCase.Example>
      </TestSuite>
      <TestSuite name="isBoldTextEnabled">
        <TestCase.Manual<boolean[]>
          itShould="pass when history contains both states"
          initialState={[]}
          skip={{android: true, harmony: false}}
          arrange={({setState, state}) => {
            return (
              <>
                <Text style={{fontSize: 12}}>
                  {
                    "To change font weight on HarmonyOS, go to 'Settings' > 'Text & display size' and drag the Text weight slider until the OS classifies the text weight as bold"
                  }
                </Text>
                <Text style={{marginTop: 16}}>
                  Call result history: {JSON.stringify(state)}
                </Text>
                <Button
                  label="check text weight"
                  onPress={() => {
                    AccessibilityInfo.isBoldTextEnabled().then(
                      isBoldTextEnabled => {
                        setState(prev => [...prev, isBoldTextEnabled]);
                      },
                    );
                  }}
                />
              </>
            );
          }}
          assert={async ({expect, state}) => {
            await new Promise(resolve => {
              if (state.includes(true) && state.includes(false)) {
                resolve(undefined);
              }
            });
            expect(state.includes(true)).to.be.true;
            expect(state.includes(false)).to.be.true;
          }}
        />
      </TestSuite>
    </TestSuite>
  );
}

function SendAccessibilityEventExample({
  accessibilityEvent,
  textProps,
}: {
  accessibilityEvent: AccessibilityEventTypes;
  textProps?: TextProps;
}) {
  const ref = useRef<Text>(null);

  return (
    <>
      <Text
        ref={ref}
        accessible={true}
        style={{
          backgroundColor: 'silver',
          width: '100%',
          height: 64,
        }}
        {...(textProps ?? {})}>
        target content
      </Text>
      <Button
        label="SEND ACCESSIBILITY EVENT"
        onPress={() => {
          AccessibilityInfo.sendAccessibilityEvent(
            ref.current!,
            accessibilityEvent,
          );
        }}
      />
    </>
  );
}

function AccessibilityInfoScreenReaderStatus({
  onScreenReaderChanged,
}: {
  onScreenReaderChanged: (isEnabled: boolean) => void;
}) {
  const [isScreenReaderEnabled, setIsScreenReaderEnabled] = useState(false);
  const backgroundColor = isScreenReaderEnabled ? 'green' : 'transparent';

  useEffect(() => {
    const listener = AccessibilityInfo.addEventListener(
      'screenReaderChanged',
      isEnabled => {
        setIsScreenReaderEnabled(isEnabled);
        onScreenReaderChanged(isEnabled);
      },
    );

    AccessibilityInfo.isScreenReaderEnabled().then(isEnabled => {
      setIsScreenReaderEnabled(isEnabled);
      onScreenReaderChanged(isEnabled);
    });
    return () => {
      listener.remove();
    };
  }, []);

  return (
    <View style={{backgroundColor, padding: 16}}>
      <Text>{`Current Screen Reader status: ${isScreenReaderEnabled ? 'enabled' : 'disabled'}`}</Text>
    </View>
  );
}

function AccessibilityInfoAccessibilityServiceChanged() {
  const [isEnabled, setIsEnabled] = useState(false);
  const backgroundColor = isEnabled ? 'red' : 'transparent';

  useEffect(() => {
    const listener = AccessibilityInfo.addEventListener(
      'accessibilityServiceChanged' as AccessibilityChangeEventName,
      (isOn: boolean) => {
        setIsEnabled(isOn);
      },
    );

    return function cleanup() {
      listener.remove();
    };
  }, []);

  return (
    <View style={{backgroundColor, padding: 16}}>
      <Text>{`Accessibility Service change is ${isEnabled ? 'enabled' : 'disabled'}.`}</Text>
    </View>
  );
}

function AccessibilityFeatureStatusTestCase({
  itShould,
  onChangeAccessibilityFeature,
  skip,
}: {
  itShould: string;
  onChangeAccessibilityFeature: () => Promise<boolean>;
  skip?: React.ComponentProps<typeof TestCase.Manual>['skip'];
}) {
  return (
    <TestCase.Manual<boolean[]>
      itShould={itShould}
      initialState={[]}
      skip={skip}
      arrange={({setState, state}) => {
        return (
          <>
            <Text>Call history: {JSON.stringify(state)}</Text>
            <Button
              label="trigger function"
              onPress={() => {
                onChangeAccessibilityFeature().then(isScreenReaderEnabled => {
                  setState(prev => [...prev, isScreenReaderEnabled]);
                });
              }}
            />
          </>
        );
      }}
      assert={async ({expect, state}) => {
        await new Promise(resolve => {
          if (state.includes(true) && state.includes(false)) {
            resolve(undefined);
          }
        });
        expect(state.includes(true)).to.be.true;
        expect(state.includes(false)).to.be.true;
      }}
    />
  );
}
