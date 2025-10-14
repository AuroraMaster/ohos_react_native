/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import image from '@ohos.multimedia.image';
import type { RemoteImageMemoryCache } from './RemoteImageCache';
import { RemoteImageLoaderError } from './RemoteImageLoaderError';
import request from '@ohos.request';
import type { RemoteImageDiskCache } from './RemoteImageDiskCache';
import fs from '@ohos.file.fs';
import { fetchDataFromUrl, FetchOptions, FetchResult } from '../RNOH/HttpRequestHelper';
import { RemoteImageSource } from './RemoteImageSource';
import { common } from '@kit.AbilityKit';

export class RemoteImageLoader {
  private activeRequestByUrl: Map<string, Promise<FetchResult>> = new Map();
  private activePrefetchByUrl: Map<string, Promise<boolean>> = new Map();
  private abortPrefetchByUrl: Map<number, request.DownloadTask> = new Map()

  public constructor(
    private memoryCache: RemoteImageMemoryCache,
    private diskCache: RemoteImageDiskCache,
    private context: common.UIAbilityContext,
    private onDiskCacheUpdate: (e: {remoteUri: string, fileUri: string}) => void,
  ) {
  }

  private async fetchImage(url: string, headers?: Record<string, any>): Promise<FetchResult> {
    if (this.activeRequestByUrl.has(url)) {
      return this.activeRequestByUrl.get(url);
    }

    let options: FetchOptions = {
      usingCache: true,
      headers: headers,
    };
    const promise = fetchDataFromUrl(url, options);
    this.activeRequestByUrl.set(url, promise);
    promise.finally(() => {
      this.activeRequestByUrl.delete(url);
    });
    return promise;
  }

  public async getPixelMap(uri: string): Promise<image.PixelMap> {
    const imageSource = await this.getImageSource(uri);
    return await imageSource.getPixelMapPromise();
  }

  public async getImageSource(uri: string, headers?: Record<string, any>): Promise<RemoteImageSource> {
    if (uri.startsWith("data:")) {
      const imageSource = image.createImageSource(uri);
      return new RemoteImageSource(imageSource, '');
    }
    if (this.memoryCache.has(uri)) {
      return this.memoryCache.get(uri);
    }

    // if the image is being prefetched, wait until it's downloaded,
    // ignoring any errors that occur (we'll try to download it to memory instead)
    if (this.activePrefetchByUrl.has(uri)) {
      await this.activePrefetchByUrl.get(uri).catch(() => {
      });
    }

    if (this.diskCache.has(uri)) {
      const location = `file://${this.diskCache.getLocation(uri)}`;
      const imageSource = image.createImageSource(location);
      if (imageSource === null) {
        throw new RemoteImageLoaderError("Couldn't create ImageSource");
      }
      const remoteImageSource = new RemoteImageSource(imageSource, location);
      this.memoryCache.set(uri, remoteImageSource);
      return remoteImageSource;
    }

    let response: FetchResult;
    try {
      response = await this.fetchImage(uri, headers);
    } catch (e) {
      throw new RemoteImageLoaderError(
        e.message ?? 'Failed to fetch the image',
      );
    }

    // if more than one component tried fetching at the same time,
    // it's possible the source is now in the cache.
    // Check it first before creating a new source
    if (this.memoryCache.has(uri)) {
      return this.memoryCache.get(uri);
    }

    const imageSource = image.createImageSource(response.result);
    if (imageSource === null) {
      throw new RemoteImageLoaderError("Couldn't create ImageSource");
    }
    // NOTE: workaround for `Image` component not supporting redirects or ImageSource sources.
    // If the image is an animated GIF, we cannot convert it to a PixelMap
    // since that would display a single frame only.
    // Instead, when redirected, we attach the `location` url to the imageSource object
    // and use it as the source parameter for `Image`
    // eslint-disable-next-line dot-notation
    const location = response.headers['location'] ?? uri;
    const remoteImageSource = new RemoteImageSource(imageSource, location);
    this.memoryCache.set(uri, remoteImageSource);

    if (!this.activePrefetchByUrl.has(uri) && !this.diskCache.has(uri)) {
      // some other component might have prefetched it
      const promise = this.saveFile(uri, response.result);
      this.activePrefetchByUrl.set(uri, promise);
      promise.finally(() => {
        this.activePrefetchByUrl.delete(uri);
      });
    }
    return remoteImageSource;
  }

  private async saveFile(uri: string, arrayBuffer: ArrayBuffer) {
    try {
      const path = this.diskCache.getLocation(uri);
      const file = await fs.open(
        path,
        // eslint-disable-next-line no-bitwise
        fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE,
      );
      await fs.write(file.fd, arrayBuffer);
      fs.close(file);
      this.diskCache.set(uri);
    } catch (error) {
      console.error(
        'Error occurred when storing file for disk cache ' + error.message,
      );
    }
    return true;
  }

  public async abortPrefetch(requestId: number): Promise<void> {
    this.abortPrefetchByUrl.get(requestId)?.delete();
  }

  public async prefetch(uri: string, requestId: number): Promise<boolean> {
    if (this.diskCache.has(uri)) {
      return true;
    }

    if (this.activePrefetchByUrl.has(uri)) {
      return await this.activePrefetchByUrl.get(uri);
    }

    // evict the image from cache if present
    if (this.memoryCache.has(uri)) {
      this.memoryCache.remove(uri);
    }

    const promise = this.downloadFile(uri, requestId);
    this.activePrefetchByUrl.set(uri, promise);
    promise.finally(() => {
      this.activePrefetchByUrl.delete(uri);
      const fileUri = `file://${this.diskCache.getLocation(uri)}`;
      this.onDiskCacheUpdate({remoteUri: uri, fileUri})
    });

    return await promise;
  }

  private async performDownload(config: request.DownloadConfig, requestId: number): Promise<boolean> {
    return await new Promise(async (resolve, reject) => {
      try {
        const downloadTask = await request.downloadFile(this.context, config);
        this.abortPrefetchByUrl.set(requestId, downloadTask);
        downloadTask.on("complete", () => {
          this.abortPrefetchByUrl.delete(requestId);
          resolve(true)
        });
        downloadTask.on("remove", () => {
          this.abortPrefetchByUrl.delete(requestId);
          reject(`Download task remove.`)
        });
        downloadTask.on("fail", (err: number) => {
          this.abortPrefetchByUrl.delete(requestId);
          reject(`Failed to download the task. Code: ${err}`)
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  private async downloadFile(uri: string, requestId: number): Promise<boolean> {
    const path = this.diskCache.getLocation(uri);
    const tempPath = path + '_tmp';

    try {
      // Download to a temporary location to avoid risks of corrupted files from incomplete downloads, 
      // as request.downloadFile does not clean up failed downloads automatically.
      if (fs.accessSync(tempPath)){
        await fs.unlink(tempPath);
      }
      await this.performDownload({ url: uri, filePath: tempPath }, requestId);
      // Move the file to the final location and remove the temporary file
      await fs.moveFile(tempPath, path);
      this.diskCache.set(uri);
    } catch (e) {
      return Promise.reject(e);
    }
    return true;
  }

  public queryCache(uri: string): 'memory' | 'disk' | undefined {
    if (this.diskCache.has(uri)) {
      return 'disk';
    }
    if (this.memoryCache.has(uri)) {
      return 'memory';
    }
    return undefined;
  }

  public getPrefetchResult(uri: string) {
    if (this.diskCache.has(uri)) {
      return `file://${this.diskCache.getLocation(uri)}`;
    }
    return uri;
  }
}
