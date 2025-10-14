/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModule, TurboModuleContext, TurboModulesFactory } from '@rnoh/react-native-openharmony/ts';
import { RNPackage } from '@rnoh/react-native-openharmony';
import { NavigationTurboModule } from './NavigationTurboModule';

class NavigationTurboModuleFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule | null {
    if (name === 'NavigationTurboModule') {
      return new NavigationTurboModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === 'NavigationTurboModule';
  }
}

export default class NavigationTurboModulePackage extends RNPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    return new NavigationTurboModuleFactory(ctx);
  }
}