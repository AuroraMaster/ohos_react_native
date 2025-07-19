/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Command } from './types';
import { AbsolutePath, DescriptiveError } from '../core';
import { Logger, RealFS } from '../io';
import { Autolinking } from '../autolinking';

const COMMAND_NAME = 'link-harmony';

export const commandLinkHarmony: Command = {
  name: COMMAND_NAME,
  description:
    'Handles the automatic linking of RNOH libraries. This command is meant to be used by the RNOH HvigorPlugin.',
  options: [
    {
      name: '--harmony-project-path <path>',
      description: 'Specifies the root of your OpenHarmony project.',
      default: './harmony',
    },
    {
      name: '--node-modules-path <path>',
      description:
        'Defines the path to the node_modules directory which holds npm packages.',
      default: './node_modules',
    },
    {
      name: '--cmake-autolink-path-relative-to-harmony <path>',
      description:
        'Path relative to the root of your OpenHarmony project for the generated CMake file involved in autolinking.',
      default: './entry/src/main/cpp/autolinking.cmake',
    },
    {
      name: '--cpp-rnoh-packages-factory-path-relative-to-harmony <path>',
      description:
        'Path relative to the root of your OpenHarmony project, for the generated file used to link packages on C++ side.',
      default: './entry/src/main/cpp/RNOHPackagesFactory.h',
    },
    {
      name: '--ets-rnoh-packages-factory-path-relative-to-harmony <path>',
      description:
        'Path relative to the root of your OpenHarmony project, for the generated file used to link packages on ETS side.',
      default: './entry/src/main/ets/RNOHPackagesFactory.ets',
    },
    {
      name: '--oh-package-path-relative-to-harmony <path>',
      description:
        'Path to the oh-package.json5 relative to the root of your OpenHarmony project.',
      default: './oh-package.json5',
    },
    {
      name: '--exclude-npm-packages [string]',
      description:
        "Lists npm package names to be excluded from the autolinking process, separated by semicolons. By default every library is included. Can't be used with --included-npm-packages.",
    },
    {
      name: '--include-npm-packages [string]',
      description:
        "Lists npm package names to be included from the autolinking process, separated by semicolons. By default every library is included. Can't be used with --excluded-npm-packages.",
    },
  ],
  func: async (_argv, _config, rawArgs: any) => {
    const logger = new Logger();
    const fs = new RealFS();
    const autolinking = new Autolinking(fs, logger);
    try {
      const config = await autolinking.prepareInput({
        harmonyProjectPath: new AbsolutePath(rawArgs.harmonyProjectPath),
        nodeModulesPath: new AbsolutePath(rawArgs.nodeModulesPath),
        cmakeAutolinkPathRelativeToHarmony:
          rawArgs.cmakeAutolinkPathRelativeToHarmony,
        cppRNOHPackagesFactoryPathRelativeToHarmony:
          rawArgs.cppRnohPackagesFactoryPathRelativeToHarmony,
        etsRNOHPackagesFactoryPathRelativeToHarmony:
          rawArgs.etsRnohPackagesFactoryPathRelativeToHarmony,
        ohPackagePathRelativeToHarmony: rawArgs.ohPackagePathRelativeToHarmony,
        excludedNpmPackageNames: new Set(
          rawArgs.excludeNpmPackages
            ? (rawArgs.excludeNpmPackages ?? '').split(';')
            : undefined
        ),
        includedNpmPackageNames: new Set(
          rawArgs.includeNpmPackages
            ? (rawArgs.includeNpmPackages ?? '').split(';')
            : undefined
        ),
      });
      const result = autolinking.evaluate(config);
      autolinking.saveAndLogOutput(result);
    } catch (err) {
      if (err instanceof DescriptiveError && !err.isUnexpected()) {
        logger.descriptiveError(err);
        return;
      }
      throw err;
    }
  },
};