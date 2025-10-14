/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { BoundingBox, Point } from '../../RNOH/ts';
import { RNViewManager } from './RNViewManager'

export class RNModalHostViewManager extends RNViewManager {
  public isPointInBoundingBox(point: Point): boolean {
    return this.isPointInView(point)
  }

  public getBoundingBox(): BoundingBox {
    // we don't want these views to be taken into account
    // when calculating their parent's bounding box.
    // Return an empty bounding box.
    return {
      left: 0,
      right: 0,
      top: 0,
      bottom: 0,
    }
  }

  public updateBoundingBox(): void {
    return;
  }

  public isHandlingTouches() {
    return true;
  }
}