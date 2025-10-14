/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

//@ts-check
const pathUtils = require('path');
const fs = require('fs');
const colors = require('colors/safe');

let shouldPrintInfoAboutRNRedirection = true;

/**
 * @param msg {string}
 */
function info(msg) {
  const infoPrefix = '[' + colors.bold(colors.cyan(`INFO`)) + ']';
  console.log(infoPrefix, msg);
}

/**
 * @param options {{reactNativeHarmonyPackageName: string} | undefined}
 * @returns {import("metro-config").InputConfigT}
 */
function createHarmonyMetroConfig(options) {
  const reactNativeHarmonyName =
    options?.reactNativeHarmonyPackageName ?? 'react-native-harmony';
  return {
    transformer: {
      assetRegistryPath: 'react-native/Libraries/Image/AssetRegistry',
      getTransformOptions: async () => ({
        transform: {
          experimentalImportSupport: false,
          inlineRequires: true,
        },
      }),
    },
    resolver: {
      blockList: [/third-party(\\\\|\\|\/)hermes/],
      resolveRequest: (ctx, moduleName, platform) => {
        if (platform === 'harmony') {
          if (shouldPrintInfoAboutRNRedirection) {
            info(
              `Redirected imports from ${colors.bold(
                colors.gray('react-native')
              )} to ${colors.bold(reactNativeHarmonyName)}`
            );
            shouldPrintInfoAboutRNRedirection = false;
          }
          if (moduleName === 'react-native') {
            return ctx.resolveRequest(ctx, reactNativeHarmonyName, platform);
          } else if (moduleName.startsWith('react-native/')) {
            return ctx.resolveRequest(ctx, moduleName, 'ios');
          } else if (
            isInternalReactNativeRelativeImport(ctx.originModulePath)
          ) {
            if (moduleName.startsWith('.')) {
              const moduleAbsPath = pathUtils.resolve(
                pathUtils.dirname(ctx.originModulePath),
                moduleName
              );
              const [_, modulePathRelativeToReactNative] = moduleAbsPath.split(
                `${pathUtils.sep}node_modules${pathUtils.sep}react-native${pathUtils.sep}`
              );
              try {
                return ctx.resolveRequest(
                  ctx,
                  `${reactNativeHarmonyName}${pathUtils.sep}${modulePathRelativeToReactNative}`,
                  'harmony'
                );
              } catch (err) {}
            }
            return ctx.resolveRequest(ctx, moduleName, 'ios');
          } else if(isHarmonyPackageInternalImport(ctx.originModulePath, moduleName)) {
            /**
             * Replace internal imports in `react-native-foo` with equivalent files from `react-native-harmony-foo` 
             * if a package has internal import redirection enabled in its package.json configuration e.g.
             *
             * react-native-harmony-foo/package.json:
             *  "harmony": {
             *     "alias": "react-native-foo"
             *     "redirectInternalImports": true,
             *  }
             */
            const alias = getPackageNameFromOriginModulePath(ctx.originModulePath);
            if (alias) {
              const harmonyPackage = getHarmonyPackageByAliasMap(".");
              const harmonyPackageName = harmonyPackage[alias]?.name;
              const redirectInternalImports = harmonyPackage[alias]?.redirectInternalImports;
              if (harmonyPackageName && !isRequestFromHarmonyPackage(ctx.originModulePath, harmonyPackageName) && redirectInternalImports) {
                const moduleAbsPath = pathUtils.resolve(
                  pathUtils.dirname(ctx.originModulePath),
                  moduleName,
                );
                const slashes = new RegExp('/', 'g');
                const [_, modulePathRelativeToOriginalPackage] = moduleAbsPath.split(
                  `${pathUtils.sep}node_modules${pathUtils.sep}${alias.replace(slashes, pathUtils.sep)}${pathUtils.sep}`,
                );
                const backslashes = new RegExp('\\\\', 'g');
                const pathToHarmonyModule = `${harmonyPackageName}/${modulePathRelativeToOriginalPackage.replace(backslashes, "/")}`;
                try {
                  return ctx.resolveRequest(
                    ctx,
                    pathToHarmonyModule,
                    'harmony',
                  );
                } catch (err) {
                }
              }
            }
          } else {
            /**
             * Replace `react-native-foo` with `react-native-harmony-foo` if a package has harmony directory and proper package.json configuration e.g.
             *
             * react-native-harmony-foo/package.json:
             *  "harmony": {
             *     "alias": "react-native-foo"
             *  }
             */
            const harmonyPackageByAlias = getHarmonyPackageByAliasMap(".");
            const alias = getPackageName(moduleName);
            if (alias) {
              const harmonyPackageName = harmonyPackageByAlias[alias]?.name;
              if (
                harmonyPackageName &&
                !isRequestFromHarmonyPackage(
                  ctx.originModulePath,
                  harmonyPackageName
                )
              ) {
                return ctx.resolveRequest(
                  ctx,
                  moduleName.replace(alias, harmonyPackageName),
                  platform
                );
              }
            }
          }
        }
        return ctx.resolveRequest(ctx, moduleName, platform);
      },
    },
  };
}

module.exports = {
  createHarmonyMetroConfig,
};

/**
 * @param moduleName {string}
 * @returns {string | null}
 */
function getPackageName(moduleName) {
  if (moduleName.startsWith('.')) return null;
  if (moduleName.startsWith('@')) {
    const segments = moduleName.split('/', 3);
    if (segments.length == 2) {
      return moduleName;
    } else if (segments.length > 2) {
      return `${segments[0]}/${segments[1]}`;
    }
    return null;
  }
  if (moduleName.includes('/')) {
    return moduleName.split('/')[0];
  } else {
    return moduleName;
  }
}

/**
 * @param originModulePath {string}
 * @returns {string}
 */
function getPackageNameFromOriginModulePath(originModulePath) {
  const nodeModulesPosition = originModulePath.search("node_modules");
  const pathRelativeToNodeModules = originModulePath.substring(nodeModulesPosition);
  const pathSegments = pathRelativeToNodeModules.split(pathUtils.sep);
  const module = pathSegments[1];
  if (module.startsWith('@')) {
    return `${pathSegments[1]}/${pathSegments[2]}`;
  }
  else {
    return pathSegments[1];
  }
}

/**
 * @param originModulePath {string}
 * @param moduleName {string}
 * @returns {boolean}
 */
function isHarmonyPackageInternalImport(originModulePath, moduleName) {
  if (moduleName.startsWith(".")) {
    const alias = getPackageNameFromOriginModulePath(originModulePath);
    const slashes = new RegExp('/', 'g');
    if (alias && originModulePath.includes(`${pathUtils.sep}node_modules${pathUtils.sep}${alias.replace(slashes, pathUtils.sep)}${pathUtils.sep}`)) {
      const harmonyPackage = getHarmonyPackageByAliasMap(".");
      const harmonyPackageName = harmonyPackage[alias]?.name;
      if (
        harmonyPackageName &&
        !isRequestFromHarmonyPackage(
          originModulePath,
          harmonyPackageName,
        )
      ) {
        return true;
      }
    }
  }
  return false;
}

/**
 * @param originModulePath {string}
 * @returns {boolean}
 */
function isInternalReactNativeRelativeImport(originModulePath) {
  return originModulePath.includes(
    `${pathUtils.sep}node_modules${pathUtils.sep}react-native${pathUtils.sep}`
  );
}

/**
 * @param originModulePath {string}
 * @param harmonyPackageName {string}
 * @returns {boolean}
 */
function isRequestFromHarmonyPackage(originModulePath, harmonyPackageName) {
  const slashes = new RegExp('/', 'g');
  const packagePath = harmonyPackageName.replace(slashes, pathUtils.sep);
  return originModulePath.includes(
    `${pathUtils.sep}node_modules${pathUtils.sep}${packagePath}${pathUtils.sep}`
  );
}

/**
 * @type {Record<string, {name: string, redirectInternalImports: boolean}> | undefined}
 */
let cachedHarmonyPackageByAliasMap = undefined;

/**
 * @param projectRootPath {string}
 */
function getHarmonyPackageByAliasMap(projectRootPath) {
  /**
   * @type {Record<string, {name: string, redirectInternalImports: boolean}>}
   */
  const initialAcc = {};
  if (cachedHarmonyPackageByAliasMap) {
    return cachedHarmonyPackageByAliasMap;
  }
  cachedHarmonyPackageByAliasMap = findHarmonyNodeModulePaths(
    findHarmonyNodeModuleSearchPaths(projectRootPath),
  ).reduce((acc, harmonyNodeModulePath) => {
    const harmonyNodeModulePathSegments = harmonyNodeModulePath.split(
      pathUtils.sep,
    );
    let harmonyNodeModuleName =
      harmonyNodeModulePathSegments[harmonyNodeModulePathSegments.length - 1];
    if (harmonyNodeModulePathSegments.length > 1) {
      const harmonyNodeModuleParentDirName =
        harmonyNodeModulePathSegments[harmonyNodeModulePathSegments.length - 2];
      if (harmonyNodeModuleParentDirName.startsWith('@')) {
        harmonyNodeModuleName = `${harmonyNodeModuleParentDirName}/${harmonyNodeModuleName}`;
      }
    }
    const packageJSONPath = `${harmonyNodeModulePath}${pathUtils.sep}package.json`;
    const packageJSON = readHarmonyModulePackageJSON(packageJSONPath);
    const alias = packageJSON.harmony?.alias;
    const redirectInternalImports = packageJSON?.harmony?.redirectInternalImports ?? false;
    if (alias) {
      acc[alias] ={
        name: harmonyNodeModuleName,
        redirectInternalImports: redirectInternalImports
      }
    }
    return acc;
  }, initialAcc);
  const harmonyPackagesCount = Object.keys(
    cachedHarmonyPackageByAliasMap,
  ).length;
  if (harmonyPackagesCount > 0) {
    const prettyHarmonyPackagesCount = colors.bold(
      harmonyPackagesCount > 0
        ? colors.green(harmonyPackagesCount.toString())
        : harmonyPackagesCount.toString(),
    );
    info(
      `Redirected imports to ${prettyHarmonyPackagesCount} harmony-specific third-party package(s):`,
    );
    if (harmonyPackagesCount > 0) {
      Object.entries(cachedHarmonyPackageByAliasMap).forEach(
        ([original, {name: alias}]) => {
          info(
            `• ${colors.bold(colors.gray(original))} → ${colors.bold(alias)}`,
          );
        },
      );
    }
  } else {
    info('No harmony-specific third-party packages have been detected');
  }
  console.log('');
  return cachedHarmonyPackageByAliasMap;
}

/**
 * @param projectRootPath {string}
 * @returns {string[]}
 */
function findHarmonyNodeModuleSearchPaths(projectRootPath) {
  const nodeModulesPath = `${projectRootPath}${pathUtils.sep}node_modules`;
  const searchPaths = fs
    .readdirSync(nodeModulesPath)
    .filter((dirName) => dirName.startsWith('@'))
    .map((dirName) => `${nodeModulesPath}${pathUtils.sep}${dirName}`);
  searchPaths.push(nodeModulesPath);
  return searchPaths;
}

/**
 * @param searchPaths {string[]}
 * @returns {string[]}
 */
function findHarmonyNodeModulePaths(searchPaths) {
  return searchPaths
    .map((searchPath) => {
      return fs
        .readdirSync(searchPath)
        .map((dirName) => `${searchPath}${pathUtils.sep}${dirName}`)
        .filter(hasPackageJSON);
    })
    .flat();
}

/**
 * @param nodeModulePath {string}
 * @returns {boolean}
 */
function hasPackageJSON(nodeModulePath) {
  if (!fs.lstatSync(nodeModulePath).isDirectory()) return false;
  const nodeModuleContentNames = fs.readdirSync(nodeModulePath);
  return nodeModuleContentNames.includes('package.json');
}

/**
 * @param packageJSONPath {string}
 * @returns {{name: string, harmony?: {alias?: string, redirectInternalImports?: boolean}}}
 */
function readHarmonyModulePackageJSON(packageJSONPath) {
  return JSON.parse(fs.readFileSync(packageJSONPath).toString());
}
