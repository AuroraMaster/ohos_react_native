/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

const path = require('path');
const fs = require('fs');
const MD5 = require('crypto-js/md5');
const basicModuleIds = require('./map/basicModuleIds.json');
const businessModuleIds = require('./map/businessModuleIds.json');

function getModuleId(projectRootPath, modulePath, bundleName) {
  const pathRelative = modulePath.substr(projectRootPath.length + 1);
  const moduleIds = bundleName === 'basic' ? basicModuleIds : businessModuleIds;
  const moduleId = MD5(pathRelative).toString();
  moduleIds[pathRelative] = moduleId;
  fs.writeFileSync(path.resolve(__dirname, `./map/${bundleName === 'basic' ? 'basicModuleIds' : 'businessModuleIds'}.json`), JSON.stringify(moduleIds));
  return moduleId;
}

function createModuleIdFactoryWrap(projectRootPath, bundleName) {
  return () => {
    return (path) => {
      const pathRelative = path.substr(projectRootPath.length + 1);
      const moduleId = basicModuleIds[pathRelative] || businessModuleIds[pathRelative] || getModuleId(projectRootPath, path, bundleName);
      return moduleId;
    };
  };
}

function postProcessModulesFilterWrap(projectRootPath) {
  // 返回false则不打入bundle中
  return (module) => {
    const modulePath = module.path;
    const pathRelative = modulePath.substr(projectRootPath.length + 1);
    const moduleId = basicModuleIds[pathRelative];
    if (pathRelative.indexOf('__prelude__') >= 0 ||
      pathRelative.indexOf(`node_modules${path.sep}@react-native${path.sep}polyfills`) >= 0 ||
      pathRelative.indexOf(`node_modules${path.sep}metro-runtime${path.sep}src${path.sep}polyfills`) >= 0) {
      return false;
    }
    return !moduleId
  };
}

module.exports = { createModuleIdFactoryWrap, postProcessModulesFilterWrap };
