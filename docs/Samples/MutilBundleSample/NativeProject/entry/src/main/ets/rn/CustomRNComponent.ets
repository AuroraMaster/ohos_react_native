/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { ComponentBuilderContext, } from '@rnoh/react-native-openharmony';

@Builder
export function buildCustomRNComponent(ctx: ComponentBuilderContext) {
  Stack() {
  }
  .position({ x: 0, y: 0 })
}

export const arkTsComponentNames: Array<string> = [];
