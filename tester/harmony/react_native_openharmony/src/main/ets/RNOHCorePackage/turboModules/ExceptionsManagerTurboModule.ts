/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModule, FatalRNOHError, RNOHError, RNOHErrorStack } from "../../RNOH/ts";

export type StackFrame = {
  column?: number,
  file?: string,
  lineNumber?: number,
  methodName: string,
  collapse?: boolean,
};

export type ExceptionData = {
  message: string,
  originalMessage?: string,
  name?: string,
  componentStack?: string,
  stack: StackFrame[],
  id: number,
  isFatal: boolean,
  extraData?: Object,
};

/**
 * Note: This TurboModule is used when __DEV__ flag in JS bundle is set to false.
 */
export class ExceptionsManagerTurboModule extends TurboModule {
  public static readonly NAME = 'ExceptionsManager';

  reportFatalException(message: string, stack: StackFrame[], exceptionId: number): void {
    this.ctx.reportRNOHError(new FatalRNOHError({
      whatHappened: message,
      howCanItBeFixed: [],
      customStack: new RNOHErrorStack(stack),
    }))
  }

  reportSoftException(message: string, stack: StackFrame[], exceptionId: number): void {
    this.ctx.reportRNOHError(new RNOHError({
      whatHappened: message,
      howCanItBeFixed: [],
      customStack: new RNOHErrorStack(stack)
    }))
  }

  reportException(data: ExceptionData): void {
    this.ctx.rnInstance.getInitialBundleUrl()
    if (data.isFatal) {
      this.ctx.reportRNOHError(new FatalRNOHError({
        whatHappened: data.message,
        howCanItBeFixed: [],
        customStack: new RNOHErrorStack(data.stack),
        extraData: data.extraData
      }))
    } else {
      this.ctx.reportRNOHError(new RNOHError({
        whatHappened: data.originalMessage,
        howCanItBeFixed: [],
        customStack: new RNOHErrorStack(data.stack),
        extraData: data.extraData
      }))
    }
  }


  updateExceptionMessage(message: string, stack: StackFrame[], exceptionId: number): void {
    this.ctx.logger.warn(`ExceptionsManager::updateExceptionMessage is not implemented`);
    this.ctx.logger.error(`ExceptionsManager::updateExceptionMessage ${message}`);
    stack.forEach((frame) => {
      this.ctx.logger.error(JSON.stringify(frame));
    });
  }

  dismissRedbox(): void {
    this.ctx.devToolsController.dismissRNOHErrorDialog()
  }
}
