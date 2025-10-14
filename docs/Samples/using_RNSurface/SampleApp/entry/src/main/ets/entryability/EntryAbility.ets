/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNAbility, } from '@rnoh/react-native-openharmony';
import dataPreferences from '@ohos.data.preferences';
import window from '@ohos.window';

export let preferences: dataPreferences.Preferences | null = null;

export default class EntryAbility extends RNAbility {
  getPagePath() {
    return 'pages/Index';
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    super.onWindowStageCreate(windowStage);
    try {
      let options: dataPreferences.Options = {
        name: 'test'
      };
      preferences = dataPreferences.getPreferencesSync(this.context, options);
    } catch (err) {
      console.error('Failed to get preferences');
    }
    AppStorage.setOrCreate('RNAbility', this);
  }
}
