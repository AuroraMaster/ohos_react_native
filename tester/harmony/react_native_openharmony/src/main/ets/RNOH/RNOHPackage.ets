/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNPackage } from "./RNPackage"
import { ComponentBuilderContext } from "./EtsRNOHContext"

/**
 * @api
 */
export class RNOHPackage extends RNPackage {
  createWrappedCustomRNComponentBuilderByComponentNameMap(): Map<string, WrappedBuilder<[ComponentBuilderContext]>> {
    return new Map()
  }
}