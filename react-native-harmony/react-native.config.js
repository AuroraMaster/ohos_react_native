/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

const { config: harmonyConfig } = require('@rnoh/react-native-harmony-cli')

/**
 * @type {import("@react-native-community/cli-types").Config}
 */
const config = {
  commands: harmonyConfig.commands,
}

module.exports = config
