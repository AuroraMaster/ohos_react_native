/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import { Logger } from "../../common/Logger";
import { globSync } from "glob";
import { CommandExecutor } from "../../common/CommandExecutor";
import { Subtask } from "../../types";
import { ValidationError } from "./SyncTask";
import * as pathUtils from "node:path";
import os from "os";

export type MetroPortForwardSubtaskInput = { port: any } | null;

export class MetroPortForwardSubtask implements Subtask {
  constructor(
    private commandExecutor: CommandExecutor,
    private logger: Logger,
    private input: MetroPortForwardSubtaskInput
  ) {}

  run(): void {
    if (this.input === null) {
      this.logger.warn("[metro] skipped port forwarding");
      return;
    }
    const hdcPath = this.getHdcPath();
    const result = this.commandExecutor.run(
      `"${hdcPath}" rport tcp:${this.input.port} tcp:${this.input.port}`
    );
    this.logger.info(`[metro] ${result}`);
  }

  private getHdcPath() {
    const DEVECO_SDK_HOME = process.env.DEVECO_SDK_HOME;
    if (!DEVECO_SDK_HOME) {
      throw new ValidationError(
        "metro",
        "DEVECO_SDK_HOME must be specified if metro configuration is not null."
      );
    }

    const basePath = pathUtils.join(DEVECO_SDK_HOME, "..");
    const hdcBinary = os.platform() === "win32" ? "hdc.exe" : "hdc";
    const files = globSync(`${basePath}/**/${hdcBinary}`, { nodir: true });
    if (files.length === 0) {
      throw new Error("hdc not found in DEVECO_SDK_HOME");
    }

    return files[0];
  }
}
