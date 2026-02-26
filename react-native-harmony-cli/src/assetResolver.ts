/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
import fs from 'fs';
import pathUtils from 'path';
import type { AssetData } from 'metro';
import { Logger } from './io';

const ENABLE_DEBUG_LOG = process.env.RNOH_BUNDLE_DEBUG === 'true';

function debugLog(message: string) {
  if (ENABLE_DEBUG_LOG) {
    const timestamp = new Date().toISOString().replace('T', ' ').substring(0, 23);
    console.log(`[${timestamp}] ${message}`);
  }
}

type Path = string;
type CopiedFiles = Record<Path, Path>;

const ALLOWED_SCALES: number[] = [1, 2, 3, 4];

function filterAssetScales(
  scales: readonly number[],
): readonly number[] {
  const result = scales.filter(scale => ALLOWED_SCALES.includes(scale));
  if (result.length === 0 && scales.length > 0) {
    // No matching scale found, but there are some available. Ideally we don't
    // want to be in this situation and should throw, but for now as a fallback
    // let's just use the closest larger image
    const maxScale = ALLOWED_SCALES[ALLOWED_SCALES.length - 1];
    for (const scale of scales) {
      if (scale > maxScale) {
        result.push(scale);
        break;
      }
    }
    // There is no larger scales available, use the largest we have
    if (result.length === 0) {
      result.push(scales[scales.length - 1]);
    }
  }
  return result;
}

export async function copyAssets(
  logger: Logger,
  assetsData: readonly AssetData[],
  assetsDest: Path
): Promise<void> {
  if (assetsDest == null) {
    logger.warn((s) => 'Assets destination folder is not set, skipping...');
    return;
  }
  
  const filesToCopy: CopiedFiles = {};
  const addAssetToCopy = (asset: AssetData) => {
    const validScales = filterAssetScales(asset.scales);
    asset.scales.forEach((scale, idx) => {
      if (!validScales.includes(scale)) {
        return;
      }
      const src = asset.files[idx];
      const dest = pathUtils.join(assetsDest, getAssetDestRelativePath(asset, scale));
      filesToCopy[src] = dest;
    });
  };
  
  assetsData.forEach(addAssetToCopy);

  return copyFiles(logger, filesToCopy);
}

function getAssetDestRelativePath(asset: AssetData, scale: number = 1): string {
  const suffix = scale === 1 ? '' : `@${scale}x`;
  const fileName = getResourceIdentifier(asset);
  // Assets can have relative paths outside of the project root.
  // Replace `../` with `_` to make sure they don't end up outside of
  // the expected assets directory.
  return `${fileName + suffix}.${asset.type}`.replace(/\.\.\//g, '_');
}

function getResourceIdentifier(asset: AssetData): string {
  const folderPath = getBasePath(asset);
  return `${folderPath}/${asset.name}`.replace(/^assets\//, '');
}

function getBasePath(asset: AssetData): string {
  const basePath = asset.httpServerLocation;
  return basePath.startsWith('/') ? basePath.slice(1) : basePath;
}

function copyFiles(logger: Logger, fileDestBySrc: CopiedFiles) {
  const fileSources = Object.keys(fileDestBySrc);
  if (fileSources.length === 0) {
    return Promise.resolve();
  }

  const assetFilesCount = fileSources.length;
  return new Promise<void>((resolve, reject) => {
    const copyNext = (error?: Error) => {
      if (error) {
        reject(error);
        return;
      }
      if (fileSources.length === 0) {
        logger.info(
          () =>
            `Copied ${assetFilesCount} ${
              assetFilesCount === 1 ? 'asset' : 'assets'
            }`
        );
        debugLog('All assets copied, resolving promise...');
        resolve();
        debugLog('Assets copy promise resolved');
      } else {
        // fileSources.length === 0 is checked in previous branch, so this is string
        const src = fileSources.shift()!;
        const dest = fileDestBySrc[src];
        copyFile(src, dest, copyNext);
      }
    };
    copyNext();
  });
}

function copyFile(
  src: string,
  dest: string,
  onFinished: (error?: Error) => void
): void {
  const destDir = pathUtils.dirname(dest);
  fs.mkdir(destDir, { recursive: true }, (err?) => {
    if (err) {
      onFinished(err);
      return;
    }
    const readStream = fs.createReadStream(src);
    const writeStream = fs.createWriteStream(dest);
    
    let finished = false;
    const handleError = (err: Error) => {
      if (!finished) {
        finished = true;
        console.error(`[Asset Copy Error] ${src} -> ${dest}:`, err.message);
        readStream.destroy();
        writeStream.destroy();
        onFinished(err);
      }
    };
    
    const handleFinish = () => {
      if (!finished) {
        finished = true;
        onFinished();
      }
    };
    
    readStream.on('error', handleError);
    writeStream.on('error', handleError);
    writeStream.on('finish', handleFinish);
    
    readStream.pipe(writeStream);
  });
}
