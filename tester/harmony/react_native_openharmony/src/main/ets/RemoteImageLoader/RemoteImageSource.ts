/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import image from '@ohos.multimedia.image'

export class RemoteImageSource {
  private pixelMapPromise: Promise<image.PixelMap> | undefined

  constructor(private imageSource: image.ImageSource, private location: string) {
  }
  
  getImageSource(): image.ImageSource {
    return this.imageSource;
  }

  getLocation(): string {
    return this.location;
  }

  async getPixelMapPromise(): Promise<image.PixelMap> {
    if (this.pixelMapPromise === undefined) {
      return this.createPixelMap();
    }

    return this.pixelMapPromise;
  }

  private createPixelMap(): Promise<image.PixelMap> {
    const pixelMapPromise = (async () => {
      if (await this.imageSource.getFrameCount() === 1) {
        return this.imageSource.createPixelMap()
      }
      throw Error("Cannot create a PixelMap for an animated image");
    })()
    this.pixelMapPromise = pixelMapPromise;

    pixelMapPromise.catch(() => this.pixelMapPromise = undefined);

    return pixelMapPromise;
  }
}