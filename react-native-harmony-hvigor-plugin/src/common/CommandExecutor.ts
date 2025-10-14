/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import {
  execSync,
  ExecSyncOptionsWithStringEncoding,
} from "node:child_process";

export class CommandExecutor {
  public run(
    command: string,
    args?: Record<string, string | number | boolean | undefined>,
    execOptions: ExecSyncOptionsWithStringEncoding = {
      encoding: "utf-8",
    }
  ): string {
    let commandWithArgs = command;
    if (args) {
      commandWithArgs += " " + this.stringifyCliArgs(args);
    }
    return execSync(commandWithArgs, execOptions);
  }

  protected stringifyCliArgs(
    args: Record<string, string | number | boolean | undefined>
  ): string {
    return Object.entries(args)
      .filter(([_, value]) => {
        if (typeof value === "boolean") {
          return value;
        }
        return value !== undefined && value !== null;
      })
      .map(([key, value]) => {
        if (value === undefined) return "";

        const formattedKey = this.toKebabCase(key);

        if (typeof value === "boolean") {
          return `--${formattedKey}`;
        }

        return `--${formattedKey} ${value}`;
      })
      .join(" ");
  }

  protected toKebabCase(str: string): string {
    return str
      .replace(/([a-z])([A-Z])/g, "$1-$2")
      .replace(/[\s_]+/g, "-")
      .toLowerCase();
  }
}
