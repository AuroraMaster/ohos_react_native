/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { StackFrame } from '../RNOHCorePackage/turboModules';
import { EventEmitter } from './EventEmitter'
import { RNInstance } from './RNInstance'

export interface ErrorData {
  whatHappened: string,
  howCanItBeFixed: string[]
  extraData?: any
  customStack?: RNOHErrorStack | string
}

export class RNOHError extends Error {
  protected data: undefined | ErrorData;
  protected rnohError: undefined | RNOHError;

  static fromMultipleRNOHErrors(rnohErrors: RNOHError[]) {
    const message = rnohErrors.map((err, idx) => {
      return `${idx + 1}) ${err.getMessage()}`
    }).join("\n")
    const suggestions = rnohErrors.flatMap(err => err.getSuggestions())
    const details =
      rnohErrors.map(err => err.getDetails()).filter(d =>!!d).map((d, idx) => `${idx + 1}) ${d.trim()}`).join("\n\n")
    return new RNOHError({ whatHappened: message, howCanItBeFixed: suggestions, extraData: details })
  }


  constructor(data: ErrorData | RNOHError) {
    super(data instanceof RNOHError ? data.getMessage() : data.whatHappened)
    if (data instanceof RNOHError) {
      this.rnohError = data;
    } else {
      this.data = data;
    }
  }

  getMessage(): string {
    if (this.data) {
      return this.data.whatHappened;
    } else {
      return this.rnohError.getMessage();
    }
  }

  getSuggestions(): string[] {
    if (this.data) {
      return this.data.howCanItBeFixed
    } else {
      return this.rnohError.getSuggestions();
    }
  }

  getStack(): string | RNOHErrorStack | undefined {
    if (this.data) {
      let customStack = this.data.customStack;

      if (typeof customStack === 'string') {
        return customStack;
      } else if (customStack instanceof RNOHErrorStack) {
        return customStack;
      } else {
        return this.stack;
      }
    } else {
      return this.rnohError.getStack();
    }
  }

  getExtraData(): any {
    if (this.data) {
      return this.data.extraData;
    } else {
      return this.rnohError.getExtraData();
    }
  }

  /**
   * @deprecated: use getExtraData instead.
   */

  getDetails(): string {
    if (this.data) {
      if (!this.data.extraData) {
        return ""
      }
      if (typeof this.data.extraData === "string") {
        return this.data.extraData.trim()
      }
      if (this.data.extraData instanceof Error) {
        let lines = [`${this.data.extraData.name}: ${this.data.extraData.message}`]
        for (let stackEntry of (this.data.extraData.stack ?? "").split("\n")) {
          lines.push("")
          lines.push(stackEntry)
        }
        return lines.join("\n")
      }
      try {
        return JSON.stringify(this.data.extraData, null, 2)
      } catch (err) {
        return ""
      }
    } else {
      return this.rnohError.getDetails();
    }
  }
}

export class RNInstanceError extends RNOHError {
  constructor(data: ErrorData | RNOHError, protected rnInstanceData: {
    name: string | undefined,
    id: number,
  }) {
    super(data);
  }

  getExtraData() {
    return {
      rnInstanceName: this.rnInstanceData.name,
      rnInstanceId: this.rnInstanceData.id,
      wrappedData: super.getExtraData(),
    }
  }

  getRNInstanceName() {
    return this.rnInstanceData.name;
  }

  getRNInstanceId() {
    return this.rnInstanceData.id;
  }
}

export class FatalRNOHError extends RNOHError {
}


export class WorkerRNOHError extends RNOHError {
}

export class RNOHErrorStack {
  constructor(private frames: StackFrame[]) {
  };

  getFrames() {
    return this.frames;
  }

  toString(): string {
    return this.frames.map(entry => {
      let location = ""
      let locationInFile = ""
      if (entry.lineNumber) {
        if (entry.column !== undefined) {
          locationInFile = `${entry.lineNumber}:${entry.column}`
        } else {
          locationInFile = `${entry.lineNumber}`
        }
      }
      if (entry.file) {
        if (locationInFile) {
          location = `${entry.file}:${locationInFile}`
        } else {
          location = `${entry.file}`
        }
      } else {
        location = locationInFile
      }
      if (location) {
        return `${entry.methodName} (${location})`
      } else {
        return `${entry.methodName}`
      }
    }).join("\n")
  }
}

export type RNOHErrorEventEmitter = EventEmitter<{
  'NEW_ERROR': [RNOHError];
}>;

export type RNInstanceErrorEventEmitter = EventEmitter<{
  'NEW_ERROR': [RNInstanceError];
}>;