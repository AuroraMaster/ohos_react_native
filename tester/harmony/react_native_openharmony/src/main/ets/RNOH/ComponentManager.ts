/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Tag } from "./DescriptorBase";

/**
 * @api
 * Component(Instance)Manager. Unlike (ComponentInstance)Descriptor, exposes behavior and encapsulates data.
 */
export abstract class ComponentManager {
  onDestroy() {}
  abstract getParentTag(): Tag
  abstract getTag(): Tag
}
