/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

try {
  const { config: harmonyConfig } = require('@rnoh/react-native-harmony-cli');
} catch (err) {
  console.warn(
    '[WARNING] react-native-harmony failed to load CLI Config from @rnoh/react-native-harmony-cli. Error message: ',
    err?.message
  );
  throw err;
}

/**
 * @type {import("@react-native-community/cli-types").Config}
 */
const config = {
  commands: harmonyConfig.commands,
};

module.exports = config;
