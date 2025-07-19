/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import chalk from 'chalk';
import { DescriptiveError } from '../core';
import { getLoader, logger as LOGGER } from '@react-native-community/cli-tools';
import process from 'node:process';
export class Logger {
  private loader = getLoader({
    interval: process.stdout.isTTY ? undefined : 0,
  });
  private logger = LOGGER;
  private isLoaderActive = false;

  start(prepareMsg: (styles: typeof chalk) => string) {
    this.loader.start(prepareMsg(chalk));
    this.isLoaderActive = true;
    return (options?: { persist?: boolean }) => {
      this.isLoaderActive = false;
      if (options?.persist) {
        console.log({ loader: this.loader });
        this.loader.stopAndPersist();
      } else {
        this.loader.stop();
      }
    };
  }

  info(prepareMsg: (styles: typeof chalk) => string) {
    const info = (msg: string) => {
      this.isLoaderActive ? this.loader.info(msg) : this.logger.info(msg);
    };
    info(prepareMsg(chalk));
  }

  succeed(prepareMsg: (styles: typeof chalk) => string) {
    const succeed = (msg: string) =>
      this.isLoaderActive ? this.loader.succeed(msg) : this.logger.success(msg);
    succeed(prepareMsg(chalk));
  }

  warn(prepareMsg: (styles: typeof chalk) => string) {
    const warn = (msg: string) =>
      this.isLoaderActive ? this.loader.warn(msg) : this.logger.warn(msg);
    warn(prepareMsg(chalk));
  }

  debug(prepareMsg: (styles: typeof chalk) => string) {
    if (this.isLoaderActive) {
      clearLine(process.stdout);
      moveCursor(process.stdout, 0);
      console.debug(prepareMsg(chalk));
      this.loader.render();
    } else {
      console.debug(prepareMsg(chalk));
    }
  }

  descriptiveError(error: DescriptiveError) {
    const lines: string[] = [];
    if (error.isUnexpected() && error.stack) {
      lines.push('');
      lines.push(chalk.bold('Stack'));
      error.stack.split('\n').forEach((line) => lines.push(chalk.gray(line)));
    }
    const details = error.getDetails();
    if (details) {
      lines.push('');
      lines.push(chalk.bold('Details'));
      details.split('\n').forEach((line) => lines.push(line));
    }
    lines.push('');
    lines.push(`${error.getMessage()}`);
    const suggestions = error.getSuggestions();

    if (suggestions.length > 0) {
      lines.push('');
      lines.push(chalk.bold(`Suggestions`));
      suggestions.forEach((suggestion) => {
        lines.push('• ' + suggestion);
      });
    }

    if (this.isLoaderActive) {
      this.loader.fail(lines.join('\n').trim());
    } else {
      this.logger.error(lines.join('\n').trim());
    }
  }
}

const clearLine = (stream: NodeJS.WriteStream): void => {
  if (stream.clearLine) {
    stream.clearLine(0);
  } else {
    process.stdout.write(
      '\r' + ' '.repeat(process.stdout.columns || 80) + '\r'
    );
  }
};

const moveCursor = (stream: NodeJS.WriteStream, x: number): void => {
  if (stream.cursorTo) {
    stream.cursorTo(x);
  } else {
    process.stdout.write('\r');
  }
};
