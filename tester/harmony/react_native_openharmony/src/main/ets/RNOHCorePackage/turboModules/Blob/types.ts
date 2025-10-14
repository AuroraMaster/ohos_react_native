/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export type Blob = ArrayBuffer;

export type BlobMetadata = {
  blobId: string,
  offset: number,
  size: number,
  name?: string,
  type?: string,
  lastModified?: number,
};

export type BlobPart = {
  type: string,
  data: string | BlobMetadata,
}