/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNInstance } from './RNInstance';
import { HttpClient, DefaultHttpClient } from '../HttpClient/HttpClient';

/**
 * @deprecated: Create defaultHttpClient in `RNAbility::onCreateDefaultHttpClient` or provide a httpClient in RNInstanceOptions (latestRNOHVersion: 0.72.27)
 */
export interface HttpClientProvider {
  getInstance(rnInstance: RNInstance): HttpClient;
}

/**
 * @deprecated: Check HttpClientProvider deprecation comment for more info (latestRNOHVersion: 0.72.27)
 */
export class DefaultHttpClientProvider implements HttpClientProvider {
  private static instance: HttpClient;

  public getInstance(rnInstance: RNInstance): HttpClient {
    if (!DefaultHttpClientProvider.instance) {
      DefaultHttpClientProvider.instance = new DefaultHttpClient();
    }

    return DefaultHttpClientProvider.instance;
  }
}