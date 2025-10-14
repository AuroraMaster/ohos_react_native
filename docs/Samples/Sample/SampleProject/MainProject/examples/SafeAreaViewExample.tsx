/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { useState } from 'react';
import { SafeAreaView, StatusBar, View } from 'react-native';
import { Button } from '../components';

export function SafeAreaViewExample() {
  const [isStatusBarHidden, setIsStatusBarHidden] = useState(false);

  return (
    <>
      <StatusBar hidden={isStatusBarHidden} />
      <SafeAreaView style={{ backgroundColor: 'red', flex: 1 }}>
        <View style={{ backgroundColor: 'green', flex: 1 }}>
          <Button
            onPress={() => {
              setIsStatusBarHidden(prev => !prev);
            }}
            label="Toogle Status Bar"
          />
        </View>
      </SafeAreaView>
    </>
  );
}
