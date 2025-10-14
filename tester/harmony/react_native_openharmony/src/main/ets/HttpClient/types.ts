/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import http from '@ohos.net.http'

export interface RequestInterceptor {
  shouldIntercept(url: string, requestOptions: RequestOptions): boolean

  intercept(url: string, requestOptions: RequestOptions): {
    url: string,
    requestOptions: RequestOptions
  }
}

export interface ResponseInterceptor {
  shouldIntercept(url: string, response: HttpResponse): boolean

  intercept(url: string, response: HttpResponse): HttpResponse

}

export type ReceivingProgress = {
  totalLength: number,
  lengthReceived: number,
  bitsReceived: ArrayBuffer
}

export type SendingProgress = {
  totalLength: number,
  lengthSent: number,
}

export interface HttpResponse {
  statusCode: number,
  headers: object,
  body: string | ArrayBuffer,
}

export interface HttpErrorResponse {
  statusCode: number,
  error: Error
  timeout?: boolean
}

export type RequestOptions = http.HttpRequestOptions & {
  handleCookies?: boolean,
  timeout?: number //in ms, maximum time the whole request can take
  onReceiveProgress?: (receiveProgress: ReceivingProgress) => void,
  onSendProgress?: (sendProgress: SendingProgress) => void
}

export type CancelRequestCallback = () => void
