/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Command } from './types';
import { Logger, RealFS } from '../io';

import pathUtils from 'path';
import fs from 'fs';
import zlib from 'zlib';
import tar from 'tar';
import JSON5 from 'json5';
import {
  ValidationError,
  AbsolutePath,
  ProjectDependenciesManager,
} from '../core';

export const commandUnpackHarmony: Command = {
  name: 'unpack-harmony',
  description: 'Unpacks native code.',
  options: [
    {
      name: '--project-root-path <path>',
      description: 'Relative path to package root',
      default: '.',
    },
    {
      name: '--output-dir <string>',
      description: 'Output directory to which OH modules should be unpacked to',
      default: './harmony/rnoh_modules',
    },
  ],
  func: async (argv, config, args: any) => {
    try {
      await unpack(
        new AbsolutePath(args.projectRootPath),
        new AbsolutePath(args.outputDir)
      );
    } catch (err) {
      if (err instanceof ValidationError) {
        console.error('[ERROR]', err.message);
        process.exit(1);
      } else {
        throw err;
      }
    }
  },
};

async function unpack(
  projectRootPath: AbsolutePath,
  outputDirPath: AbsolutePath
) {
  const logger = new Logger();
  const fs = new RealFS();
  const projectDependenciesManager = new ProjectDependenciesManager(
    fs,
    projectRootPath
  );
  logger.info((styles) => styles.bold(`Unpacking RNOH modules`));
  let unpackedArchivesCount = 0;
  let skippedArchivesCount = 0;
  await projectDependenciesManager.forEachAsync(async (dependency) => {
    const packageJSON = dependency.readPackageJSON();
    const harmonyArchivePath = dependency
      .getRootPath()
      .copyWithNewSegment('harmony.tar.gz');
    const pkgHarmonyDirPath = dependency
      .getRootPath()
      .copyWithNewSegment('harmony');
    const harmonyTarGzPathStr = dependency
      .getRootPath()
      .copyWithNewSegment('harmony.tar.gz')
      .relativeTo(projectRootPath)
      .getValue();
    if (
      packageJSON.version &&
      shouldUnpack(
        harmonyArchivePath,
        pkgHarmonyDirPath,
        outputDirPath,
        packageJSON.version
      )
    ) {
      if (!fs.existsSync(pkgHarmonyDirPath)) {
        /**
         * After fresh install, harmony.tar.gz wasn't unpacked to harmony. Updating npm package should remove unpacked harmony dir.
         */
        logger.info(
          (styles) =>
            `[${styles.cyan('unpacking')}] ${styles.gray(harmonyTarGzPathStr)}`
        );
        await unpackTarGz(harmonyArchivePath, pkgHarmonyDirPath);
        unpackedArchivesCount++;
      } else {
        skippedArchivesCount++;
        logger.info(
          (styles) =>
            `[${styles.yellow('skipped')}]   ${styles.gray(
              harmonyTarGzPathStr
            )}`
        );
      }
      const ohModulesInfo = getOhModulesInfo(pkgHarmonyDirPath, outputDirPath);
      for (const ohModuleInfo of ohModulesInfo) {
        const moduleArchivePathStr = dependency
          .getRootPath()
          .copyWithNewSegment('harmony', ohModuleInfo.archiveFileName)
          .relativeTo(projectRootPath)
          .getValue();
        logger.info(
          (styles) =>
            `[${styles.cyan('unpacking')}] ${styles.gray(moduleArchivePathStr)}`
        );
        unpackedArchivesCount++;
        await unpackTarGz(
          ohModuleInfo.archivePath,
          ohModuleInfo.computedModulePath
        );
      }
    } else {
      if (fs.existsSync(harmonyArchivePath)) {
        skippedArchivesCount++;
        logger.info(
          (styles) =>
            `[${styles.yellow('skipped')}]   ${styles.gray(
              harmonyTarGzPathStr
            )}`
        );
      }
      const ohModulesInfo = getOhModulesInfo(pkgHarmonyDirPath, outputDirPath);
      for (const ohModuleInfo of ohModulesInfo) {
        const moduleArchivePathStr = dependency
          .getRootPath()
          .copyWithNewSegment('harmony', ohModuleInfo.archiveFileName)
          .relativeTo(projectRootPath)
          .getValue();
        skippedArchivesCount++;
        logger.info(
          (styles) =>
            `[${styles.yellow('skipped')}]   ${styles.gray(
              moduleArchivePathStr
            )}`
        );
      }
    }
  });
  logger.info(
    (styles) =>
      `\nUnpacked: ${styles.cyan(
        unpackedArchivesCount
      )}, Skipped: ${styles.yellow(skippedArchivesCount)}`
  );
}

function shouldUnpack(
  harmonyArchivePath: AbsolutePath,
  pkgHarmonyDirPath: AbsolutePath,
  outputDirPath: AbsolutePath,
  nodePackageVersion: string
) {
  if (!fs.existsSync(harmonyArchivePath.getValue())) return false;
  if (!fs.existsSync(pkgHarmonyDirPath.getValue())) return true;
  const ohModulesInfo = getOhModulesInfo(pkgHarmonyDirPath, outputDirPath);
  for (const ohModuleInfo of ohModulesInfo) {
    if (
      !fs.existsSync(ohModuleInfo.computedModulePath.getValue()) ||
      getOhPackageVersion(ohModuleInfo.computedModulePath) !==
        nodePackageVersion
    ) {
      return true;
    }
  }
  return false;
}

function getOhPackageVersion(modulePath: AbsolutePath) {
  const ohPackagePath = modulePath.copyWithNewSegment('oh-package.json5');
  if (!fs.existsSync(ohPackagePath.getValue())) return null;
  const content = fs.readFileSync(ohPackagePath.getValue(), {
    encoding: 'utf-8',
  });
  return JSON5.parse(content)['version'];
}

function getOhModulesInfo(
  pkgHarmonyDirPath: AbsolutePath,
  outputDirPath: AbsolutePath
) {
  const results: {
    archivePath: AbsolutePath;
    archiveFileName: string;
    computedModulePath: AbsolutePath;
    computedModuleName: string;
  }[] = [];
  if (!fs.existsSync(pkgHarmonyDirPath.getValue())) return [];
  const names = fs.readdirSync(pkgHarmonyDirPath.getValue());
  for (const name of names) {
    if (name.endsWith('.tar.gz')) {
      const archivePath = pkgHarmonyDirPath.copyWithNewSegment(name);
      const computedModuleName = pathUtils
        .parse(archivePath.getValue())
        .name.split('.')[0];
      const computedModulePath =
        outputDirPath.copyWithNewSegment(computedModuleName);
      results.push({
        archivePath,
        archiveFileName: name,
        computedModulePath,
        computedModuleName,
      });
    }
  }
  return results;
}

function unpackTarGz(archivePath: AbsolutePath, outputDirPath: AbsolutePath) {
  if (!fs.existsSync(outputDirPath.getValue())) {
    fs.mkdirSync(outputDirPath.getValue(), { recursive: true });
  }
  const readStream = fs.createReadStream(archivePath.getValue());
  const writeStream = tar.extract({
    cwd: outputDirPath.getValue(),
  });
  readStream.pipe(zlib.createGunzip()).pipe(writeStream);
  return new Promise((resolve, reject) => {
    writeStream.on('finish', () => {
      resolve(undefined);
    });
    writeStream.on('error', (err) => {
      reject(err);
    });
  });
}
