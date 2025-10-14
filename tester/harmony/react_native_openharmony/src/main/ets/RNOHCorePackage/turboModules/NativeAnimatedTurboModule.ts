/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { Tag } from '../../RNOH/ts';
import { TurboModule } from "../../RNOH/TurboModule";

export class NativeAnimatedTurboModule extends TurboModule {
  public static readonly NAME = 'NativeAnimatedTurboModule';

  public setViewProps(tag: Tag, props: Object) {
    this.ctx.descriptorRegistry.setAnimatedRawProps(tag, props);
  }
}
