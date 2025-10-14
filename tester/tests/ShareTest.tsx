/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {useState} from 'react';
import {Button, Share, StyleSheet, Text, View} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../components';

// Skipped - https://gl.swmansion.com/rnoh/react-native-harmony/-/issues/357
export function ShareTest() {
  return (
    <TestSuite name="Linking (Stub)">
      <TestCase.Example
        skip={{android: false, harmony: true}}
        itShould="Share text message">
        <ShareMessageWithoutTitleTest />
      </TestCase.Example>
      <TestCase.Example
        skip={{android: false, harmony: true}}
        itShould="Share message, URL (iOS) and title (Android)">
        <ShareMessageWithTitleTest />
      </TestCase.Example>
      <TestCase.Example
        skip={{android: false, harmony: true}}
        itShould="sharedAction: If the content was successfully shared">
        <SharedActionTest />
      </TestCase.Example>
    </TestSuite>
  );
}

const shareMessage = () => {
  Share.share({
    message:
      'Our top priority for React Native is to match the expectations people have for each platform. This is why React Native renders to platform primitives. We value native look-and-feel over cross-platform consistency.' +
      'For example, the TextInput in React Native renders to a UITextField on iOS. This ensures that integration with password managers and keyboard controls work out of the box. By using platform primitives, React Native apps are also able to stay up-to-date with design and behavior changes from new releases of Android and iOS.',
  });
};

const shareText = () => {
  Share.share(
    {
      title: 'Massive Scale',
      message:
        'Hundreds of screens in the Facebook app are implemented with React Native. The Facebook app is used by billions of people on a huge range of devices. This is why we invest in the most challenging problems at scale.' +
        'Deploying React Native in our apps lets us identify problems that we wouldn’t see at a smaller scale. For example, Facebook focuses on improving performance across a broad spectrum of devices from the newest iPhone to many older generations of Android devices. This focus informs our architecture projects such as Hermes, Fabric, and TurboModules.',
      url: 'https://reactnative.dev/blog/2020/07/17/react-native-principles',
    },
    {
      subject: 'MUST READ: Massive Scale',
      dialogTitle: 'Share React Native Blog',
      excludedActivityTypes: ['com.apple.UIKit.activity.PostToTwitter'],
      tintColor: 'blue',
    },
  );
};

const ShareMessageWithoutTitleTest = () => {
  return (
    <View style={styles.container}>
      <Text style={styles.title}>Native Experience</Text>
      <Text>
        Our top priority for React Native is to match the expectations people
        have for each platform. This is why React Native renders to platform
        primitives. We value native look-and-feel over cross-platform
        consistency. For example, the TextInput in React Native renders to a
        UITextField on iOS. This ensures that integration with password managers
        and keyboard controls work out of the box. By using platform primitives,
        React Native apps are also able to stay up-to-date with design and
        behavior changes from new releases of Android and iOS.
      </Text>
      <Button title="SHARE" onPress={shareMessage} />
    </View>
  );
};

const ShareMessageWithTitleTest = () => {
  return (
    <View style={styles.container}>
      <Text style={styles.title}>Massive Scale</Text>
      <Text>
        Hundreds of screens in the Facebook app are implemented with React
        Native. The Facebook app is used by billions of people on a huge range
        of devices. This is why we invest in the most challenging problems at
        scale. Deploying React Native in our apps lets us identify problems that
        we wouldn’t see at a smaller scale. For example, Facebook focuses on
        improving performance across a broad spectrum of devices from the newest
        iPhone to many older generations of Android devices. This focus informs
        our architecture projects such as Hermes, Fabric, and TurboModules.
      </Text>
      <Button title="SHARE" onPress={shareText} />
    </View>
  );
};

const SharedActionTest = () => {
  const [shared, setShared] = useState<string | null>();

  const sharedAction = async () => {
    try {
      const result = await Share.share(
        {
          title: 'Create native apps',
          message:
            'React Native combines the best parts of native development with React, a best-in-class JavaScript library for building user interfaces.',
          url: 'https://reactnative.dev/',
        },
        {
          subject: 'MUST READ: Create native apps with React Native',
          dialogTitle: 'Share React Native Home Page',
          tintColor: 'blue',
        },
      );
      if (result.action === Share.sharedAction) {
        setShared(result.action);
      } else if (result.action === Share.dismissedAction) {
        //iOS only, if dialog was dismissed
        setShared(null);
      }
    } catch (e) {
      console.error(e);
    }
  };
  return (
    <View style={styles.container}>
      <Text>action: {shared ? shared : 'null'}</Text>
      <Text style={styles.title}>Create native apps</Text>
      <Text>
        React Native combines the best parts of native development with React, a
        {/* best-in-class JavaScript library for building user interfaces. */}
      </Text>
      <Button title="SHARE" onPress={sharedAction} />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  title: {
    fontSize: 30,
    margin: 10,
    textAlign: 'center',
  },
});
