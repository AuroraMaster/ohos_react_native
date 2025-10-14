/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import Url from '@ohos.url'
import util from '@ohos.util';
import { Blob, BlobMetadata, BlobPart } from './types';


export class BlobRegistry {
  private blobById: Map<string, Blob> = new Map();

  findByUri(uri: string): Blob | null {
    const url = Url.URL.parseURL(uri);
    const urlParams = new Url.URLParams(url.search)
    const blobId: string | undefined = url.pathname.split("/").pop();
    let offset = 0;
    let size = -1;
    const offsetParam = urlParams.get("offset");
    if (offsetParam !== null) {
      offset = parseInt(offsetParam, 10);
    }
    const sizeParam = urlParams.get("size");
    if (sizeParam !== null) {
      size = parseInt(sizeParam, 10);
    }
    return this.findById(blobId ?? '', offset, size);
  }

  findById(blobId: string, offset: number, size: number): Blob | null {
    const data = this.blobById.get(blobId);
    if (!data) {
      return null;
    }
    if (size === -1) {
      size = data.byteLength - offset;
    }

    if (offset > 0 || size !== data.byteLength) {
      const bytes = new Uint8Array(data, offset, size);
      return bytes.buffer;
    }
    return data;
  }

  findByMetadata(blobMetadata: BlobMetadata): Blob | null {
    return this.findById(blobMetadata.blobId, blobMetadata.offset, blobMetadata.size);
  }

  appendPartsToBlob(parts: Array<BlobPart>, blobId: string): void {
    let totalBlobSize: number = 0;
    let partList: Array<ArrayBuffer> = new Array<ArrayBuffer>(parts.length);

    for (let i = 0; i < parts.length; i++) {
      let part = parts[i];
      switch (part.type) {
        case "blob":
          let blobMetadata = part.data as BlobMetadata;
          totalBlobSize += blobMetadata.size;
          partList[i] = this.findByMetadata(blobMetadata);
          break;
        case "string":
          let textEncoder = new util.TextEncoder();
          let blob: Blob = textEncoder.encodeInto(part.data as string).buffer;
          totalBlobSize += blob.byteLength;
          partList[i] = blob;
          break;
        default:
          throw new Error("Invalid type for blob: " + part["type"]);
      }
    }
    const blob: Blob = new ArrayBuffer(totalBlobSize);
    const bufferView = new Uint8Array(blob);
    let pos = 0;
    for (let part of partList) {
      let partView = new Uint8Array(part);
      bufferView.set(partView, pos);
      pos += part.byteLength;
    }
    this.saveWithId(blob, blobId);
  }

  save(blob: Blob): string {
    const blobId: string = util.generateRandomUUID();
    this.saveWithId(blob, blobId);
    return blobId;
  }

  saveWithId(blob: Blob, blobId: string): void {
    this.blobById.set(blobId, blob);
  }

  deleteBlobById(blobId: string) {
    this.blobById.delete(blobId);
  }
}