/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import http from '@ohos.net.http';

export type FetchOptions = {
  headers?: Record<string, any>,
  usingCache?: boolean
}

export type FetchResult = {
  headers: Object,
  result: ArrayBuffer,
  responseCode?: number
}

export async function fetchDataFromUrl(url: string, options: FetchOptions = { usingCache: true }, onProgress?: (progress: number) => void): Promise<FetchResult> {
  return new Promise((resolve, reject) => {
    const httpRequest = http.createHttp();
    const dataChunks: ArrayBuffer[] = [];
    let result: ArrayBuffer | undefined;
    let headers: Object | undefined;
    let responseCode: number | undefined;
    let isCleanedUp = false;

    const onDataReceiveProgress = ({receiveSize, totalSize}: {receiveSize: number, totalSize: number}) => {
      onProgress?.(receiveSize / totalSize);
    };

    const onHeadersReceive = (data: Object) => {
      headers = data;
      maybeResolve();
    };

    const onDataReceive = (chunk: ArrayBuffer) => {
      dataChunks.push(chunk);
    };

    const onDataEnd = () => {
      const totalLength = dataChunks.map(chunk => chunk.byteLength).reduce((acc, length) => acc + length, 0);
      const data = new Uint8Array(totalLength);
      let offset = 0;
      for (const chunk of dataChunks) {
        const chunkArray = new Uint8Array(chunk);
        data.set(chunkArray, offset);
        offset += chunk.byteLength;
      }
      result = data.buffer;
      maybeResolve();
    };

    httpRequest.on("dataReceiveProgress", onDataReceiveProgress);
    httpRequest.on("headersReceive", onHeadersReceive);
    httpRequest.on("dataReceive", onDataReceive);
    httpRequest.on("dataEnd", onDataEnd);

    function cleanUp() {
      if (isCleanedUp) {
        return;
      }
      isCleanedUp = true;
      httpRequest.off("dataReceiveProgress", onDataReceiveProgress);
      httpRequest.off("headersReceive", onHeadersReceive);
      httpRequest.off("dataReceive", onDataReceive);
      httpRequest.off("dataEnd", onDataEnd);
      httpRequest.destroy();
    }

    function maybeResolve() {
      if (result !== undefined && headers !== undefined && responseCode !== undefined) {
        resolve({ headers, result, responseCode });
        cleanUp();
      }
    }

    try {
      httpRequest.requestInStream(
        url,
        {
          header: options.headers,
          usingCache: options.usingCache
        },
        (err, data) => {
          responseCode = data
          if (err) {
            reject(new Error(`Couldn't fetch data from ${url}, ${err.message}`));
            cleanUp();
          } else {
            maybeResolve();
          }
        }
      );
    } catch (err) {
      reject(new Error(`Couldn't fetch data from ${url}, ${err.message}`));
      cleanUp();
    }
  })
}