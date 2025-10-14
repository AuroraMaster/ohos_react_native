/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../components';
import {SampleTurboModule} from 'react-native-harmony-sample-package';

export function ErrorHandlingTest() {
  return (
    <TestSuite name="ErrorHandling">
      <ErrorBoxTestCase
        itShould="display error box with 'ERROR_MESSAGE' on the red background, a native stack of that error and '(rnInstanceName=app_name, rnInstanceId=0)' in the details"
        errorData={{whatHappened: 'ERROR_MESSAGE', howCanItBeFixed: []}}
      />
      <ErrorBoxTestCase
        itShould="display one suggestion"
        errorData={{
          whatHappened: 'ERROR_MESSAGE',
          howCanItBeFixed: ['Press "Dismiss"'],
        }}
      />
      <ErrorBoxTestCase
        itShould="display two suggestions"
        errorData={{
          whatHappened: 'ERROR_MESSAGE',
          howCanItBeFixed: ['Press "Dismiss"', 'Press "Reload'],
        }}
      />
      <ErrorBoxTestCase
        itShould="display custom stack (foo/bar/baz) instead of a native stack"
        errorData={{
          whatHappened: 'ERROR_MESSAGE',
          howCanItBeFixed: [],
          customStack: 'foo\nbar\nbaz',
        }}
      />
      <ErrorBoxTestCase
        itShould="display object extra data as JSON"
        errorData={{
          whatHappened: 'ERROR_MESSAGE',
          howCanItBeFixed: [],
          extraData: {foo: 'bar'},
        }}
      />
      <TestCase.Example itShould="display exception thrown from turbomodule (CPP)">
        <Button
          label="Throw Exception"
          onPress={() => {
            SampleTurboModule.throwExceptionCpp();
          }}
        />
      </TestCase.Example>
      <TestCase.Example itShould="display exception thrown from turbomodule (ArkTS)">
        <Button
          label="Throw Exception"
          onPress={() => {
            SampleTurboModule.throwExceptionArk();
          }}
        />
      </TestCase.Example>
      <TestCase.Example itShould="not display error when exception is caught">
        <Button
          label="Throw Exception"
          onPress={() => {
            try {
              SampleTurboModule.throwExceptionCpp();
            } catch (e) {
              // do nothing
            }
          }}
        />
      </TestCase.Example>
    </TestSuite>
  );
}

function ErrorBoxTestCase({
  itShould,
  errorData,
}: {
  itShould: string;
  errorData: {
    whatHappened: string;
    howCanItBeFixed: string[];
    extraData?: any;
    customStack?: string;
  };
}) {
  return (
    <TestCase.Example itShould={itShould}>
      <Button
        label="Show Error Box"
        onPress={() => {
          SampleTurboModule.displayRNOHError(errorData);
        }}
      />
    </TestCase.Example>
  );
}
