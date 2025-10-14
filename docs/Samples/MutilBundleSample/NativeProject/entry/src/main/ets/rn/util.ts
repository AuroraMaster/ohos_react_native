/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export class ObjectUtil {
  static Assign<T extends {}, U>(target: T, source: U): T & U {
    return Object.assign({}, target, source);
  }
}
