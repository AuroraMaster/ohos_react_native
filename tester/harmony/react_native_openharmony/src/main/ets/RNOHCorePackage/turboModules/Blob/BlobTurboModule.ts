/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AnyThreadTurboModule } from '../../../RNOH/TurboModule';
import util from '@ohos.util';
import fs from '@ohos.file.fs';
import { ContentHandler, WebSocketTurboModule } from '../WebSocketTurboModule';
import { NetworkingTurboModule, RequestBodyHandler, ResponseBodyHandler, UriHandler } from '../Networking/index';
import uri from '@ohos.uri';
import { Blob, BlobMetadata, BlobPart } from './types';
import { BlobRegistry } from './BlobRegistry';

/**
 * BlobTurboModule implements js Blobs. NetworkingTurboModule handlers are used, to allow handling blobs differently than normal data.
 * Reading the data stored in Blobs is implemented by FileReaderTurboModule.
 */

export class BlobTurboModule extends AnyThreadTurboModule {
  public static readonly NAME = 'BlobModule';
  private blobRegistry: BlobRegistry = new BlobRegistry()

  private contentHandler: ContentHandler = {
    processMessage: (params) => {
      return params;
    },
    processByteMessage: (blob, params) => {
      return {
        id: params.id,
        type: "blob",
        data: {
          blobId: this.blobRegistry.save(blob),
          offset: 0,
          size: blob.byteLength
        }
      }
    }

  }

  private networkingUriHandler: UriHandler = {
    supports: (query) => {
      const parsedUri = new uri.URI(query.url)
      const remote = parsedUri.scheme === 'http' || parsedUri.scheme === 'https';
      if (!remote && query.responseType === 'blob') {
        return true;
      }
      return false;
    },
    fetch: async (query) => {
      const parsedUri = new uri.URI(query.url)
      if (parsedUri.scheme === 'asset') {
        // assets can't be handled using normal file API's
        const path = query.url.replace("asset://", this.ctx.rnInstance.getAssetsDest())
        const bytes = this.ctx.uiAbilityContext.resourceManager.getRawFileContentSync(path);
        const blob = bytes.buffer;
        return {
          blobId: this.blobRegistry.save(blob),
          offset: bytes.byteOffset,
          size: bytes.byteLength,
          type: undefined, //no way to find the type using Ark
          name: path,
          lastModified: 0 //we don't know the value
        }
      }
      if (parsedUri.scheme === 'file') {
        query.url = query.url.replace('file://', '');
      }
      const file = fs.openSync(query.url, fs.OpenMode.READ_ONLY);
      const stat = await fs.stat(query.url);
      const blob: Blob = new ArrayBuffer(stat.size);
      await fs.read(file.fd, blob);
      return {
        blobId: this.blobRegistry.save(blob),
        offset: 0,
        size: stat.size,
        type: undefined, //no way to find the type using Ark
        name: file.name,
        lastModified: stat.mtime
      }
    }
  }

  private requestBodyHandler: RequestBodyHandler = {
    supports: (data) => {
      if ('blob' in data) {
        return true;
      }
      return false;
    },

    handleRequest: (data, contentType) => {
      let type = contentType;
      if (data.blob.type) {
        type = data.blob.type;
      }
      if (type == null || type === '') {
        type = 'application/octet-stream';
      }

      if ('blob' in data) {
        const blobMetadata: BlobMetadata = data.blob;
        const bytes = this.blobRegistry.findByMetadata(blobMetadata);

        return { body: bytes, contentType: type }
      }
    }
  }

  private responseBodyHandler: ResponseBodyHandler = {
    supports: (responseType) => {
      return 'blob' === responseType;
    },
    handleResponse: (response) => {
      if (response instanceof ArrayBuffer) {
        return {
          blobId: this.blobRegistry.save(response),
          offset: 0,
          size: response.byteLength
        }
      } else if (typeof response === 'string') {
        const textEncoder = new util.TextEncoder();
        const bytes: Blob = textEncoder.encodeInto(response).buffer;
        return {
          blobId: this.blobRegistry.save(bytes),
          offset: 0,
          size: bytes.byteLength
        }
      }

    }
  }


  private getWebSocketModule(): WebSocketTurboModule {
    return this.ctx.rnInstance.getTurboModule<WebSocketTurboModule>(WebSocketTurboModule.NAME);
  }

  private getNetworkingModule(): NetworkingTurboModule {
    return this.ctx.rnInstance.getTurboModule<NetworkingTurboModule>(NetworkingTurboModule.NAME);
  }

  findByUri(uri: string): Blob | null {
    return this.blobRegistry.findByUri(uri);
  }

  findByMetaData(blob: BlobMetadata): Blob | null {
    return this.blobRegistry.findByMetadata(blob);
  }

  createFromParts(parts: Array<BlobPart>, blobId: string): void {
    this.blobRegistry.appendPartsToBlob(parts, blobId)
  }

  sendOverSocket(blob: BlobMetadata, idDouble: number): void {
    const id = Math.floor(idDouble);
    const webSocketModule = this.getWebSocketModule();

    if (webSocketModule) {
      const data = this.blobRegistry.findById(blob.blobId, blob.offset, blob.size);

      if (data) {
        webSocketModule.sendBinaryArray(new Uint8Array(data), id);
      } else {
        webSocketModule.sendBinary(null, id);
      }
    }
  }

  addNetworkingHandler(): void {
    const networkingModule = this.getNetworkingModule();
    networkingModule.addUriHandler(this.networkingUriHandler);
    networkingModule.addRequestBodyHandler(this.requestBodyHandler);
    networkingModule.addResponseBodyHandler(this.responseBodyHandler);

  }

  addWebSocketHandler(socketID: number): void {
    const websocketModule = this.getWebSocketModule()
    websocketModule.setContentHandler(socketID, this.contentHandler);
  }

  removeWebSocketHandler(socketID: number): void {
    const module = this.getWebSocketModule()
    module.setContentHandler(socketID, null);
  }

  release(blobId: string): void {
    this.blobRegistry.deleteBlobById(blobId)
  }

  getConstants() {
    return { BLOB_URI_SCHEME: 'blob', BLOB_URI_HOST: null };
  }
}
