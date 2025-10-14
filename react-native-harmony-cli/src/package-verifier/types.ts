/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export type CheckStatus = 'pass' | 'fail' | 'skip';

export type Check = {
  checkIf: string;
  run: () => Promise<CheckStatus | { status: CheckStatus; message: string }>;
};

export type CheckResult = {
  name: string;
  status: CheckStatus;
  message: string;
};
