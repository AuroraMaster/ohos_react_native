/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  EnterKeyHintType,
  EnterKeyHintTypeAndroid,
  KeyboardTypeOptions,
  Platform,
  ReturnKeyType,
  ReturnKeyTypeAndroid,
  StyleSheet,
  Text,
  TextInput,
  TextInputProps,
  View,
  ScrollView
} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {useState, useRef, useEffect} from 'react';
import {Button, Effect, StateKeeper, TestCase} from '../components';

const KEYBOARD_TYPES: KeyboardTypeOptions[] = [
  'default',
  'number-pad',
  'decimal-pad',
  'numeric',
  'email-address',
  'phone-pad',
  'url',
];

export function TextInputTest() {
  return (
    <TestSuite name="TextInput">
      <TestCase.Example itShould="render textinput and change the text component based on the values inputted">
        <TextInputWithText style={styles.textInput} />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="not crash when invalid keyboardType is supplied">
        <TextInputWithText
          style={styles.textInput}
          keyboardType={'invalid_type' as KeyboardTypeOptions}
        />
      </TestCase.Example>
      <TestCase.Example itShould="render textinput with set content">
        <TextInputWithTextContent style={styles.textInput} />
      </TestCase.Example>
      <TestCase.Example itShould="render non-editable textInput">
        <TextInputWithText
          defaultValue="test"
          style={styles.textInput}
          editable={false}
        />
      </TestCase.Example>
      <TestCase.Example itShould="render textInput with Pacifico Regular font">
        <TextInputWithText
          style={[styles.textInput, {fontFamily: 'Pacifico-Regular'}]}
        />
      </TestCase.Example>
      <TestCase.Example itShould="render textInput with caret hidden">
        <TextInputWithText style={styles.textInput} caretHidden />
      </TestCase.Example>
      <TestCase.Example itShould="render textInput with red caret">
        <TextInputWithText style={styles.textInput} cursorColor={'red'} />
      </TestCase.Example>
      <TestCase.Manual
        itShould="report content size changes (onContentSizeChange)"
        initialState={false}
        skip={{android: false, harmony: true}}
        arrange={({setState}) => {
          return (
            <TextInputWithText
              style={styles.textInput}
              multiline
              onContentSizeChange={() => setState(true)}
            />
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example
        modal
        tags={['C_API']}
        itShould="report content size changes with parameter(onContentSizeChange)">
        <OnContentSizeChangeView />
      </TestCase.Example>
      <TestSuite name="focus/blur">
        <TestCase.Manual
          itShould="blur text on submit (singleline)"
          initialState={false}
          arrange={({setState}) => {
            return (
              <>
                <TextInputWithText
                  style={styles.textInput}
                  blurOnSubmit
                  onBlur={() => setState(true)}
                />
              </>
            );
          }}
          assert={({expect, state}) => {
            expect(state).to.be.true;
          }}
        />
        <TestCase.Manual
          itShould="blur text after switching to another textinput"
          initialState={false}
          arrange={({setState}) => {
            return (
              <>
                <TextInputWithText
                  style={styles.textInput}
                  onBlur={() => setState(true)}
                />
                <TextInputWithText
                  style={styles.textInput}
                  onBlur={() => setState(true)}
                />
              </>
            );
          }}
          assert={({expect, state}) => {
            expect(state).to.be.true;
          }}
        />
        <TestCase.Example
          modal
          itShould="not blur text on submit"
          skip={{android: false, harmony: true}}
          //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/403
        >
          <TextInputWithText style={styles.textInput} blurOnSubmit={false} />
          <TextInputWithText
            style={styles.textInput}
            blurOnSubmit={false}
            multiline
          />
        </TestCase.Example>
        <TestCase.Manual
          modal
          itShould="automatically focus textInput when displayed"
          initialState={false}
          arrange={({setState}) => (
            <TextInputWithText
              style={styles.textInput}
              autoFocus
              onFocus={() => setState(true)}
            />
          )}
          assert={({expect, state}) => {
            expect(state).to.be.true;
          }}
        />
        <TestCase.Manual
          itShould="focus textInput on click"
          initialState={false}
          arrange={({setState}) => (
            <TextInput
              style={styles.textInput}
              onFocus={() => setState(true)}
            />
          )}
          assert={({expect, state}) => {
            expect(state).to.be.true;
          }}
        />
        <TestCase.Manual
          modal
          itShould="focus textInput when pressing the button"
          initialState={false}
          arrange={({setState}) => <FocusTextInputTest setState={setState} />}
          assert={({state, expect}) => {
            expect(state).to.be.true;
          }}
        />
        <TestCase.Manual
          modal
          itShould="When the TextInput is unmounted, other components will not gain focus passively.​"
          initialState={'init'}
          arrange={({setState}) => <DisableFocusPassivelyTest setState={setState} />}
          assert={({state, expect}) => {
            expect(state).to.be.eq('pass');
          }}
        />
      </TestSuite>
      <TestCase.Example
        modal
        itShould="render textInput with blue underline"
        skip={{android: false, harmony: true}}
        //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/404
      >
        <TextInputWithText
          style={styles.textInput}
          underlineColorAndroid={'blue'}
        />
      </TestCase.Example>
      <TestCase.Example itShould="render textinput with red placeholder">
        <TextInputWithText
          style={styles.textInput}
          placeholder="Placeholder"
          placeholderTextColor={'red'}
        />
      </TestCase.Example>
      <TestCase.Example itShould="render textinput with default selection color">
        <TextInputWithText style={styles.textInput} />
      </TestCase.Example>
      <TestCase.Example itShould="render textinput with green selection color">
        <TextInputWithText style={styles.textInput} selectionColor="green" />
      </TestCase.Example>
      <TestCase.Example itShould="render multiline text input">
        <TextInputWithText style={styles.textInputBigger} multiline={true} />
      </TestCase.Example>
      <TestCase.Example itShould="render multiline text input with common border">
        <TextInputWithText style={styles.multilineInput} multiline={true} />
      </TestCase.Example>
      <TestCase.Example itShould="render multiline text input with Pacifico Regular font">
        <TextInputWithText
          style={[styles.textInputBigger, {fontFamily: 'Pacifico-Regular'}]}
          multiline
        />
      </TestCase.Example>
      <TestCase.Example itShould="render text input with maximally 10 characters">
        <TextInputWithText style={styles.textInput} maxLength={10} />
      </TestCase.Example>
      <TestCase.Example modal itShould="toggle between maxLength 5 and 0">
        <SwitchBetweenDiffMaxLength />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="toggle between rendering 10 and 5 characters">
        <StateKeeper
          initialValue={10}
          renderContent={(maxLength, setMaxLength) => {
            return (
              <Effect
                onMount={() => {
                  const interval = setInterval(() => {
                    setMaxLength(prev => (prev === 10 ? 5 : 10));
                  }, 1000);
                  return () => {
                    clearInterval(interval);
                  };
                }}>
                <TextInputWithText
                  style={styles.textInput}
                  maxLength={maxLength}
                  value="1234567890"
                />
              </Effect>
            );
          }}
        />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="toggle between different capitalization modes"
        skip={{android: false, harmony: true}}
        //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/408
      >
        <AutoCapitalize />
      </TestCase.Example>
      <TestCase.Manual
        modal
        itShould="trigger onSubmitEditing event after submiting"
        initialState={false}
        arrange={({setState}) => (
          <TextInputWithText
            style={styles.textInput}
            onSubmitEditing={() => setState(true)}
          />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example modal itShould="toggle between different return keys">
        <ReturnKeyTypeView />
      </TestCase.Example>
      <TestCase.Example modal itShould="toggle between different enter keys">
        <EnterKeyHintExample />
      </TestCase.Example>
      <TestCase.Example itShould="render secure text input (text obscured)">
        <TextInputWithText style={styles.textInput} secureTextEntry />
      </TestCase.Example>
      <TestCase.Manual
        modal
        itShould="trigger onKeyPress event after pressing delete key (press 'Backspace' to pass)"
        initialState={''}
        arrange={({setState}) => (
          <TextInputWithText
            style={styles.textInput}
            autoFocus
            onKeyPress={event => {
              setState(event.nativeEvent.key);
            }}
          />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.eq('Backspace');
        }}
      />
      <TestCase.Manual
        modal
        itShould="trigger onKeyPress event after pressing key (press 'A' to pass)"
        initialState={''}
        arrange={({setState}) => (
          <TextInputWithText
            style={styles.textInput}
            autoFocus
            onKeyPress={event => {
              setState(event.nativeEvent.key);
            }}
          />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.eq('A');
        }}
      />
      <TestCase.Manual
        modal
        skip={{android: false, harmony: {arkTS: true, cAPI: false}}}
        //https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/736
        itShould="trigger onKeyPress event after pressing backspace"
        initialState={''}
        arrange={({setState}) => (
          <>
            <TextInputWithText
              style={styles.textInput}
              autoFocus
              defaultValue="a"
              onKeyPress={event => setState(event.nativeEvent.key)}
            />
            <TextInputWithText
              style={styles.textInputBigger}
              autoFocus
              multiline
              defaultValue="a"
              onKeyPress={event => setState(event.nativeEvent.key)}
            />
          </>
        )}
        assert={({expect, state}) => {
          expect(state).to.be.eq('Backspace');
        }}
      />
      <TestCase.Example
        modal
        itShould="show text input with default value (defaultProps)">
        <DefaultProps />
      </TestCase.Example>
      <TestCase.Example itShould="show text input with default value (defaultValue)">
        <TextInput style={styles.textInput} defaultValue="defaultText" />
      </TestCase.Example>
      <TestCase.Manual
        modal
        itShould="trigger onLayout event on mount"
        initialState={{}}
        arrange={({setState, state}) => {
          return (
            <>
              <Text>{JSON.stringify(state)}</Text>
              <TextInput
                style={styles.textInput}
                onLayout={event => {
                  setState(event.nativeEvent.layout);
                }}
              />
            </>
          );
        }}
        assert={({expect, state}) => {
          expect(state).to.include.all.keys('width', 'height', 'x', 'y');
        }}
      />
      <TestCase.Example
        modal
        itShould="render textinputs with different keyboard types">
        <View>
          {KEYBOARD_TYPES.map(type => (
            <TextInputKeyboardType
              key={`singleline_${type}`}
              keyboardType={type}
            />
          ))}
        </View>
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="render multiline textinputs with different keyboard types">
        <View>
          {KEYBOARD_TYPES.map(type => (
            <TextInputKeyboardType
              key={`multiline_${type}`}
              keyboardType={type}
              multiline
            />
          ))}
        </View>
      </TestCase.Example>
      <TestCase.Example modal itShould="render textinput with allowFontScaling">
        <TextInputWithText
          style={styles.textInput}
          allowFontScaling
          defaultValue="Scaled"
        />
        <TextInputWithText
          style={styles.textInput}
          allowFontScaling={false}
          defaultValue="Not scaled"
        />
        <TextInputWithText
          style={[styles.textInput, {fontSize: 40}]}
          allowFontScaling={true}
          defaultValue="Scaled big"
        />
        <TextInputWithText
          style={[styles.textInput, {fontSize: 40}]}
          allowFontScaling={false}
          defaultValue="Not scaled big"
        />
      </TestCase.Example>
      <TestCase.Example itShould="show textInput with padding" modal>
        <View style={{width: 300, height: 200}}>
          <TextInputWithText
            style={{
              paddingLeft: 10,
              paddingTop: 20,
              paddingRight: 30,
              paddingBottom: 40,
              backgroundColor: 'red',
            }}
          />
        </View>
      </TestCase.Example>
      <TestCase.Example itShould="show textInput multiline with padding" modal>
        <View style={{width: 300, height: 200}}>
          <TextInputWithText
            style={{
              paddingLeft: 10,
              paddingTop: 20,
              paddingRight: 30,
              paddingBottom: 40,
              backgroundColor: 'red',
              height: 100,
            }}
            multiline
          />
        </View>
      </TestCase.Example>
      <TestCase.Example modal itShould="render textinput with readonly">
        <TextInputWithText
          style={styles.textInput}
          defaultValue="readOnly"
          // @ts-ignore
          readOnly
        />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display bold, italic, large placeholder with a custom font">
        <TextInput
          style={{
            fontFamily: 'StintUltraCondensed-Regular',
            fontWeight: 'bold',
            fontSize: 24,
            fontStyle: 'italic',
          }}
          placeholder="placeholder"
        />
      </TestCase.Example>
      <TestCase.Example modal itShould="render textinput with red text color">
        <TextInputWithText style={[styles.textInput, {color: 'red'}]} />
      </TestCase.Example>
      <TestCase.Example modal itShould="clear text on focus">
        {/* iOS only */}
        <TextInputWithText
          style={styles.textInput}
          defaultValue="Hello, World!"
          clearTextOnFocus
        />
      </TestCase.Example>
      <TestCase.Example
        // Only "unless-editing" mode doesn't work on C_API
        modal
        itShould="use different clearButtonMode values">
        <TextInputWithText style={styles.textInput} clearButtonMode="always" />
        <Text>clearButtonMode="always"</Text>
        {
          /* Except Android */
          Platform.OS !== 'android' && (
            <>
              <TextInputWithText
                style={styles.textInput}
                clearButtonMode="while-editing"
              />
              <Text>clearButtonMode="while-editing"</Text>
              <TextInputWithText
                style={styles.textInput}
                clearButtonMode="unless-editing"
              />
              <Text>clearButtonMode="unless-editing"</Text>
            </>
          )
        }
        <TextInputWithText style={styles.textInput} clearButtonMode="never" />
        {/* You cannot see inputs with */}
        <Text style={{marginBottom: 200}}>clearButtonMode="never"</Text>
      </TestCase.Example>
      <TestCase.Example modal itShould="show selection start and end values">
        <TextSelectionChangeTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="show textinput with 'Selection' substring selected">
        <SelectionTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="show controlled textinput with 'Selection' substring selected">
        <ControlledSelectionTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="not show text context menu when long press">
        <ContextMenuHiddenTest />
      </TestCase.Example>
      <TestCase.Example modal itShould="select text on focus">
        <TextInput
          value="selectTextOnFocus"
          selectTextOnFocus
          style={styles.textInput}
        />
        <TextInput
          value={'selectTextOnFocus \n but multiline'}
          selectTextOnFocus
          multiline
          style={[styles.textInputBigger, {marginTop: 10}]}
        />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="allow to change the text input value via props">
        <TextInputValueSetProgrammatically />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="should show transparent TextInput on a red background">
        <View style={{backgroundColor: 'red'}}>
          <TextInput
            value="parent is red but I am transparent"
            style={{height: 40}}
          />
        </View>
      </TestCase.Example>
      <TestCase.Manual
        modal
        itShould="trigger onEndEditing event after editing ends"
        initialState={false}
        arrange={({setState}) => (
          <TextInputWithText
            style={styles.textInput}
            autoFocus
            onEndEditing={() => setState(true)}
          />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        modal
        itShould="trigger onChange event after changing value"
        initialState={false}
        arrange={({setState}) => (
          <TextInputWithText onChange={() => setState(true)} />
        )}
        assert={({expect, state}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example
        modal
        itShould="set cursorColor to red after pressing button (setNativeProps)">
        <SetNativePropsTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="select only Selection word (selectTextOnFocus && selection)">
        <TextInput
          style={styles.textInput}
          selection={{start: 4, end: 13}}
          value="TextSelectionTest"
          selectTextOnFocus
        />
      </TestCase.Example>
      <TestCase.Manual
        modal
        itShould="not trigger onChange event after changing value through state"
        initialState={{text: '', onChangeFired: false}}
        arrange={({setState, state}) => {
          return (
            <View>
              <TextInput
                style={styles.textInput}
                value={state.text}
                onChange={() => {
                  setState(prev => ({...prev, onChangeFired: true}));
                }}
              />
              <Button
                label="change text"
                onPress={() =>
                  setState(prev => ({...prev, text: prev.text + 'a'}))
                }
              />
            </View>
          );
        }}
        assert={({expect, state}) => {
          expect(state.onChangeFired).to.be.false;
          expect(state.text).to.not.be.equal('');
        }}
      />
      <TestCase.Example
        modal
        itShould="change the text input value programmatically on blur">
        <TextInputValueSetProgrammaticallyOnBlur />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="change the text input value programmatically when submitted">
        <TextInputValueSetProgrammaticallyWhenSubmitted />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="extended textContentType test (based on SDK API 16)">
        <TextInputTextContentType />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="When entering a bank card number, the cursor should
        automatically move past the space after encountering one.">
        <TextInputBankCardNumber />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="cursor should be at the end when focusing a textinput with value"
      >
        <FocusWithCursorTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="show textinput with predefined selection">
        <SelectionTestComponent />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display onKeyPress event data">
        <OnKeyPressTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="display onChange event data">
        <OnChangeTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="clear textinput by calling clear method">
        <ClearButtonTest />
      </TestCase.Example>
      <TestCase.Example
        modal
        itShould="format zipcode input (add dash after 5 digits)">
        <ZipCodeTextInputMask />
      </TestCase.Example>
    </TestSuite>
  );
}

const SetNativePropsTest = () => {
  const ref = useRef<TextInput>(null);

  return (
    <View>
      <TextInput cursorColor={'blue'} ref={ref} />
      <Button
        label="setCursorColor"
        onPress={() => ref.current?.setNativeProps({cursorColor: 'red'})}
      />
    </View>
  );
};

const SelectionTest = () => {
  return (
    <View>
      <TextInput
        style={styles.textInput}
        value="TextSelectionTest"
        selection={{start: 4, end: 13}}
      />
      <TextInput
        style={[styles.textInputBigger, {marginTop: 10}]}
        value={'TextSelect\nionTest'}
        selection={{start: 4, end: 13}}
        multiline
      />
    </View>
  );
};

const ContextMenuHiddenTest = () => {
  return (
    <View>
      <TextInput
        style={styles.textInput}
        value="ContextMenuHiddenTest"
        contextMenuHidden={true}
      />
      <TextInput
        style={[styles.textInput, {marginTop: 5}]}
        value="ContextMenuHiddenTest_multiline"
        multiline={true}
        contextMenuHidden={true}
      />
    </View>
  );
};

const TextSelectionChangeTest = () => {
  const [eventData, setEventData] = useState('');
  return (
    <View>
      <TextInput
        style={styles.textInput}
        onSelectionChange={event =>
          setEventData(JSON.stringify(event.nativeEvent.selection))
        }
      />
      <Text style={{marginVertical: 8}}>Selection {eventData}</Text>
    </View>
  );
};

const FocusTextInputTest = (props: {
  setState: React.Dispatch<React.SetStateAction<boolean>>;
}) => {
  const ref = useRef<TextInput>(null);
  return (
    <View>
      <Button label="focus text input" onPress={() => ref.current?.focus()} />
      <TextInput onFocus={() => props.setState(true)} ref={ref} />
    </View>
  );
};

const DisableFocusPassivelyTest = (props: {
  setState: React.Dispatch<React.SetStateAction<string>>;
}) => {
  const ref = useRef<TextInput>(null);
  const [show, setShow] = useState(true);
  const [firstFocus, setFirstFocus] = useState(false);
  const [secondFocus, setSecondFocus] = useState(false);
  const [done, setDone] = useState(false);
  useEffect(() => {
    setTimeout(() => {
      setShow(false);
    }, 300);
    setTimeout(() => {
      setShow(true);
    }, 400);
    setTimeout(() => {
      setShow(false);
      ref.current?.focus();
    }, 500);
    setTimeout(() => {
      setDone(true)
    }, 600);
  }, [])
  useEffect(() => {
    if (done) {
      props.setState((firstFocus && !secondFocus) ? 'pass' : 'fail');
    }
  }, [done])

  return (
    <View collapsable={false} style={{gap: 10}}>
      <TextInput
        ref={ref}
        placeholder="TextInput1"
        style={{backgroundColor: 'pink', height: 40}}
        onFocus={() => {
          setFirstFocus(true);
        }}
      />
      <TextInput
        placeholder="TextInput2"
        style={{backgroundColor: 'orange', height: 40}}
        onFocus={() => {
          setSecondFocus(true);
        }}
      />
      {show ? (
        <TextInput
          placeholder="TextInput3"
          autoFocus={true}
          style={{backgroundColor: 'blue', height: 40}}
        />
      ) : null}
    </View>
  );
};

const TextInputKeyboardType = (props: TextInputProps) => {
  return (
    <>
      <Text>{props.keyboardType}</Text>
      <TextInput
        style={{...styles.textInputSmall, marginBottom: 10}}
        keyboardType={props.keyboardType}
        multiline={props.multiline}
      />
    </>
  );
};

const TextInputWithTextContent = (props: TextInputProps) => {
  const texts = [
    'Text content',
    'Hello, World!',
    'Lorem ipsum dolor sit amet, consectetur adipiscing elit.',
  ];
  const [index, setIndex] = useState(0);

  return (
    <>
      <TextInput {...props}>{texts[index]}</TextInput>
      <Button
        label="Toggle text"
        onPress={() => setIndex(i => (i + 1) % texts.length)}
      />
    </>
  );
};

const TextInputWithText = (props: TextInputProps) => {
  const [text, onChangeText] = useState(props.defaultValue ?? '');
  return (
    <>
      <Text style={styles.text}>{text}</Text>
      <TextInput {...props} onChangeText={onChangeText} value={text} />
    </>
  );
};
type CapitalizationType = 'none' | 'sentences' | 'words' | 'characters';
const AutoCapitalize = () => {
  const [state, setState] = useState<CapitalizationType>('none');
  const capitalizations: Array<CapitalizationType> = [
    'none',
    'sentences',
    'words',
    'characters',
  ];
  const toggleCapitalization = () => {
    const index = capitalizations.indexOf(state);
    setState(capitalizations[(index + 1) % capitalizations.length]);
  };
  return (
    <>
      <TextInputWithText style={styles.textInput} autoCapitalize={state} />
      <Button label="toggle capitalize mode" onPress={toggleCapitalization} />
      <Text>Capitalize mode: {state}</Text>
    </>
  );
};
const SwitchBetweenDiffMaxLength = () => {
  const [state, setState] = useState(5)

  return (
    <>
      <TextInputWithText style={styles.textInput} maxLength={state} />
      <Button label="toggle return key type" onPress={() => {setState(state == 0 ? 5 : 0)}} />
      <Text>maxLength: {state}</Text>
    </>
  );
}
const ReturnKeyTypeView = () => {
  const [state, setState] = useState<ReturnKeyType | ReturnKeyTypeAndroid>(
    'none',
  );
  const returnKey: Array<ReturnKeyType | ReturnKeyTypeAndroid> = [
    'none',
    'done',
    'go',
    'next',
    'search',
    'send',
    'none',
    'previous', //currently not supported by ArkUI
  ];
  const toggleReturnKey = () => {
    const index = returnKey.indexOf(state);
    setState(returnKey[(index + 1) % returnKey.length]);
  };
  return (
    <>
      <TextInputWithText style={styles.textInput} returnKeyType={state} />
      <Button label="toggle return key type" onPress={toggleReturnKey} />
      <Text>Return key: {state}</Text>
    </>
  );
};
const OnContentSizeChangeView = () => {
  const [width, setWidth] = useState<number>(0);
  const [height, setHeight] = useState<number>(0);
  const [inputWidth, setInputWidth] = useState<number>(0);
  const [inputHeight, setInputHeight] = useState<number>(0);
  const renderSize = (event: any) => {
    setWidth(event.nativeEvent.contentSize.width);
    setHeight(event.nativeEvent.contentSize.height);
  }
  const renderInputSize = (event: any) => {
    setInputWidth(event.nativeEvent.contentSize.width);
    setInputHeight(event.nativeEvent.contentSize.height);
  }
  return (
    <View style={{backgroundColor: 'black'}}>
      <Text style={styles.text}>Textinput content size:</Text>
      <TextInputWithText
        style={styles.textInput}
        onContentSizeChange={(event) => renderInputSize(event)}
      />
      <Text style={styles.text}>input content size width: {inputWidth}</Text>
      <Text style={styles.text}>input content size height: {inputHeight}</Text>
      <Text style={styles.text}>Multiline textinput content size:</Text>
      <TextInputWithText
        style={styles.textInputBigger}
        multiline
        onContentSizeChange={(event) => renderSize(event)}
      />
      <Text style={styles.text}>content size width: {width}</Text>
      <Text style={styles.text}>content size height: {height}</Text>
    </View>
  );
}
const EnterKeyHintExample = () => {
  const [index, setIndex] = useState(0);
  const enterKey: Array<EnterKeyHintType | EnterKeyHintTypeAndroid> = [
    'done',
    'go',
    'next',
    'previous',
    'search',
    'send',
  ];
  const toggleEnterKey = () => {
    setIndex(i => (i + 1) % enterKey.length);
  };
  return (
    <>
      <TextInputWithText
        style={styles.textInput}
        enterKeyHint={enterKey[index]}
      />
      <Button label="toggle enter key type" onPress={toggleEnterKey} />
      <Text>Enter key: {enterKey[index]}</Text>
    </>
  );
};

const DefaultProps = () => {
  // @ts-ignore
  TextInput.defaultProps = {
    value: 'defaultText',
  };

  return <TextInput style={styles.textInput} />;
};

const TextInputValueSetProgrammatically = () => {
  const [value, setValue] = useState('Default Text Input Value');

  return (
    <View style={{height: 400}}>
      <View style={{marginBottom: 16}}>
        <Button
          label="Set 'Hello, World!' as text input value "
          onPress={() => setValue('Hello, World!')}
        />
        <Button
          label="Set 'Lorem ipsum dolor sit amet, consectetur adipiscing elit.' as text input value "
          onPress={() =>
            setValue('Lorem ipsum dolor sit amet, consectetur adipiscing elit.')
          }
        />
        <Button label="Clear text input value" onPress={() => setValue('')} />
      </View>
      <TextInput
        style={styles.textInput}
        defaultValue={value}
        value={value}
        onChangeText={setValue}
      />
    </View>
  );
};

const ControlledSelectionTest = () => {
  const [value, setValue] = useState('TextSelectionTest');
  const [valueM, setValueM] = useState('TextSelect\nionTest');

  return (
    <View>
      <TextInput
        style={styles.textInput}
        value={value}
        onChange={event => setValue(event.nativeEvent.text)}
        selection={{start: 4, end: 13}}
      />
      <TextInput
        style={[styles.textInputBigger, {marginTop: 10}]}
        value={valueM}
        onChange={event => setValueM(event.nativeEvent.text)}
        selection={{start: 4, end: 13}}
        multiline
      />
    </View>
  );
};

const TextInputValueSetProgrammaticallyOnBlur = () => {
  const [value, setValue] = useState("I'm the first text input.");

  return (
    <>
      <TextInput
        value={value}
        autoFocus
        onChangeText={text => setValue(text)}
        style={styles.textInput}
      />
      <TextInput
        onFocus={() => setValue('It works!')}
        multiline
        defaultValue="Click me and see if the value of the first text input has been changed."
        style={styles.textInputBigger}
      />
    </>
  );
};

const TextInputValueSetProgrammaticallyWhenSubmitted = () => {
  const [value, setValue] = useState('(1) Submit the text input.');

  return (
    <>
      <TextInput
        value={value}
        autoFocus
        onChangeText={text => setValue(text)}
        style={styles.textInput}
      />
      <Button
        label="(2) Press to change the text of the text input"
        onPress={() => setValue('(3) It works!')}
      />
    </>
  );
};

const TextInputTextContentType = () => {

  return (
    <ScrollView>
      <Text>PassportNumber Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="passPortNumber"
      />

      <Text>validity Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="validity"
      />

      <Text>issueAt Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="issueAt"
      />
    
      <Text>organization Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="organization"
      />

      <Text>taxId Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="taxId"
      />

      <Text>addressCityAndState Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="addressCityAndState"
      />

      <Text>flightNumber Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="flightNumber"
      />

      <Text>licenseNumber Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="licenseNumber"
      />

      <Text>licenseFileNumber Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="licenseFileNumber"

      />

      <Text>engineNumber Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="engineNumber"

      />  

      <Text>licenseChassisNumber Input:</Text>
      <TextInput
        style={styles.textInput}
        textContentType="licenseChassisNumber"

      />
    </ScrollView>
  )
}

const TextInputBankCardNumber = () => {
  const [value, setValue] = useState('');

  const handleChangeText = (text) => {
    let ruleArr = [3, 7]
    let newText = ""
    let ruleIndex = 0
    if (text) {
      text = text.replace(/\D/g, '')
      text = text.split(" ").join("")
      text = text.replace(/\s+/g, "")
      for (let i = 0; i < text.length; i++) {
        if (i % ruleArr[ruleIndex] == 0 && i != 0) {
          ruleIndex++
          newText += " " + text[i]
        } else {
          newText += text[i]
        }
      }
    }
    setValue(newText);
  }

  return (
    <View>
      <TextInput
        maxLength={21}
        style={styles.textInput}
        onChangeText={handleChangeText}
        value={value}
      />
    </View>
  )
}

const styles = StyleSheet.create({
  container: {
    width: 80,
    height: 80,
    backgroundColor: 'red',
  },
  text: {
    width: '100%',
    height: 40,
  },
  textInputSmall: {
    height: 20, // hack
    fontSize: 8,
    color: 'black',
    backgroundColor: 'rgb(245, 240, 211)',
    borderRadius: 20,
  },
  textInput: {
    height: 40, // hack
    fontSize: 16,
    color: 'black',
    backgroundColor: 'rgb(245, 240, 211)',
    borderRadius: 40,
  },
  textInputBigger: {
    height: 80, // hack
    fontSize: 16,
    color: 'black',
    backgroundColor: 'rgb(245, 240, 211)',
    borderRadius: 20,
  },
  multilineInput: {
    height: 80,
    borderWidth: 1,
    paddingVertical: 10,
    paddingHorizontal: 15,
    borderRadius: 5,
    marginBottom: 20,
  },
  input: {
    height: 40,
    borderColor: 'gray',
    borderWidth: 1,
    marginBottom: 16,
    paddingHorizontal: 8,
  },
});

// Focus with cursor at position test
const FocusWithCursorTest = () => {
  const ref = useRef<TextInput>(null);
  const [value, setValue] = useState('12345');

  const handleFocus = () => {
    ref.current?.focus();
  };

  return (
    <View>
      <TextInput
        ref={ref}
        style={styles.textInput}
        value={value}
        onChangeText={setValue}
      />
      <Button label="点击后focus" onPress={handleFocus} />
    </View>
  );
};


// Selection test
const SelectionTestComponent = () => {
  return (
    <View>
      <Text>1. start less than end (选中文本): start=2, end=5</Text>
      <TextInput
        style={{
          height: 40,
          borderColor: 'gray',
          borderWidth: 1,
          marginBottom: 16,
          paddingHorizontal: 8,
        }}
        defaultValue="点击输入框可以选择文字"
        selection={{start: 2, end: 5}}
      />

      <Text>2. start equals end (光标位置): start=5, end=5</Text>
      <TextInput
        style={{
          height: 40,
          borderColor: 'gray',
          borderWidth: 1,
          marginBottom: 16,
          paddingHorizontal: 8,
        }}
        defaultValue="点击输入框可以选择文字"
        selection={{start: 5, end: 5}}
      />

      <Text>3. start greater than end (反向选择): start=8, end=3</Text>
      <TextInput
        style={{
          height: 40,
          borderColor: 'gray',
          borderWidth: 1,
          marginBottom: 16,
          paddingHorizontal: 8,
        }}
        defaultValue="点击输入框可以选择文字"
        selection={{start: 8, end: 3}}
      />
    </View>
  );
};

// OnKeyPress test
const OnKeyPressTest = () => {
  const [value, setValue] = useState('');
  const [data, setData] = useState('');

  const handleKeyPress = (event: any) => {
    setData(event.nativeEvent.key);
  };

  return (
    <View>
      <Text>onKeyPress 参数</Text>
      <TextInput
        style={{
          height: 40,
          borderColor: 'gray',
          borderWidth: 1,
          marginBottom: 16,
          paddingHorizontal: 8,
        }}
        value={value}
        onChangeText={setValue}
        onKeyPress={handleKeyPress}
      />
      <Text>keyPress内容为：{data}</Text>
    </View>
  );
};

// OnChange test
const OnChangeTest = () => {
  const [value, setValue] = useState('');
  const [onChangeData, setOnChangeData] = useState('');

  const handleChange = (text: string) => {
    setValue(text);
  };

  const handleChangeEvent = (event: any) => {
    setOnChangeData(event.nativeEvent.text);
  };

  return (
    <View>
      <Text>onChange 参数</Text>
      <TextInput
        style={{
          height: 40,
          borderColor: 'gray',
          borderWidth: 1,
          marginBottom: 16,
          paddingHorizontal: 8,
        }}
        value={value}
        onChangeText={handleChange}
        onChange={handleChangeEvent}
      />
      <Text>当前输入值: {value}</Text>
      <Text>onChange内容为：{onChangeData}</Text>
    </View>
  );
};

// Clear button test
const ClearButtonTest = () => {
  const ref = useRef<TextInput>(null);
  const [value, setValue] = useState('');

  return (
    <View>
      <Text>点击按钮调用clear清空</Text>
      <TextInput
        style={styles.input}
        placeholder="输入输入"
        value={value}
        onChangeText={setValue}
        ref={ref}
      />
      <Button label="清空" onPress={() => ref.current?.clear()} />
    </View>
  );
};

// ZipCode format test
const ZipCodeTextInputMask = () => {
  const [code, setCode] = useState('');
  const [rawCode, setRawCode] = useState('');

  const handleCodeChange = (text: string) => {
    // 保存原始值（去除横线）
    const raw = text.replace(/-/g, '');
    setRawCode(raw);

    // 格式化：前5位后加横线
    let formatted = '';
    for (let i = 0; i < raw.length; i++) {
      if (i === 5) {
        formatted += '-' + raw[i];
      } else {
        formatted += raw[i];
      }
    }
    setCode(formatted);
  };

  return (
    <View>
      <TextInput
        value={code}
        onChangeText={handleCodeChange}
        style={styles.input}
        placeholder="输入邮政编码"
        keyboardType="number-pad"
      />
      <Text>格式化值: {code}</Text>
      <Text>原始值: {rawCode}</Text>

      <View style={{marginTop: 10, backgroundColor: '#f0f0f0', padding: 10}}>
        <Text style={{fontWeight: 'bold'}}>测试用例：</Text>
        <Text>输入 "123" → 期望: "123"</Text>
        <Text>输入 "12345" → 期望: "12345"</Text>
        <Text>输入 "123456" → 期望: "12345-6"</Text>
        <Text>输入 "12345678" → 期望: "12345-678"</Text>
      </View>
    </View>
  );
};

