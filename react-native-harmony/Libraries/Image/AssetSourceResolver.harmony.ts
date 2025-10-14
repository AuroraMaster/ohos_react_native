/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import type { AssetData } from 'metro';
// @ts-ignore
import { getBasePath } from '@react-native/assets-registry/path-support';
import { Dimensions, Platform } from 'react-native';

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

function pickScale(scales: Array<number>, deviceScale?: number): number {
  const validScales = filterAssetScales(scales);
  if (deviceScale == null) {
    deviceScale = Dimensions.get('window').scale;
  }
  // Packager guarantees that `scales` array is sorted
  for (let i = 0; i < validScales.length; i++) {
    if (validScales[i] >= deviceScale) {
      return validScales[i];
    }
  }

  // If nothing matches, device scale is larger than any available
  // scales, so we return the biggest one. Unless the array is empty,
  // in which case we default to 1
  return validScales[validScales.length - 1] || 1;
}

function getScaledAssetPath(asset: AssetData): string {
  const scale = pickScale(asset.scales);
  const scaleSuffix = scale === 1 ? '' : '@' + scale + 'x';
  const assetDir = getBasePath(asset);
  return assetDir + '/' + asset.name + scaleSuffix + '.' + asset.type;
}

type ResolvedAssetSource = {
  readonly __packager_asset: boolean;
  readonly width?: number;
  readonly height?: number;
  readonly uri: string;
  readonly scale: number;
};

class AssetSourceResolver {
  constructor(
    private serverUrl: string | undefined,
    private jsbundleUrl: string | undefined,
    private asset: AssetData
  ) {}

  isLoadedFromServer(): boolean {
    return !!this.serverUrl;
  }

  isLoadedFromFileSystem(): boolean {
    return !!(this.jsbundleUrl && this.jsbundleUrl.startsWith('file://'));
  }

  public defaultAsset(): ResolvedAssetSource {
    if (this.isLoadedFromServer()) {
      return this.assetServerURL();
    }

    const path = this.isLoadedFromFileSystem() ? this.jsbundleUrl : 'asset://';

    // Assets can have relative paths outside of the project root.
    // Replace `../` with `_` to make sure they don't end up outside of
    // the expected assets directory.
    return this.fromSource(
      path +
        getScaledAssetPath(this.asset)
          .replace(/^assets\//, '')
          .replace(/\.\.\//g, '_')
    );
  }

  /**
   * Returns an absolute URL which can be used to fetch the asset
   * from the devserver
   */
  assetServerURL(): ResolvedAssetSource {
    if (!this.serverUrl) {
      throw new Error('need server to load from');
    }

    return this.fromSource(
      this.serverUrl +
        getScaledAssetPath(this.asset) +
        '?platform=' +
        Platform.OS +
        '&hash=' +
        this.asset.hash
    );
  }

  fromSource(source: string): ResolvedAssetSource {
    return {
      __packager_asset: true,
      width: this.asset.width,
      height: this.asset.height,
      uri: source,
      scale: pickScale(this.asset.scales),
    };
  }
}

module.exports = AssetSourceResolver;
