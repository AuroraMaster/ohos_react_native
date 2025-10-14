/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { ReactNativeFixture } from './ReactNativeFixture';

it('list link-harmony in the help description', async () => {
  const result = new ReactNativeFixture('.').help();

  expect(result.includes('link-harmony')).toBeTruthy();
});