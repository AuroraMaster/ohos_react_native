/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNAbility, RNInstanceError, RNOHCoreContext } from '@rnoh/react-native-openharmony';
import { Want } from '@kit.AbilityKit';

export default class EntryAbility extends RNAbility {
  getPagePath() {
    return 'pages/Index';
  }

  override onCreate(want: Want): void {
    super.onCreate(want);
    AppStorage.get<RNOHCoreContext>('RNOHCoreContext')!.subscribeToRNOHErrors((err) => {
      console.log('HERE: error logged from entryability! ' + JSON.stringify(err))
      console.log('HERE: err.getMessage: ' + err.getMessage())
      console.log('HERE: err.getSuggestions:' + JSON.stringify(err.getSuggestions()), null, 2)
      console.log('HERE: err.getExtraData: ' + JSON.stringify(err.getExtraData()), null, 2)
      console.log('HERE: err.getStack: ' + JSON.stringify(err.getStack(), null, 2))
      //Not all errors are RNInstanceError's, so we need to check for that.
      if (err instanceof RNInstanceError) {
        console.log('HERE: err.getRNInstanceId: ' + err.getRNInstanceId())
        console.log('HERE: err.getRNInstanceName: ' + err.getRNInstanceName())
      }
    })
  }

}