/**
 * Copyright (c) 2024-2025 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Command } from './types';
import fs from 'fs';
import fse from 'fs-extra';
import Metro from 'metro';
import { RunBuildOptions as BuildOptions } from 'metro';
import MetroServer from 'metro/src/Server';
import pathUtils from 'path';
import { copyAssets } from '../assetResolver';
import { ConfigT as MetroConfig } from 'metro-config';
import { Logger } from '../io';

const ENABLE_DEBUG_LOG = process.env.RNOH_BUNDLE_DEBUG === 'true';

function debugLog(message: string) {
  if (ENABLE_DEBUG_LOG) {
    const timestamp = new Date().toISOString().replace('T', ' ').substring(0, 23);
    console.log(`[${timestamp}] ${message}`);
  }
}

const ARK_RESOURCE_PATH = './harmony/entry/src/main/resources/rawfile';
const ASSETS_DEFAULT_DEST_PATH =
  './harmony/entry/src/main/resources/rawfile/assets';

type AssetData = Metro.AssetData;
type Bundle = { code: string; map: string };
type Path = string;

export const commandBundleHarmony: Command = {
  name: 'bundle-harmony',
  description:
    'Creates JS bundle, creates a special cpp header containing the JS code, copies assets directory to the project.',
  options: [
    {
      name: '--dev [boolean]',
      description: 'If false, warnings are disabled and the bundle is minified',
      parse: (val: string) => val !== 'false',
      default: true,
    },
    {
      name: '--entry-file <path>',
      description:
        'Path to the root JS file, either absolute or relative to JS root',
      default: 'index.js',
    },
    {
      name: '--config <path>',
      description: 'Path to the Metro configuration file',
    },
    {
      name: '--bundle-output <path>',
      description:
        'File name where to store the resulting bundle, ex. /tmp/groups.bundle',
      default: ARK_RESOURCE_PATH + '/bundle.harmony.js',
    },
    {
      name: '--assets-dest <path>',
      description:
        'Directory name where to store assets referenced in the bundle',
      default: ASSETS_DEFAULT_DEST_PATH,
    },
    {
      name: '--sourcemap-output <path>',
      description:
        'File name where to store the resulting source map, ex. /tmp/groups.map',
    },
    {
      name: '--minify [boolean]',
      description: 'Allows overriding whether bundle is minified',
      parse: (val: string) => val !== 'false',
    },
  ],
  func: async (argv, config, args: any) => {
    const logger = new Logger();
    debugLog('Starting bundle-harmony process...');
    const buildOptions: BuildOptions = {
      entry: args.entryFile,
      platform: 'harmony',
      minify: args.minify !== undefined ? args.minify : !args.dev,
      dev: args.dev,
      sourceMap: args.sourcemapOutput,
      sourceMapUrl: args.sourcemapOutput,
    };
    const metroConfig = await loadMetroConfig(args.config);
    debugLog('Creating bundle...');
    const bundle = await createBundle(metroConfig, buildOptions);
    await saveBundle(bundle, args.bundleOutput, args.sourcemapOutput);
    const assets = await retrieveAssetsData(metroConfig, buildOptions);
    debugLog('Copying assets...');
    await copyAssets(logger, assets, args.assetsDest);
    debugLog('Assets copy completed, cleaning up...');
    debugLog('Bundle process finished, exiting');
  },
};

async function loadMetroConfig(
  configPath: Path | undefined
): Promise<MetroConfig> {
  return configPath
    ? await Metro.loadConfig({ config: configPath })
    : await Metro.loadConfig();
}

async function createBundle(
  metroConfig: MetroConfig,
  buildOptions: BuildOptions
): Promise<Bundle> {
  // casting needed as Metro.runBuild returns Promise<{code: string, map: string}>
  // despite being typed as Promise<void>
  return (await Metro.runBuild(metroConfig, buildOptions)) as unknown as Bundle;
}

async function saveBundle(
  bundle: Bundle,
  bundleOutput: Path,
  sourceMapOutput: Path | undefined
) {
  await fse.ensureDir(pathUtils.dirname(bundleOutput));
  fs.writeFileSync(bundleOutput, bundle.code);
  console.log(`[CREATED] ${bundleOutput}`);
  if (sourceMapOutput) {
    fs.writeFileSync(sourceMapOutput, bundle.map);
    console.log(`[CREATED] ${sourceMapOutput}`);
  }
}

async function retrieveAssetsData(
  metroConfig: MetroConfig,
  buildOptions: BuildOptions
): Promise<readonly AssetData[]> {
  const metroServer = new MetroServer(metroConfig);
  try {
    return await metroServer.getAssets({
      ...MetroServer.DEFAULT_BUNDLE_OPTIONS,
      ...buildOptions,
      entryFile: buildOptions.entry,
    });
  } finally {
    metroServer.end();
  }
}
