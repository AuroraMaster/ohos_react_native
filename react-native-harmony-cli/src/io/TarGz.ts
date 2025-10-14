/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import fs from 'node:fs';
import zlib from 'zlib';
import tar from 'tar';
import { AbsolutePath } from '../core';

export function unpackTarGz(
  archivePath: AbsolutePath,
  outputDirPath: AbsolutePath
) {
  if (!fs.existsSync(outputDirPath.getValue())) {
    fs.mkdirSync(outputDirPath.getValue(), { recursive: true });
  }
  const readStream = fs.createReadStream(archivePath.getValue());
  const writeStream = tar.extract({
    cwd: outputDirPath.getValue(),
  });
  readStream.pipe(zlib.createGunzip()).pipe(writeStream);
  return new Promise((resolve, reject) => {
    writeStream.on('finish', () => {
      resolve(undefined);
    });
    writeStream.on('error', (err) => {
      reject(err);
    });
  });
}
