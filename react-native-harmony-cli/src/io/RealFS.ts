/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AbsolutePath } from '../core';
import { Dirent, FS } from '../core/FileSystem';
import fs, { Dirent as NodeFSDirent } from 'node:fs';

class RealFSDirent extends Dirent {
  constructor(private nodeFSDirent: NodeFSDirent) {
    super();
  }

  isDirectory(): boolean {
    return this.nodeFSDirent.isDirectory();
  }

  get name(): string {
    return this.nodeFSDirent.name;
  }
}

export class RealFS extends FS {
  existsSync(path: AbsolutePath): boolean {
    return fs.existsSync(path.toString());
  }

  readTextSync(path: AbsolutePath): string {
    return fs.readFileSync(path.toString()).toString();
  }

  async readText(path: AbsolutePath): Promise<string> {
    return new Promise((resolve, reject) => {
      fs.readFile(path.toString(), undefined, (err, data) => {
        if (err) {
          reject(err);
          return;
        }
        resolve(data.toString());
      });
    });
  }

  writeTextSync(path: AbsolutePath, data: string): void {
    fs.writeFileSync(path.toString(), data);
  }

  readDirentsSync(
    path: AbsolutePath,
    options?: { recursive?: boolean | undefined } | undefined
  ): Dirent[] {
    return fs
      .readdirSync(path.toString(), {
        withFileTypes: true,
        recursive: options?.recursive,
      })
      .map((rawDirent) => new RealFSDirent(rawDirent));
  }
}