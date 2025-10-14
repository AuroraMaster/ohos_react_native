/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AbsolutePath } from '../../core';
import { SpecSchema } from './UberSchema';

type FileContent = string;
export interface CodeGenerator<T> {
  generate(data: T): Map<AbsolutePath, FileContent>;
}

export interface SpecCodeGenerator extends CodeGenerator<SpecSchema> {}
