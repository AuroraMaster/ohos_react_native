/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import fs from 'fs';
import pathUtils from 'path';

export type FileStructure = { [key: string]: string | FileStructure };



export function copyMetroConfig(rootPath: string, filePath: string, nodeModulesPath: string) {
  const localPath = pathUtils.join(rootPath, "metro.config.js");
  
  const fileContent = fs.readFileSync(filePath, 'utf-8');

  const testParentDir = pathUtils.dirname(__dirname);
  const absoluteColorsPath = pathUtils.join(testParentDir, nodeModulesPath, "colors", "safe");
  const colorsLine =  "const colors = require('colors/safe');";
  const updatedFileContent = fileContent.replace(colorsLine, `const colors = require("${absoluteColorsPath}")`);

  fs.writeFileSync(localPath, updatedFileContent);
  
  return localPath;
}

export function createFileStructure(
  rootPath: string,
  fileStructure: FileStructure
) {
  for (const [name, content] of Object.entries(fileStructure)) {
    if (typeof content === 'string') {
      fs.writeFileSync(pathUtils.join(rootPath, name), content);
    } else {
      fs.mkdirSync(pathUtils.join(rootPath, name));
      createFileStructure(pathUtils.join(rootPath, name), content);
    }
  }
}