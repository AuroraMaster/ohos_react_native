/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import pathUtils from "node:path";
import { Logger } from "../../common/Logger";
import {
  CodegenConfig,
  FS,
  RNOHModulePluginError,
  RNOHModulePluginOptions,
  Subtask,
} from "../../types";
import { CommandExecutor } from "../../common/CommandExecutor";
import { AutolinkingArgs, AutolinkingSubtask } from "./AutolinkingSubtask";
import { CodegenSubtask } from "./CodegenSubtask";
import {
  MetroPortForwardSubtask,
  MetroPortForwardSubtaskInput,
} from "./MetroPortForwardSubtask";
import { CheckHvigorVersionSubtask } from "./CheckHvigorVersionSubtask";

export class ValidationError extends RNOHModulePluginError {
  constructor(optionName: keyof RNOHModulePluginOptions, msg: string) {
    super(`ValidationError: ${optionName} — ${msg}`);
  }
}

export class SyncTask {
  constructor(
    protected commandExecutor: CommandExecutor,
    protected logger: Logger,
    protected fs: FS
  ) {}

  run(options: RNOHModulePluginOptions) {
    try {
      const input = this.prepareInput(options);
      this.runSubtasks(input);
    } catch (err) {
      if (err instanceof RNOHModulePluginError) {
        this.logger.error(err.message);
        throw err;
      }
      throw err;
    }
  }

  private prepareInput(options: RNOHModulePluginOptions) {
    const nodeModulesPath =
      options.nodeModulesPath ??
      pathUtils.join(process.cwd(), "../node_modules");
    if (!this.fs.existsSync(nodeModulesPath)) {
      throw new ValidationError("nodeModulesPath", "path doesn't exist");
    }
    if (options.codegen !== null &&
        !options.codegen?.rnohModulePath &&
        !options.codegen?.etsOutputPath) {
      throw new ValidationError(
        "codegen",
        "etsOutputPath must be specified if codegen is not null"
      );
    }
    const DEVECO_SDK_HOME = process.env.DEVECO_SDK_HOME;
    if (!DEVECO_SDK_HOME) {
      throw new ValidationError(
        "metro",
        "DEVECO_SDK_HOME must be specified if metro configuration is not null."
      );
    }
    // The codegen and autolinking will switch to the parent directory of node_modules to execute.
    const cwd = pathUtils.resolve(nodeModulesPath, '..');
    const harmonyProjectPathRelativeToCWD = pathUtils.relative(cwd, './');
    const projectRootPathRelativeToCWD = pathUtils.relative(
      cwd,
      options.codegen?.projectRootPath || '../'
    ) || '.';
    const cppOutputPathRelativeToCWD = pathUtils.relative(
      cwd,
      options.codegen?.cppOutputPath || './entry/src/main/cpp/generated'
    );
    const etsOutputPathRelativeToCWD = pathUtils.relative(
      cwd,
      options.codegen?.etsOutputPath || options.codegen?.rnohModulePath || './'
    );
    return {
      nodeModulesPath,
      codegenArgs:
        options.codegen === null
          ? null
          : ({
              ...(options.codegen ?? {}),
              projectRootPath: projectRootPathRelativeToCWD,
              cppOutputPath: cppOutputPathRelativeToCWD,
              etsOutputPath: etsOutputPathRelativeToCWD,
              rnohModulePath: undefined,
            } satisfies CodegenConfig),
      metro:
        options.metro === null
          ? null
          : { port: 8081, ...(options.metro ?? {}) },
      autolinkingArgs:
        options.autolinking === null
          ? null
          : ({
              harmonyProjectPath: harmonyProjectPathRelativeToCWD,
              nodeModulesPath: nodeModulesPath,
              cmakeAutolinkPathRelativeToHarmony:
                options.autolinking?.cmakeAutolinkPath ??
                "./entry/src/main/cpp/autolinking.cmake",
              cppRnohPackagesFactoryPathRelativeToHarmony:
                options.autolinking?.cppRNOHPackagesFactoryPath ??
                "./entry/src/main/cpp/RNOHPackagesFactory.h",
              etsRnohPackagesFactoryPathRelativeToHarmony:
                options.autolinking?.etsRNOHPackagesFactoryPath ??
                "./entry/src/main/ets/RNOHPackagesFactory.ets",
              ohPackagePathRelativeToHarmony:
                options.autolinking?.ohPackagePath ?? "./oh-package.json5",
              excludeNpmPackages: this.maybeWrapStringWithQuotes(
                options.autolinking?.excludeNpmPackages?.join(";") || undefined
              ),
              includeNpmPackages: this.maybeWrapStringWithQuotes(
                options.autolinking?.includeNpmPackages?.join(";") || undefined
              ),
            } satisfies AutolinkingArgs),
    };
  }

  private maybeWrapStringWithQuotes(str: string | undefined) {
    if (str === undefined || str === "") return undefined;
    return `"${str}"`;
  }

  protected runSubtasks(input: {
    nodeModulesPath: string;
    metro: MetroPortForwardSubtaskInput;
    codegenArgs: CodegenConfig | null;
    autolinkingArgs: AutolinkingArgs | null;
  }) {
    (
      [
        new CheckHvigorVersionSubtask(this.logger),
        new MetroPortForwardSubtask(
          this.commandExecutor,
          this.logger,
          input.metro
        ),
        new CodegenSubtask(
          this.commandExecutor,
          this.logger,
          input.nodeModulesPath,
          input.codegenArgs
        ),
        new AutolinkingSubtask(
          this.commandExecutor,
          this.logger,
          input.autolinkingArgs
        ),
      ] satisfies Subtask[]
    ).forEach((subtask) => subtask.run());
  }
}
