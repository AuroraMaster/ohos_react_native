/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  StyleSheet,
  Text,
  TouchableWithoutFeedback,
  View,
  findNodeHandle,
} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import React, {useRef, useState} from 'react';
import {Button, TestCase} from '../../components';
import {ViewAccessibilityTest} from './ViewAccessibilityTest';

export function ViewTest() {
  return (
    <TestSuite name="View">
      <TestCase.Example
        modal
        itShould="log the descriptor on the native side when 'debug' hint is provided">
        <View
          id="__harmony::debug:sampleNativeId"
          style={{width: 64, height: 64, backgroundColor: 'red'}}
        />
      </TestCase.Example>
      <TestCase.Example modal itShould="apply testId to view">
        <View
          testID="testId"
          style={{width: 64, height: 64, backgroundColor: 'red'}}
        />
      </TestCase.Example>
      <TestCase.Example itShould="render square with transparent background on gray background">
        <View style={{width: '100%', height: 100, backgroundColor: 'gray'}}>
          <View
            style={{
              width: 200,
              height: 100,
              borderWidth: 1,
              borderColor: '#FF00FF',
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render square with rounded corners with different radii (left/right)">
        <View style={{width: '100%', height: 100, backgroundColor: 'gray'}}>
          <View
            style={{
              width: 100,
              height: 100,
              borderWidth: 3,
              borderColor: 'white',
              borderTopLeftRadius: 10,
              borderTopRightRadius: 20,
              borderBottomRightRadius: 30,
              borderBottomLeftRadius: 40,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render square with rounded corners with different radii (start/end)">
        <View style={{width: '100%', height: 100, backgroundColor: 'gray'}}>
          <View
            style={{
              width: 100,
              height: 100,
              borderWidth: 3,
              borderColor: 'white',
              borderTopStartRadius: 10,
              borderTopEndRadius: 20,
              borderBottomEndRadius: 30,
              borderBottomStartRadius: 40,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render squares with borderTopStartRadius and borderTopEndRadius">
        <View style={styles.squaresContainer}>
          <View style={[styles.square, {borderTopStartRadius: 24}]}>
            <Text style={styles.squareContent}>borderTopStartRadius</Text>
          </View>
          <View style={[styles.square, {borderTopEndRadius: 24}]}>
            <Text style={styles.squareContent}>borderTopEndRadius</Text>
          </View>
          <View
            style={[
              styles.square,
              {borderTopEndRadius: 24, borderTopStartRadius: 20},
            ]}>
            <Text style={styles.squareContent}>
              borderTopEndRadius + borderTopStartRadius
            </Text>
          </View>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render squares with borderBottomStartRadius + borderBottomEndRadius">
        <View style={styles.squaresContainer}>
          <View style={[styles.square, {borderBottomStartRadius: 24}]}>
            <Text style={styles.squareContent}>borderBottomStartRadius</Text>
          </View>
          <View style={[styles.square, {borderBottomEndRadius: 24}]}>
            <Text style={styles.squareContent}>borderBottomEndRadius</Text>
          </View>
          <View
            style={[
              styles.square,
              {borderBottomEndRadius: 24, borderBottomStartRadius: 24},
            ]}>
            <Text style={styles.squareContent}>
              borderBottomStartRadius + borderBottomEndRadius
            </Text>
          </View>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render circles">
        <View style={styles.squaresContainer}>
          <View style={[styles.square, {borderRadius: 50}]} />
          <View
            style={[
              styles.square,
              {
                borderBottomStartRadius: 50,
                borderBottomEndRadius: 50,
                borderTopStartRadius: 50,
                borderTopEndRadius: 50,
              },
            ]}
          />
          <View
            style={[
              styles.square,
              {
                borderBottomLeftRadius: 50,
                borderBottomRightRadius: 50,
                borderTopLeftRadius: 50,
                borderTopRightRadius: 50,
              },
            ]}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render square with borders with different widths">
        <View style={{width: '100%', height: 100, backgroundColor: 'gray'}}>
          <View
            style={{
              width: 100,
              height: 100,
              borderColor: 'white',
              borderLeftWidth: 2,
              borderTopWidth: 4,
              borderRightWidth: 6,
              borderBottomWidth: 8,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render rectangle with borders with different widths and colors">
        <View style={{width: '100%', height: 100, backgroundColor: 'gray'}}>
          <View
            style={{
              height: 80,
              width: 200,
              borderLeftWidth: 1,
              borderTopWidth: 10,
              borderRightWidth: 5,
              borderBottomWidth: 20,
              borderLeftColor: '#ff0000',
              borderRightColor: '#ffff00',
              borderTopColor: 'pink',
              borderBottomColor: 'skyblue',
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render square with borders with different colors">
        <View style={{width: '100%', height: 100, backgroundColor: 'gray'}}>
          <View
            style={{
              width: 100,
              height: 100,
              borderWidth: 3,
              borderLeftColor: 'blue',
              borderTopColor: 'red',
              borderRightColor: 'green',
              borderBottomColor: 'yellow',
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render square with borders with different start/end colors">
        <View style={{width: '100%', height: 100, backgroundColor: 'gray'}}>
          <View
            style={{
              width: 100,
              height: 100,
              borderWidth: 3,
              borderStartColor: 'blue',
              borderEndColor: 'green',
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render squares with borders with different style">
        <View
          style={{
            width: '100%',
            height: 100,
            backgroundColor: 'gray',
            flexDirection: 'row',
          }}>
          <View
            style={{
              width: 100,
              height: 100,
              marginEnd: 4,
              borderWidth: 3,
              borderColor: 'white',
              borderStyle: 'solid',
            }}
          />
          <View
            style={{
              width: 100,
              height: 100,
              marginEnd: 4,
              borderWidth: 3,
              borderColor: 'white',
              borderStyle: 'dotted',
            }}
          />
          <View
            style={{
              width: 100,
              height: 100,
              borderWidth: 3,
              borderColor: 'white',
              borderStyle: 'dashed',
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="hide the overflow">
        <View
          style={{
            width: 64,
            height: 64,
            backgroundColor: 'red',
            overflow: 'hidden',
          }}
          collapsable={false}>
          <View
            style={{
              width: 64,
              height: 64,
              backgroundColor: 'green',
              marginLeft: 32,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="not hide the overflow">
        <View
          style={{
            width: 64,
            height: 64,
            backgroundColor: 'red',
          }}
          collapsable={false}>
          <View
            style={{
              width: 64,
              height: 64,
              backgroundColor: 'green',
              marginLeft: 32,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="not show view with height 0 and overflow hidden">
        <View
          style={{
            width: 64,
            height: 0,
            backgroundColor: 'red',
            overflow: 'hidden',
            margin: 10,
          }}>
          <View
            style={{
              width: 64,
              height: 64,
              backgroundColor: 'green',
              marginLeft: 32,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render blue rectangle (zIndex test)">
        <View>
          <View
            style={{
              width: 64,
              height: 64,
              backgroundColor: 'blue',
              zIndex: 2,
              position: 'relative', // https://github.com/facebook/react-native/issues/38513
            }}
          />
          <View
            style={{
              width: 64,
              height: 64,
              backgroundColor: 'red',
              zIndex: 1,
              position: 'absolute',
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example
        itShould="render square with elevation"
        skip={{android: false, harmony: {arkTS: true, cAPI: true}}}
        //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/238
      >
        <View style={{width: '100%', height: 100}}>
          <View
            style={{
              width: 80,
              height: 80,
              margin: 10,
              backgroundColor: 'blue',
              elevation: 10,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example
        skip={{
          android: false,
          harmony: {
            arkTS: true,
            cAPI: true,
          },
        }}
        itShould="show inner rectangle with the same color as the reference (needsOffscreenAlphaCompositing)"
        //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/322
      >
        <View
          style={{
            opacity: 0.5,
            backgroundColor: 'red',
            height: 100,
            width: 100,
          }}
          needsOffscreenAlphaCompositing>
          <View
            style={{
              backgroundColor: 'black',
              width: 60,
              height: 40,
              opacity: 0.5,
            }}
          />
        </View>
        <Text style={{height: 20}}>
          Reference black color with opacity: 0.5
        </Text>
        <View
          style={{
            width: 60,
            height: 40,
            backgroundColor: 'black',
            opacity: 0.5,
          }}
        />
      </TestCase.Example>
      <TestCase.Example itShould="show view with X moved by 500px right, Y scaled 2 times, rotated by 5deg">
        <View style={{width: '100%', height: 20}}>
          <View
            style={{
              width: 100,
              backgroundColor: 'blue',
              transform: [{translateX: 500}, {rotate: '5deg'}, {scaleY: 2}],
            }}>
            <Text style={{height: 20}}>Text</Text>
          </View>
        </View>
      </TestCase.Example>
      <TestSuite name="pointerEvents">
        <TestCase.Manual
          itShould="call inner and outer view when pressing inner"
          initialState={{inner: false, outer: false, outerContainer: false}}
          arrange={({setState, reset}) => {
            return (
              <PointerEventsView
                pointerEventsOuter="auto"
                setState={setState}
                reset={reset}
              />
            );
          }}
          assert={({expect, state}) => {
            expect(state).to.be.deep.eq({
              inner: true,
              outer: true,
              outerContainer: true,
            });
          }}
        />
        <TestCase.Manual
          itShould="call only outer when pressing inner view"
          initialState={{inner: false, outer: false, outerContainer: true}}
          arrange={({setState, reset}) => {
            return (
              <PointerEventsView
                pointerEventsOuter="box-only"
                setState={setState}
                reset={reset}
              />
            );
          }}
          assert={({expect, state}) => {
            expect(state).to.be.deep.eq({
              inner: false,
              outer: true,
              outerContainer: true,
            });
          }}
        />
        <TestCase.Manual
          itShould="call inner and outer only when pressing inner view"
          initialState={{inner: false, outer: false, outerContainer: false}}
          arrange={({setState, reset}) => {
            return (
              <PointerEventsView
                disableOuterContainerTouch
                pointerEventsOuter="box-none"
                setState={setState}
                reset={reset}
              />
            );
          }}
          assert={({expect, state}) => {
            expect(state.inner).to.be.true;
            expect(state.outer).to.be.true;
          }}
        />
        <TestCase.Manual
          itShould="not call inner or outer when pressing inner or outer views"
          initialState={{inner: false, outer: false, outerContainer: false}}
          arrange={({setState, reset}) => {
            return (
              <PointerEventsView
                pointerEventsOuter="none"
                setState={setState}
                reset={reset}
              />
            );
          }}
          assert={({expect, state}) => {
            expect(state).to.be.deep.eq({
              inner: false,
              outer: false,
              outerContainer: true,
            });
          }}
        />
      </TestSuite>
      <TestCase.Manual
        itShould="pass on touching blue background"
        initialState={false}
        arrange={({setState}) => (
          <View style={{backgroundColor: 'blue', alignSelf: 'center'}}>
            <View
              hitSlop={{top: 48, left: 48, bottom: 48, right: 48}}
              style={{
                width: 48,
                height: 48,
                backgroundColor: 'green',
                margin: 48,
              }}
              onTouchEnd={() => {
                setState(true);
              }}>
              <View
                style={{width: 48, height: 48, backgroundColor: 'red'}}
                onTouchEnd={e => {
                  e.stopPropagation();
                }}
              />
            </View>
          </View>
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="pass on touching transparent view"
        initialState={false}
        arrange={({setState, reset}) => (
          <>
            <View
              style={{
                backgroundColor: 'transparent',
                width: '100%',
                height: 100,
              }}
              onTouchEnd={() => setState(true)}
            />
            <Button label="reset" onPress={reset} />
          </>
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="blue view should not allow clicks with non-touch input device"
        modal
        initialState={{first: false, second: false, third: false}}
        arrange={({setState}) => (
          <View style={{width: '100%', height: 100, flexDirection: 'row'}}>
            <TouchableWithoutFeedback
              onPress={() => setState(prev => ({...prev, first: true}))}>
              <View
                style={{
                  width: 76,
                  height: 76,
                  marginRight: 4,
                  backgroundColor: 'red',
                }}
              />
            </TouchableWithoutFeedback>
            <TouchableWithoutFeedback
              onPress={() => setState(prev => ({...prev, second: true}))}>
              <View
                style={{
                  width: 76,
                  height: 76,
                  marginRight: 4,
                  backgroundColor: 'blue',
                }}
                // @ts-ignore
                tabIndex={-1}
              />
            </TouchableWithoutFeedback>
            <TouchableWithoutFeedback
              onPress={() => setState(prev => ({...prev, third: true}))}>
              <View
                style={{
                  width: 76,
                  height: 76,
                  marginRight: 4,
                  backgroundColor: 'red',
                }}
              />
            </TouchableWithoutFeedback>
          </View>
        )}
        assert={({state, expect}) => {
          expect(state).to.be.deep.eq({
            first: true,
            second: false,
            third: true,
          });
        }}
      />
      <TestCase.Example itShould="render view with fixed width and aspectRatio 1">
        <View style={{width: '100%', height: 100}}>
          <View
            style={{
              width: 100,
              backgroundColor: 'blue',
              aspectRatio: 1,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render views with set flex and aspectRatio 1">
        <View style={{width: '100%', height: 100}}>
          <View
            style={{
              width: 100,
              backgroundColor: 'blue',
              flex: 1,
              aspectRatio: 1,
            }}
          />
          <View
            style={{
              width: 100,
              backgroundColor: 'blue',
              flex: 2,
              aspectRatio: 1,
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="show view rotated by 180deg(backfaceVisibility: visible)">
        <View style={{width: '100%', height: 20}}>
          <View
            style={{
              width: 100,
              backgroundColor: 'blue',
              transform: [{rotateY: '180deg'}],
              backfaceVisibility: 'visible',
            }}>
            <Text style={{height: 20}}>Backface</Text>
          </View>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="not show view rotated by 180deg(backfaceVisibility: hidden)">
        <View style={{width: '100%', height: 20}}>
          <View
            style={{
              width: 100,
              backgroundColor: 'blue',
              transform: [{rotateY: '180deg'}],
              backfaceVisibility: 'hidden',
            }}>
            <Text style={{height: 20}}>Backface</Text>
          </View>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="render light blue shadow shifted towards bottom and right">
        <View
          style={{
            width: 64,
            height: 64,
            margin: 8,
            backgroundColor: 'green',
            shadowColor: 'blue',
            shadowOffset: {width: 16, height: 16},
            shadowOpacity: 0.25,
            shadowRadius: 16,
          }}
        />
      </TestCase.Example>
      <TestCase.Example itShould="View Overflow type hidden test by exceeded text">
        <View
          style={{
            backgroundColor: 'rgba(0,255,0,1)',
            width: 150,
            height: 100,
            overflow: 'hidden',
          }}
          collapsable={false}>
          <Text>
            Michaelmas term lately over, and the Lord Chancellor sitting in
            Lincoln's Inn Hall. Implacable November weather. As much mud in the
            streets as if the waters had but newly retired from the face of the
            earth.
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="View Overflow type visible test by exceeded text">
        <View
          style={{
            backgroundColor: 'rgba(0,255,0,1)',
            width: 150,
            height: 100,
            overflow: 'visible',
          }}
          collapsable={false}>
          <Text>
            Michaelmas term lately over, and the Lord Chancellor sitting in
            Lincoln's Inn Hall. Implacable November weather. As much mud in the
            streets as if the waters had but newly retired from the face of the
            earth.
          </Text>
        </View>
      </TestCase.Example>
      <TestCase.Example
        itShould="render a view with role"
        skip={{android: false, harmony: {arkTS: true, cAPI: true}}} // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/603
      >
        <View role="alert">
          <Text>Alert</Text>
        </View>
      </TestCase.Example>
      <TestCase.Manual
        itShould="pass on blue rect touch (onResponderReject)"
        initialState={{
          responderRejectedCount: 0,
          responderGrantedCount: 0,
          childResponderGrantedCount: 0,
        }}
        arrange={({setState}) => {
          return (
            <View
              onMoveShouldSetResponder={() => true}
              onResponderReject={() => {
                setState(prev => ({
                  ...prev,
                  responderRejectedCount: prev.responderRejectedCount + 1,
                }));
              }}
              onResponderGrant={() => {
                setState(prev => ({
                  ...prev,
                  responderGrantedCount: prev.responderGrantedCount + 1,
                }));
              }}
              style={{
                backgroundColor: 'green',
                padding: 20,
              }}>
              <View
                style={{backgroundColor: 'blue', width: 64, height: 64}}
                onResponderTerminationRequest={() => false}
                onStartShouldSetResponder={() => true}
                onResponderGrant={() => {
                  setState(prev => ({
                    ...prev,
                    childResponderGrantedCount:
                      prev.childResponderGrantedCount + 1,
                  }));
                }}
              />
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state.responderRejectedCount).to.be.greaterThan(0);
        }}
      />
      <ViewAccessibilityTest />
      <TestCase.Manual
        modal
        itShould='call the "escape" gesture handler'
        initialState={false}
        skip={{android: false, harmony: {arkTS: true, cAPI: true}}} // https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/602
        arrange={({setState}) => (
          <View
            accessible={true}
            style={{width: '100%', height: 100, backgroundColor: 'gray'}}
            onAccessibilityEscape={() => {
              console.log('onAccessibilityEscape called!');
              setState(true);
            }}>
            <View
              style={{
                width: 100,
                height: 100,
                backgroundColor: 'red',
              }}
            />
          </View>
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example
        modal
        itShould='render "First Layout" view and ignore "Ignored Layout" when accessibility is true'>
        <View accessible={true} style={styles.accessibilityContainer}>
          <View
            style={[styles.accessibilityLayout, {backgroundColor: 'green'}]}
            importantForAccessibility="yes">
            <Text>First layout</Text>
          </View>
          <View
            style={[styles.accessibilityLayout, {backgroundColor: 'yellow'}]}
            importantForAccessibility="no-hide-descendants">
            <Text>Ignored Layout</Text>
          </View>
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="make the screen reader say/display 'busy' after clicking on the background">
        <View
          accessible={true}
          aria-busy={true}
          style={styles.accessibilityLayout}
        />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="make the screen reader say/display: 'checked, mixed' when both button are 'checked', 'mixed' when one of the button is 'checked' and 'unchecked' when none of the button is 'checked'">
        <ViewAccessibilityAriaChecked />
      </TestCase.Example>
      <TestCase.Example
        modal
        skip="only works on iOS"
        itShould="make the screen reader hide 'Hidden layout' accessibilityHidden">
        <View accessible={true} style={styles.accessibilityContainer}>
          <View
            style={[styles.accessibilityLayout, {backgroundColor: 'green'}]}>
            <Text>First layout</Text>
          </View>
          <View
            style={[styles.accessibilityLayout, {backgroundColor: 'yellow'}]}
            accessibilityElementsHidden={true}>
            <Text>Hidden Layout</Text>
          </View>
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="make the screen reader say/display 'This view has a red background' and 'Hint: and no text'">
        <View
          accessible={true}
          aria-label="This view has a red background"
          accessibilityHint="Hint: and no text"
          style={[styles.accessibilityLayout, {backgroundColor: 'red'}]}
        />
      </TestCase.Example>
      <TestCase.Example
        skip={{android: false, harmony: {arkTS: true, cAPI: true}}}
        modal
        itShould="allow user to move with keyboard's arrows keys between blue squares">
        <ViewNextFocus />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="change backgroundColor to red after pressing a button (setNativeProps)">
        <SetNativePropsTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="Change the height of the View by using setNativeProps({height: h}), similarly for width, top, right, bottom, and left">
        <SetNativePropsOfHeight />
      </TestCase.Example>
    </TestSuite>
  );
}

const SetNativePropsTest = () => {
  const ref = useRef<View>(null);
  return (
    <View>
      <View
        style={{backgroundColor: 'yellow', width: 50, height: 50}}
        ref={ref}
      />
      <Button
        label="setBackgroudColor"
        onPress={() =>
          ref.current?.setNativeProps({style: {backgroundColor: 'red'}})
        }
      />
    </View>
  );
};

function PointerEventsView(props: {
  disableOuterContainerTouch?: boolean;
  pointerEventsOuter?: 'box-none' | 'none' | 'box-only' | 'auto';
  pointerEventsInner?: 'box-none' | 'none' | 'box-only' | 'auto';
  setState: React.Dispatch<
    React.SetStateAction<{
      inner: boolean;
      outer: boolean;
      outerContainer: boolean;
    }>
  >;
  reset: () => void;
}) {
  return (
    <View style={{height: 100, width: '100%', flexDirection: 'row'}}>
      <View
        style={{backgroundColor: 'green'}}
        onTouchEnd={
          props.disableOuterContainerTouch
            ? undefined
            : () => {
                props.setState(prev => ({...prev, outerContainer: true}));
              }
        }>
        <View
          style={{height: 100, width: 100, backgroundColor: 'red'}}
          pointerEvents={props.pointerEventsOuter}
          onTouchEnd={() => {
            props.setState(prev => ({...prev, outer: true}));
          }}>
          <View
            style={{
              height: 40,
              width: 40,
              backgroundColor: 'blue',
              margin: 30,
            }}
            onTouchEnd={() => {
              props.setState(prev => ({...prev, inner: true}));
            }}
            pointerEvents={props.pointerEventsInner}
          />
        </View>
      </View>
      <Button label="reset" onPress={props.reset} />
    </View>
  );
}

function ViewAccessibilityAriaChecked() {
  const [firstChecked, setFirstChecked] = useState<boolean>(false);
  const [secondChecked, setSecondChecked] = useState<boolean>(false);

  const checked = firstChecked && secondChecked;
  const mixed = firstChecked !== secondChecked;

  return (
    <View
      accessible={true}
      aria-checked={checked ? true : mixed ? 'mixed' : false}
      style={[styles.accessibilityLayout, {height: 200}]}
      accessibilityRole="checkbox"
      accessibilityState={{checked}}>
      <Button
        label={`First Element is ${firstChecked ? 'checked' : 'unchecked'}`}
        onPress={() => setFirstChecked(!firstChecked)}
      />
      <Button
        label={`Second Element is ${secondChecked ? 'checked' : 'unchecked'}`}
        onPress={() => setSecondChecked(!secondChecked)}
      />
    </View>
  );
}

const BasicView = React.forwardRef<View, any>(({text, ...props}, ref) => (
  <View ref={ref} style={styles.gridContainerSquare} focusable {...props}>
    <Text style={styles.squareContent}>{text}</Text>
  </View>
));

function ViewNextFocus() {
  const [pressCounter, setPressCounter] = useState(0);

  const topLeftViewRef = React.useRef<View>(null);
  const topRightViewRef = React.useRef<View>(null);
  const midLeftViewRef = React.useRef<View>(null);
  const midRightViewRef = React.useRef<View>(null);
  const botLeftViewRef = React.useRef<View>(null);
  const botRightViewRef = React.useRef<View>(null);

  // Implementation details:
  // normally, in order to detect change in ref we would use callback ref
  // https://legacy.reactjs.org/docs/refs-and-the-dom.html#callback-refs
  // to make it very simply, we use button to force rerender of the component
  // to get the "updated" ref
  const topLeftViewNode = findNodeHandle(topLeftViewRef.current);
  const topRightViewNode = findNodeHandle(topRightViewRef.current);
  const midLeftViewNode = findNodeHandle(midLeftViewRef.current);
  const midRightViewNode = findNodeHandle(midRightViewRef.current);
  const botLeftViewNode = findNodeHandle(botLeftViewRef.current);
  const botRightViewNode = findNodeHandle(botRightViewRef.current);

  return (
    <View style={{height: '90%'}}>
      <Button
        label={`Rerender (${pressCounter}) Component - force ref to be updated`}
        onPress={() => setPressCounter(count => count + 1)}
      />
      <View style={styles.gridContainer}>
        <BasicView
          ref={topLeftViewRef}
          text="Top Left"
          nextFocusUp={botLeftViewNode ?? undefined}
          nextFocusLeft={botRightViewNode ?? undefined}
        />
        <BasicView
          ref={topRightViewRef}
          text="Top Right"
          nextFocusUp={botRightViewNode ?? undefined}
          nextFocusRight={midLeftViewNode ?? undefined}
        />
        <BasicView
          ref={midLeftViewRef}
          text="Mid Left"
          nextFocusLeft={topRightViewNode ?? undefined}
        />
        <BasicView
          ref={midRightViewRef}
          text="Mid Right"
          nextFocusRight={botLeftViewNode ?? undefined}
        />
        <BasicView
          ref={botLeftViewRef}
          text="Bot Left"
          nextFocusDown={topLeftViewNode ?? undefined}
          nextFocusLeft={midRightViewNode ?? undefined}
        />
        <BasicView
          ref={botRightViewRef}
          text="Bot Right"
          nextFocusDown={topRightViewNode ?? undefined}
          nextFocusRight={topLeftViewNode ?? undefined}
        />
      </View>
    </View>
  );
}

function SetNativePropsOfHeight() {
  const viewRef = useRef<View>(null);
  const onPress = () => {
    viewRef?.current?.setNativeProps({height: Math.random() * 256});
  };
  return (
    <TouchableWithoutFeedback onPress={onPress}>
      <View ref={viewRef} style={{backgroundColor: 'deepskyblue'}}>
        <Text>Press Me to change the height of View</Text>
      </View>
    </TouchableWithoutFeedback>
  );
}

const styles = StyleSheet.create({
  squaresContainer: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    backgroundColor: 'gray',
  },
  square: {
    width: 100,
    height: 100,
    backgroundColor: 'lightblue',
    margin: 4,
  },
  squareContent: {
    textAlignVertical: 'center',
    textAlign: 'center',
    height: '100%',
  },
  accessibilityContainer: {
    width: '100%',
    backgroundColor: 'gray',
  },
  accessibilityLayout: {
    width: '100%',
    height: 100,
    backgroundColor: 'lightblue',
  },
  gridContainer: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'center',
    alignItems: 'center',
    gap: 10,
  },
  gridContainerSquare: {
    width: 150,
    height: 150,
    backgroundColor: 'lightblue',
    margin: 4,
  },
});
