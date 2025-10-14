/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { ReactNativeFixture } from './ReactNativeFixture';

it('list verify-package-harmony in the help description', async () => {
  const result = new ReactNativeFixture('.').help();

  expect(result.includes('verify-package-harmony')).toBeTruthy();
});
