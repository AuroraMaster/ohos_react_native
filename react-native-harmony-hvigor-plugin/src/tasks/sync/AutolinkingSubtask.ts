/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import { Logger } from "../../common/Logger";
import { CommandExecutor } from "../../common/CommandExecutor";
import * as pathUtils from "node:path";

export type AutolinkingArgs = {
  harmonyProjectPath: string;
  nodeModulesPath: string;
  cmakeAutolinkPathRelativeToHarmony: string;
  cppRnohPackagesFactoryPathRelativeToHarmony: string;
  etsRnohPackagesFactoryPathRelativeToHarmony: string;
  ohPackagePathRelativeToHarmony: string;
  excludeNpmPackages: string | undefined;
  includeNpmPackages: string | undefined;
};

export type AutolinkingSubtaskInput = AutolinkingArgs | null;

export class AutolinkingSubtask {
  constructor(
    private commandExecutor: CommandExecutor,
    private logger: Logger,
    private input: AutolinkingSubtaskInput
  ) {}

  run(): void {
    if (this.input === null) {
      this.logger.warn(`[autolink] skipped`);
      return;
    }
    const result = this.commandExecutor.run(
      `node_modules${pathUtils.sep}.bin${pathUtils.sep}react-native link-harmony`,
      {
        ...this.input,
        nodeModulesPath: 'node_modules'
      },
      {
        encoding: "utf-8",
        cwd: pathUtils.join(this.input.nodeModulesPath, "..")
      }
    );
    this.logger.info(`[autolink]\n${result}`);
  }
}
