/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {
  SampleComponent,
  SampleComponentRef,
  GeneratedSampleComponentCAPI,
  GeneratedSampleComponentCAPIRef,
  GeneratedSampleComponentArkTSRef,
  GeneratedSampleComponentArkTS,
  CodegenLibSampleComponent,
  ContainerView,
} from 'react-native-sample-package';
import {useEffect, useState} from 'react';
import React from 'react';
import {Button, Effect, Ref, TestCase} from '../components';
import {IncomingData as GeneratedSampleNativeComponentArkTSCustomProps} from 'react-native-harmony-sample-package/src/specs/v1/GeneratedSampleNativeComponent';
import {
  IncomingData as GeneratedSampleNativeComponentCAPICustomProps,
  SupportedCommandArgs as GeneratedSampleNativeComponentCAPICommandArgs,
} from 'react-native-harmony-sample-package/src/specs/v2/GeneratedSampleNativeComponent';
import {Text, View, UIManager} from 'react-native';

export function CustomNativeComponentTest() {
  return (
    <TestSuite name="Custom Native Component">
      <ManualCustomComponentImplementationTest />
      <GeneratedCustomComponentTest />
      <ContainerViewTest />
    </TestSuite>
  );
}

function ManualCustomComponentImplementationTest() {
  const [refreshKey, setRefreshKey] = useState(0);

  useEffect(() => {
    const intervalId = setInterval(() => {
      setRefreshKey(prev => prev + 1);
    }, 1000);
    return () => {
      clearInterval(intervalId);
    };
  }, []);

  return (
    <TestSuite name="no codegen">
      <TestCase.Example itShould="render red rectangle">
        <SampleComponent backgroundColor="red" size={64} />
      </TestCase.Example>
      <TestCase.Example itShould="render green rectangle inside red rectangle">
        <SampleComponent backgroundColor="red" size={64}>
          <SampleComponent backgroundColor="green" size={32} />
        </SampleComponent>
      </TestCase.Example>
      <TestCase.Example itShould="render red rectangle with black text">
        <SampleComponent backgroundColor="red" size={64} textColor="black" />
      </TestCase.Example>
      <TestCase.Manual
        itShould="handle custom native event when clicked"
        initialState={false}
        arrange={({setState}) => (
          <SampleComponent
            backgroundColor="red"
            size={64}
            onSampleClick={() => setState(true)}
          />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example itShould="change bgColor every second">
        <SampleComponent
          backgroundColor={refreshKey % 2 === 0 ? 'red' : 'green'}
          size={64}
        />
      </TestCase.Example>
      <TestCase.Example itShould="show and hide red rectangle">
        <View style={{height: 64}}>
          {refreshKey % 2 === 0 && (
            <SampleComponent
              backgroundColor={refreshKey % 2 === 0 ? 'red' : 'green'}
              size={64}
            />
          )}
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="show/hide blue rectangle">
        <SampleComponent backgroundColor="red" size={64}>
          <Blinker>
            <SampleComponent backgroundColor="blue" size={32} />
          </Blinker>
        </SampleComponent>
      </TestCase.Example>
      <TestCase.Example itShould="toggle font size in the component below button (native commands)">
        <Ref<SampleComponentRef>
          render={ref => {
            return (
              <>
                <Button
                  label="Toggle Font Size"
                  onPress={() => {
                    ref.current?.toggleFontSize();
                  }}
                />
                <SampleComponent ref={ref} backgroundColor="blue" size={32} />
              </>
            );
          }}
        />
      </TestCase.Example>
    </TestSuite>
  );
}

function Blinker({children}: any) {
  const [isVisible, setIsVisible] = useState(true);

  return (
    <React.Fragment>
      <Button
        label="Run"
        onPress={() => {
          setIsVisible(prev => !prev);
        }}
      />
      {isVisible && children}
    </React.Fragment>
  );
}

function VerifyNativeCommandExists() {
  return (
    <TestSuite name="verify native commands">
      <TestCase.Logical
        itShould="verify that native command emitNativeEvent exists"
        fn={({expect}) => {
          const command = UIManager.getViewManagerConfig('GeneratedSampleView')
            ?.Commands?.emitNativeEvent;
          expect(command).to.be.eq('emitNativeEvent');
        }}
      />
    </TestSuite>
  );
}

function GeneratedCustomComponentTest() {
  return (
    <TestSuite name="generated custom component">
      <VerifyNativeCommandExists />
      <TestSuite name="ArkTS">
        <TestCase.Manual<
          | GeneratedSampleNativeComponentArkTSCustomProps
          | undefined
          | 'IN_PROGRESS'
        >
          itShould="ensure equality between provided and received data"
          initialState={undefined}
          arrange={({setState}) => {
            return (
              <Ref<GeneratedSampleComponentArkTSRef>
                render={ref => (
                  <GeneratedSampleComponentArkTS
                    ref={ref}
                    testProps={{
                      booleanTest: true,
                      intTest: 42,
                      floatTest: 42.5,
                      doubleTest: 42.5,
                      stringTest: 'foobar',
                      objectTest: {foo: {bar: 'baz'}},
                      colorTest: 'red',
                      arrayTest: ['foo', 'bar'],
                      readOnlyArrayTest: ['foo', 'bar'],
                      intEnumTest: 1,
                    }}
                    onDirectEvent={setState}>
                    <Effect
                      onMount={() => {
                        setState('IN_PROGRESS');
                        setTimeout(() =>
                          ref.current?.emitNativeEvent('directEvent'),
                        );
                      }}
                    />
                  </GeneratedSampleComponentArkTS>
                )}
              />
            );
          }}
          assert={async ({expect, state}) => {
            if (state === 'IN_PROGRESS') {
              await new Promise(() => {});
              return;
            }
            expect(state?.booleanTest).to.be.true;
            expect(state?.booleanWithDefaultTest).to.be.true;
            expect(state?.intTest).to.be.eq(42);
            expect(state?.intWithDefault).to.be.eq(42);
            expect(state?.floatTest).closeTo(42.5, 0.001);
            expect(state?.floatWithDefaultTest).closeTo(42.5, 0.001);
            expect(state?.doubleTest).closeTo(42.5, 0.001);
            expect(state?.doubleWithDefaultTest).closeTo(42.5, 0.001);
            expect(state?.stringTest).to.be.eq('foobar');
            expect(state?.stringWithDefaultTest).to.be.eq('foobar');
            expect(state?.objectTest).to.deep.eq({foo: {bar: 'baz'}});
            expect(state?.arrayTest).to.deep.eq(['foo', 'bar']);
            expect(state?.readOnlyArrayTest).to.deep.eq(['foo', 'bar']);
            expect(state?.stringEnumTest).to.be.eq('foo');
            expect(state?.intEnumTest).to.be.eq(1);
          }}
        />
      </TestSuite>
      <TestSuite name="C-API">
        <TestCase.Manual<
          | GeneratedSampleNativeComponentCAPICustomProps
          | undefined
          | 'IN_PROGRESS'
        >
          skip={{
            android: true,
            harmony: {arkTS: 'C-API only test', cAPI: false},
          }}
          itShould="receive props and emit them back via event"
          initialState={undefined}
          arrange={({setState}) => {
            return (
              <Ref<GeneratedSampleComponentCAPIRef>
                render={ref => (
                  <GeneratedSampleComponentCAPI
                    ref={ref}
                    testProps={{
                      booleanTest: true,
                      intTest: 42,
                      floatTest: 42.5,
                      doubleTest: 42.5,
                      stringTest: 'foobar',
                      colorTest: 'red',
                      arrayTest: ['foo', 'bar'],
                      readOnlyArrayTest: ['foo', 'bar'],
                    }}
                    onDirectEvent={setState}>
                    <Effect
                      onMount={() => {
                        setState('IN_PROGRESS');
                        setTimeout(() =>
                          ref.current?.emitNativeEvent('directEvent'),
                        );
                      }}
                    />
                  </GeneratedSampleComponentCAPI>
                )}
              />
            );
          }}
          assert={async ({expect, state}) => {
            if (state === 'IN_PROGRESS') {
              await new Promise(() => {});
              return;
            }
            expect(state?.booleanTest).to.be.true;
            expect(state?.booleanWithDefaultTest).to.be.true;
            expect(state?.intTest).to.be.eq(42);
            expect(state?.intWithDefault).to.be.eq(42);
            expect(state?.floatTest).closeTo(42.5, 0.1);
            expect(state?.floatWithDefaultTest).closeTo(42.5, 0.1);
            expect(state?.doubleTest).closeTo(42.5, 0.1);
            expect(state?.doubleWithDefaultTest).closeTo(42.5, 0.1);
            expect(state?.stringTest).to.be.eq('foobar');
            expect(state?.stringWithDefaultTest).to.be.eq('foobar');
            expect(state?.arrayTest).to.deep.eq(['foo', 'bar']);
            expect(state?.readOnlyArrayTest).to.deep.eq(['foo', 'bar']);
            expect(state?.stringEnumTest).to.be.eq('foo');
            expect(state?.colorTest).to.be.not.undefined;
          }}
        />
        <TestCase.Manual<
          | GeneratedSampleNativeComponentCAPICommandArgs
          | undefined
          | 'IN_PROGRESS'
        >
          skip={{
            android: true,
            harmony: {arkTS: 'C-API only test', cAPI: false},
          }}
          itShould="receive command args and emit them back via event"
          initialState={undefined}
          arrange={({setState}) => {
            return (
              <Ref<GeneratedSampleComponentCAPIRef>
                render={ref => (
                  <GeneratedSampleComponentCAPI
                    ref={ref}
                    hidden
                    testProps={{
                      booleanTest: true,
                      intTest: 0,
                      floatTest: 0.0,
                      doubleTest: 0.0,
                      stringTest: '',
                      colorTest: 'blue',
                      arrayTest: [],
                      readOnlyArrayTest: [],
                    }}
                    onReceivedCommandArgs={setState}>
                    <Effect
                      onMount={() => {
                        setState('IN_PROGRESS');
                        setTimeout(() => {
                          ref.current?.emitCommandArgs(
                            42 /* intTest */,
                            42.42 /* floatTest */,
                            42.42 /* doubleTest */,
                            'foobar' /* stringTest */,
                            true /* booleanTest */,
                          );
                        });
                      }}
                    />
                  </GeneratedSampleComponentCAPI>
                )}
              />
            );
          }}
          assert={async ({expect, state}) => {
            if (state === 'IN_PROGRESS') {
              await new Promise(() => {});
              return;
            }
            expect(state?.intTest).to.be.eq(42);
            expect(state?.floatTest).closeTo(42.5, 0.1);
            expect(state?.doubleTest).closeTo(42.5, 0.1);
            expect(state?.stringTest).eq('foobar');
            expect(state?.booleanTest).to.be.true;
          }}
        />
      </TestSuite>
      <TestSuite name="generated by codegen-lib">
        <TestCase.Manual
          itShould="mount ArkTS component that relies on the generated code by codegen-lib, and onMount is called only once"
          initialState={{
            text: '',
            count: 0,
          }}
          arrange={({setState}) => {
            return (
              <CodegenLibSampleComponent
                style={{width: 100, height: 100}}
                implementation="ArkTS"
                text="ArkTS"
                onMount={text => {
                  setState(prevState => ({
                    text,
                    count: prevState.count + 1,
                  }));
                }}
              />
            );
          }}
          assert={({expect, state}) => {
            expect(state.text).to.be.eq('ArkTS');
            expect(state.count).to.be.eq(1);
          }}
        />
        <TestCase.Manual
          itShould="mount Cpp component that relies on the generated code by codegen-lib"
          initialState={''}
          arrange={({setState}) => {
            return (
              <CodegenLibSampleComponent
                style={{width: 100, height: 100}}
                implementation="Cpp"
                text="Cpp"
                onMount={text => {
                  setState(text);
                }}
              />
            );
          }}
          assert={({expect, state}) => {
            expect(state).to.be.eq('Cpp');
          }}
        />
      </TestSuite>
    </TestSuite>
  );
}

function ContainerViewTest() {
  return (
    <TestSuite name="Custom container component">
      <TestCase.Example itShould="display a custom component with children">
        <ContainerView>
          <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
          <Text>Content</Text>
        </ContainerView>
      </TestCase.Example>

      <TestCase.Example itShould="display a custom component with toggleable child">
        <ContainerView>
          <Blinker>
            <View style={{width: 100, height: 100, backgroundColor: 'red'}} />
            <Text>Content</Text>
          </Blinker>
        </ContainerView>
      </TestCase.Example>
    </TestSuite>
  );
}