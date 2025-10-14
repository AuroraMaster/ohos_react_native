/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

const path = require('path');
const { mergeConfig, getDefaultConfig } = require('@react-native/metro-config');
const { createHarmonyMetroConfig } = require('@react-native-oh/react-native-harmony/metro.config');
const moduleId = require('./build/moduleId');

const projectRootPath = path.join(__dirname);

const config = {
  serializer: {
    createModuleIdFactory: moduleId.createModuleIdFactoryWrap(
      projectRootPath,
      'basic',
    ),
  },
};

module.exports = mergeConfig(getDefaultConfig(__dirname), createHarmonyMetroConfig({
  reactNativeHarmonyPackageName: '@react-native-oh/react-native-harmony',
}), config);
