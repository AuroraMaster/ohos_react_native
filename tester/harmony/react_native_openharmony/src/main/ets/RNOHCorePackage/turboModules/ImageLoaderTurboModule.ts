/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AnyThreadTurboModule } from '../../RNOH/TurboModule';
import {
  RemoteImageDiskCache,
  RemoteImageLoader, RemoteImageMemoryCache,
  RemoteImageLoaderError
} from '../../RemoteImageLoader';
import { AnyThreadTurboModuleContext } from '../../RNOH/RNOHContext';
import image from '@ohos.multimedia.image';
import { RemoteImageSource } from '../../RemoteImageLoader/RemoteImageSource';

export class ImageLoaderTurboModule extends AnyThreadTurboModule {
  static NAME = "ImageLoader" as const

  private imageLoader: RemoteImageLoader

  constructor(protected ctx: AnyThreadTurboModuleContext) {
    super(ctx)
    this.imageLoader = new RemoteImageLoader(
      new RemoteImageMemoryCache(128), new RemoteImageDiskCache(128, `${ctx.uiAbilityContext.cacheDir}/rn_image_cache`),
      ctx.uiAbilityContext, ({ remoteUri, fileUri }) => {
      ctx.rnInstance.postMessageToCpp('UPDATE_IMAGE_SOURCE_MAP', {
        remoteUri,
        fileUri,
      });
    })
  }

  /**
   * called from cpp
   */
  protected getPrefetchResult(uri: string): string | undefined {
    return this.imageLoader.getPrefetchResult(uri);
  }

  public getConstants() {
    return {}
  }

  public async getSize(uri: string): Promise<number[]> {
    if (uri && uri.length > 0) {
      const imageSource = await this.imageLoader.getImageSource(uri)
      const imageInfo = await imageSource.getImageSource().getImageInfo()
      return [imageInfo.size.width, imageInfo.size.height]
    } else {
      throw 'Cannot get the size of an image for an empty URI'
    }
  }

  public async getSizeWithHeaders(uri: string, headers: Object): Promise<{
    width: number,
    height: number
  } & Record<string, any>> {
    let srcHeaders = headers as Record<string, any>
    let destHeaders: Record<string, any> = {}

    if (srcHeaders['crossOrigin'] === 'use-credentials') {
      destHeaders['Access-Control-Allow-Credentials'] = 'true'
    }

    if (srcHeaders['referrerPolicy'] !== undefined) {
      destHeaders['Referrer-Policy'] = srcHeaders['referrerPolicy']
    }

    const imageSource = await this.imageLoader.getImageSource(uri, destHeaders)
    const imageInfo = await imageSource.getImageSource().getImageInfo()
    return Promise.resolve({ width: imageInfo.size.width, height: imageInfo.size.height })
  }

  public async prefetchImage(uri: string, requestId: number = 0): Promise<boolean> {
    return this.imageLoader.prefetch(uri, requestId);
  }

  public abortPrefetch(requestId: number): void {
    this.imageLoader.abortPrefetch(requestId)
  }

  public prefetchImageWithMetadata(uri: string, queryRootName: string, rootTag: number): Promise<boolean> {
    this.ctx.logger.warn("ImageLoader::prefetchImageWithMetadata is not supported")
    return Promise.resolve(false)
  }

  public queryCache(uris: Array<string>): Promise<Object> {
    const cachedUriEntries = uris.map(uri =>
      [uri, this.imageLoader.queryCache(uri)]
    ).filter(([_uri, value]) => value !== undefined);
    const cachedUriMap = Object.fromEntries(cachedUriEntries)
    return Promise.resolve(cachedUriMap)
  }

  public async getRemoteImageSource(uri:string): Promise<RemoteImageSource> {
    try {
      const imageSource = await this.imageLoader.getImageSource(uri);
      return imageSource;
    }
    catch (e) {
      if (!(e instanceof RemoteImageLoaderError) && e instanceof Object && e.message) {
        throw new RemoteImageLoaderError(`Failed to load the image: ${e.message}`);
      }
      if (typeof e === 'string') {
        throw new RemoteImageLoaderError(e);
      }
      throw e;
    }
  }

  public async getImageSource(uri: string): Promise<image.ImageSource> {
    return (await this.getRemoteImageSource(uri)).getImageSource()
  }

  public async getPixelMap(uri: string): Promise<image.PixelMap> {
    try {
      return await this.imageLoader.getPixelMap(uri);
    }
    catch (e) {
      if (!(e instanceof RemoteImageLoaderError) && e instanceof Object && e.message) {
        throw new RemoteImageLoaderError(`Failed to load the image: ${e.message}`);
      }
      if (typeof e === 'string') {
        throw new RemoteImageLoaderError(e);
      }
      throw e;
    }
  }

}