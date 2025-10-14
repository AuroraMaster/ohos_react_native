/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNAbility } from '@rnoh/react-native-openharmony';
import window from '@ohos.window';

export default class EntryAbility extends RNAbility {
  getPagePath() {
    return 'pages/Index';
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    super.onWindowStageCreate(windowStage);
    AppStorage.setOrCreate('RNAbility', this);
  }
}
