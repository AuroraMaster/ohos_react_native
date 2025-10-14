/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AnyThreadTurboModule } from '../../../RNOH/TurboModule';
import { BlobTurboModule } from './BlobTurboModule';
import { BlobMetadata as BlobMetadata } from './types';
import util from '@ohos.util';

export class FileReaderTurboModule extends AnyThreadTurboModule {
  public static readonly NAME = 'FileReaderModule';

  private base64Helper = new util.Base64Helper();


  private getBlobTurboModule(): BlobTurboModule {
    const blobModule = this.ctx.rnInstance.getTurboModule<BlobTurboModule>(BlobTurboModule.NAME);
    if (blobModule == null) {
      throw new Error('FileReaderModule::Could not get BlobModule')
    }
    return blobModule;
  }

  async readAsText(blobMetadata: BlobMetadata, encoding?: string): Promise<string> {
    const blobModule = this.getBlobTurboModule();
    const bytes = new Uint8Array(blobModule.findByMetaData(blobMetadata));
    const textDecoder = util.TextDecoder.create(encoding);
    const result = textDecoder.decodeWithStream(bytes, { stream: false });
    return result;
  }

  async readAsDataURL(blobMetadata: BlobMetadata): Promise<string> {

    const blobModule = this.getBlobTurboModule();
    const bytes = new Uint8Array(blobModule.findByMetaData(blobMetadata));
    let urlString = 'data:';
    if (blobMetadata.type != null && blobMetadata.type.length > 0) {
      urlString += blobMetadata.type;
    } else {
      urlString += 'application/octet-stream';
    }
    urlString += ';base64,';
    urlString += await this.base64Helper.encodeToString(bytes);
    return urlString;
  }
}
