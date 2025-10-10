/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { HvigorPlugin } from "@ohos/hvigor";
import { SyncTask } from "./tasks/sync/SyncTask";
import fs from "node:fs";
import { Logger } from "./common/Logger";
import { CommandExecutor } from "./common/CommandExecutor";
import { RNOHModulePluginOptions } from "./types";

export function createRNOHModulePlugin(
  options: RNOHModulePluginOptions
): HvigorPlugin {
  return {
    pluginId: "rnohModulePlugin",
    apply: () => {
      const commandExecutor = new CommandExecutor();
      const logger = new Logger();
      const task = new SyncTask(commandExecutor, logger, fs);
      task.run(options);
    },
  };
}

/**
 * @deprecated Use `createRNOHModulePlugin` instead.
 */
export const createRNOHHvigorPlugin = createRNOHModulePlugin;
