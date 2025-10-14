/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type common from '@ohos.app.ability.common';
import { RNOHContext, RNInstance, RNInstanceOptions } from '@rnoh/react-native-openharmony/ts';

export interface UPRNSDK {
  abilityContext: common.UIAbilityContext;

  createAndRegisterRNInstance(options: RNInstanceOptions): Promise<RNInstance>;

  createRNOHContext({ rnInstance }: { rnInstance: RNInstance }): RNOHContext;

  onBackPress(): boolean;
}