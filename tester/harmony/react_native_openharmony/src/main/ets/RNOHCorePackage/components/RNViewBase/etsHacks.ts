/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export function areFirstLevelPropsEqual(lhs: Object, rhs: Object): boolean {
  if (Object.keys(lhs).length !== Object.keys(rhs).length) {
    return false
  }
  for (const key of Object.keys(lhs)) {
    if (lhs[key] !== rhs[key]) {
      return false
    }
  }
  return true
}

export function areFirstLevelElementsEqual<T>(lhs: Array<T>, rhs: Array<T>): boolean {
  if (lhs.length !== rhs.length) {
    return false
  }
  for (let i = 0; i < lhs.length; i++) {
    if (lhs[i] !== rhs[i]) {
      return false
    }
  }
  return true
}