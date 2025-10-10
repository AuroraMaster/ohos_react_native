/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
import fs from "node:fs";
import * as pathUtils from "node:path";
import JSON5 from 'json5';
import { Logger } from "../../common/Logger";

export class CheckHvigorVersionSubtask {
  constructor(
    private logger: Logger,
  ) {}

  run(): void {
    const regx = /rnoh-hvigor-plugin-(\d+\.\d+\.\d+)\.tgz/;
    const configFilePath = './hvigor/hvigor-config.json5';
    const configFileContent = fs.readFileSync(configFilePath, 'utf8');
    const hvigorConfig = JSON5.parse(configFileContent);
    const rnohPluginPath = hvigorConfig.dependencies['@rnoh/hvigor-plugin']?.replace('file:', '');
    if (!rnohPluginPath) {
      return;
    }
    const oldVersion = rnohPluginPath.match(regx)?.[1];
    if (!oldVersion) {
      this.logger.info('[@rnoh/hvigor-plugin] skip');
      return;
    }
    const files = fs.readdirSync(pathUtils.join('./hvigor', rnohPluginPath, '..'));
    const file = files.find(f => regx.test(f));
    if (!file) {
      this.logger.info('[@rnoh/hvigor-plugin] skip');
      return;
    }
    const newVersion = file.match(regx)?.[1];
    if (oldVersion !== newVersion) {
      const updatedContent = configFileContent.replace(/rnoh-hvigor-plugin-(\d+\.\d+\.\d+)\.tgz/g, file);
      fs.writeFileSync(configFilePath, updatedContent, 'utf8');
      this.logger.info(`[@rnoh/hvigor-plugin]\n• ​​new version (${newVersion}) has been detected. You need to sync again to install the latest version.​\n`);
    }
  }
}
