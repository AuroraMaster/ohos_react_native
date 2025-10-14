/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {Linking} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {Button, TestCase} from '../components';

export function LinkingTest() {
  return (
    <TestSuite name="Linking">
      <TestCase.Logical
        skip={'This test always fails'}
        itShould="support http/https urls"
        fn={async ({expect}) => {
          expect(await Linking.canOpenURL('http://foobar.com')).to.be.true;
          expect(await Linking.canOpenURL('https://foobar.com')).to.be.true;
        }}
      />
      <TestCase.Logical
        skip={{android: true, harmony: false}}
        itShould="support tel urls"
        fn={async ({expect}) => {
          expect(await Linking.canOpenURL('tel:1234567890')).to.be.true;
        }}
      />
      <TestCase.Logical
        skip={{android: true, harmony: false}}
        itShould="support sms urls"
        fn={async ({expect}) => {
          expect(await Linking.canOpenURL('sms:1234567890')).to.be.true;
        }}
      />
      <TestCase.Logical
        skip={{android: true, harmony: true}}
        itShould="support map url"
        fn={async ({expect}) => {
          expect(await Linking.canOpenURL('maps://navigation')).to.be.true;
        }}
      />
      <TestCase.Logical
        itShould="not support wrong schema urls"
        fn={async ({expect}) => {
          expect(await Linking.canOpenURL('wrong://host')).to.be.false;
        }}
      />
      <TestCase.Logical
        skip={{android: true, harmony: false}}
        itShould="receive linking events"
        fn={async ({expect}) => {
          const linkingEvent = await new Promise((resolve, reject) => {
            const timeout = setTimeout(() => {
              reject('Linking event timed out');
            }, 2000);
            Linking.addEventListener('url', ({url}) => {
              resolve(url);
              clearTimeout(timeout);
            });
            Linking.openURL('rntester://rnoh-test-linking');
          });
          expect(linkingEvent).to.equal('rntester://rnoh-test-linking');
        }}
      />
      <TestCase.Example itShould="open phone dialing on press">
        <Button
          onPress={() => Linking.openURL('tel:1234567890')}
          label="Call 1234567890"
        />
      </TestCase.Example>
      <TestCase.Example itShould="open sms app (sms app doesn't accept parameters)">
        <Button
          onPress={() => Linking.openURL('sms:1234567890')}
          label="Send SMS to 1234567890"
        />
      </TestCase.Example>
      <TestCase.Example itShould="open map app on press">
        <Button
          onPress={() => Linking.openURL('maps://navigation')}
          label="Open Map app"
        />
      </TestCase.Example>
      <TestCase.Example itShould="open web page on press">
        <Button
          onPress={() =>
            Linking.openURL('https://reactnative.dev/docs/linking')
          }
          label="Open Linking Docs"
        />
      </TestCase.Example>
      <TestCase.Example itShould="open (http) web page on press">
        <Button
          onPress={() => Linking.openURL('http://info.cern.ch/')}
          label="Open an old webpage"
        />
      </TestCase.Example>
      <TestCase.Example itShould="open application settings">
        <Button onPress={() => Linking.openSettings()} label="Open settings" />
      </TestCase.Example>
      <TestCase.Example itShould="fail on bad url">
        <Button
          onPress={() =>
            Linking.openURL('bad://url').catch(e => console.warn(e))
          }
          label="Bad URL"
        />
      </TestCase.Example>
    </TestSuite>
  );
}
