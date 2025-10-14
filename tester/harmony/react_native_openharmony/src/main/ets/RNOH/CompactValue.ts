/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { DimensionValue } from "./RNTypes"

/**
 * @api
 */
export class CompactValue {
  constructor(private rawValue: DimensionValue, private parentLength: number) {
  }

  public asNumber(): number {
    if (typeof this.rawValue === 'number') {
      return this.rawValue;
    }
    else if (typeof this.rawValue === 'string' && this.rawValue !== 'auto') {
      return Number.parseFloat(this.rawValue) * (this.parentLength) / 100;
    }
    return 0;
  }
}
