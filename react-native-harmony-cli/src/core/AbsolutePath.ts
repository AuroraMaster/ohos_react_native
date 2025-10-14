/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import pathUtils from 'node:path';
import { ValueObject } from './ValueObject';

export class AbsolutePath implements ValueObject {
  static fromSegments(...segments: string[]) {
    return new AbsolutePath(pathUtils.join(...segments));
  }

  private value: string = '';

  constructor(path: string) {
    this.value = pathUtils.resolve(path);
  }

  getValue(): string {
    return this.value;
  }

  toString(): string {
    return this.value;
  }

  copyWithNewSegment(...relativePath: string[]): AbsolutePath {
    return new AbsolutePath(pathUtils.join(this.getValue(), ...relativePath));
  }

  getExtension(): string | null {
    const maybeExtWithDot = pathUtils.extname(this.value);
    if (maybeExtWithDot) {
      return maybeExtWithDot.substring(1);
    }
    return null;
  }

  getDirectoryPath(): AbsolutePath {
    const dirPath = pathUtils.extname(this.value)
      ? pathUtils.dirname(this.value)
      : this.value;
    return new AbsolutePath(dirPath);
  }

  relativeTo(absolutePath: AbsolutePath): RelativePath {
    return new RelativePath(
      pathUtils.relative(absolutePath.getValue(), this.value)
    );
  }
}

class RelativePath implements ValueObject {
  constructor(private value: string) {}

  getValue() {
    return this.value;
  }

  toString() {
    return this.value;
  }
}
