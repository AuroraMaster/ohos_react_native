/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

const { mergeConfig, getDefaultConfig } = require('@react-native/metro-config');
const { createHarmonyMetroConfig } = require('@react-native-oh/react-native-harmony/metro.config');
const path = require('path');
const projectRootPath = path.join(__dirname);
const moduleId = require('./build/multibundle/moduleId');

const config = {
    serializer: {
        createModuleIdFactory: moduleId.createModuleIdFactoryWrap(
            projectRootPath,
            'homepage',
        ),
        processModuleFilter: moduleId.postProcessModulesFilterWrap(
            projectRootPath,
        ),
    },
};

module.exports = mergeConfig(getDefaultConfig(__dirname), createHarmonyMetroConfig({
    reactNativeHarmonyPackageName: '@react-native-oh/react-native-harmony',
}), config);