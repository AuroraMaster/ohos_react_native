/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

import type {RootTag} from 'react-native/Libraries/Types/RootTagTypes';
import type {ResolvedAssetSource} from 'react-native/Libraries/Image/AssetSourceResolver';
import type {ImageProps as ImagePropsType} from 'react-native/Libraries/Image/ImageProps';

import ImageViewNativeComponent from 'react-native/Libraries/Image/ImageViewNativeComponent';
import * as React from 'react';

type ImageComponentStaticsHarmony = $ReadOnly<{|
  getSize: (
    uri: string,
    success: (width: number, height: number) => void,
    failure?: (error: any) => void,
  ) => void,

  getSizeWithHeaders(
    uri: string,
    headers: {[string]: string, ...},
    success: (width: number, height: number) => void,
    failure?: (error: any) => void,
  ): any,

  prefetch(url: string, callback: ?(requestId: number) => void): any,

  prefetchWithMetadata(
    url: string,
    queryRootName: string,
    rootTag?: ?RootTag,
  ): any,

  queryCache(
    urls: Array<string>,
  ): Promise<{[string]: 'memory' | 'disk' | 'disk/memory', ...}>,

  resolveAssetSource(source: any): ?ResolvedAssetSource,

  abortPrefetch(requestId: number): void,
|}>;

export type ImageHarmony = React.AbstractComponent<
  ImagePropsType,
  React.ElementRef<typeof ImageViewNativeComponent>,
> &
  ImageComponentStaticsHarmony;
