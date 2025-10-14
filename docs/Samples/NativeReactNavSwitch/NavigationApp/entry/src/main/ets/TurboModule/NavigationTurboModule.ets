/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModule } from '@rnoh/react-native-openharmony/ts';
import { NavigationManager } from '../NavigationTool/NavigationManager';

export class NavigationTurboModule extends TurboModule {
  navigatePush(name: string) {
    console.log("NavigationTurboModule navigatePush name: " + name);
    NavigationManager.getInstance().pushDestinationByName(name);
  }

  navigatePop() {
    console.log("NavigationTurboModule navigatePop");
    NavigationManager.getInstance().popDestination();
  }
}