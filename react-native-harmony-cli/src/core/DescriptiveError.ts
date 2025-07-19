/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export class DescriptiveError extends Error {
  constructor(
    private data:
      | {
          whatHappened: string;
          whatCanUserDo?: string[];
          extraData?: any;
        }
      | {
          whatHappened: string;
          unexpected: true;
          extraData?: any;
        }
  ) {
    super(data.whatHappened);
  }

  isUnexpected(): boolean {
    return 'unexpected' in this.data;
  }

  getMessage(): string {
    return this.data.whatHappened;
  }

  getSuggestions() {
    if ('whatCanUserDo' in this.data) {
      return this.data.whatCanUserDo ?? [];
    } else {
      return [];
    }
  }

  getRawData() {
    return this.data;
  }

  getDetails(): string {
    if (!this.data.extraData) {
      return '';
    }
    if (typeof this.data.extraData === 'string') {
      return this.data.extraData.trim();
    }
    if (this.data.extraData instanceof Error) {
      let lines = [
        `${this.data.extraData.name}: ${this.data.extraData.message}`,
      ];
      for (let stackEntry of (this.data.extraData.stack ?? '').split('\n')) {
        lines.push('');
        lines.push(stackEntry);
      }
      return lines.join('\n');
    }
    try {
      return JSON.stringify(this.data.extraData, null, 2);
    } catch (err) {
      return '';
    }
  }
}
