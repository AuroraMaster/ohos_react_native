/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { CommandExecutor } from "../src/common/CommandExecutor";
import { Logger } from "../src/common/Logger";
import {
  AutolinkingArgs,
  AutolinkingSubtask,
} from "../src/tasks/sync/AutolinkingSubtask";
import { CodegenSubtask } from "../src/tasks/sync/CodegenSubtask";
import { MetroPortForwardSubtaskInput } from "../src/tasks/sync/MetroPortForwardSubtask";
import { SyncTask } from "../src/tasks/sync/SyncTask";
import { CodegenConfig, Subtask } from "../src/types";

export class StubCommandExecutor extends CommandExecutor {
  private commands: string[] = [];

  constructor(private onRun: (command: string) => string) {
    super();
  }

  run(
    command: string,
    args?: Record<string, string | number | boolean>
  ): string {
    let commandWithArgs = command;
    if (args) {
      commandWithArgs += " " + this.stringifyCliArgs(args);
    }
    this.commands.push(commandWithArgs);
    return this.onRun(commandWithArgs);
  }

  getCommands() {
    return this.commands;
  }
}

export class FakeLogger implements Logger {
  private logs: { type: "info" | "warn" | "error"; msg: string }[] = [];

  info(message: string) {
    this.logs.push({ type: "info", msg: message });
  }

  warn(message: string): void {
    this.logs.push({ type: "warn", msg: message });
  }

  error(message: string): void {
    this.logs.push({ type: "error", msg: message });
  }

  getLogs() {
    return this.logs;
  }
}

export class FakeSyncTask extends SyncTask {
  protected runSubtasks(input: {
    nodeModulesPath: string;
    metro: MetroPortForwardSubtaskInput;
    codegenArgs: CodegenConfig | null;
    autolinkingArgs: AutolinkingArgs | null;
  }) {
    (
      [
        new FakeMetroPortForwardSubtask(
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

class FakeMetroPortForwardSubtask implements Subtask {
  constructor(
    private commandExecutor: CommandExecutor,
    private logger: Logger,
    private input: MetroPortForwardSubtaskInput
  ) {}

  run() {
    if (this.input === null) {
      this.logger.warn("[metro] skipped port forwarding");
      return;
    }
    const result = this.commandExecutor.run(
      `"/test/path" rport tcp:${this.input.port} tcp:${this.input.port}`
    );
    this.logger.info(`[metro] ${result}`);
  }
}
