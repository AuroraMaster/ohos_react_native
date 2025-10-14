/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AbsolutePath } from '../core';
import { spawn } from 'node:child_process';

export type CliOptions = {
  args: string[];
  cwd?: AbsolutePath;
  onArgsStringified?: (commandWithArgs: string) => void;
  onStdout?: (data: string) => void;
  onStderr?: (data: string) => void;
  shell?: boolean;
};

export abstract class CliExecutor {
  abstract run(command: string, options?: CliOptions): Promise<string>;
}

export class RealCliExecutor extends CliExecutor {
  async run(command: string, options?: CliOptions): Promise<string> {
    return new Promise((resolve, reject) => {
      let commandWithArgs = command;
      let argStrings: string[] = [];
      if (options?.args) {
        argStrings = options.args;
        commandWithArgs += ' ' + argStrings.join(' ');
      }
      options?.onArgsStringified?.(commandWithArgs);
      const process = spawn(command, argStrings, {
        cwd: options?.cwd?.toString(),
        shell: options?.shell ? true : false,
      });

      let stdout = '';
      let stderr = '';

      process.stdout.on('data', (data) => {
        const output = data.toString();
        stdout += output;
        options?.onStdout?.(output);
      });

      process.stderr.on('data', (data) => {
        const output = data.toString();
        stderr += output;
        options?.onStderr?.(output);
      });

      process.on('error', (error) => {
        reject(error);
      });

      process.on('close', (code) => {
        if (code === 0) {
          resolve(stdout);
        } else {
          reject(
            new Error(`Command failed with code ${code}\nStderr: ${stderr}`)
          );
        }
      });
    });
  }
}