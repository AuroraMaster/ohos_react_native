/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { TurboModule } from "../../RNOH/TurboModule";
import uri from '@ohos.uri';

export class SourceCodeTurboModule extends TurboModule {
  public static readonly NAME = 'SourceCode';

  getConstants(): { scriptURL: string | null; } {
    const bundleUrl = this.ctx.rnInstance.getInitialBundleUrl() ?? '';
    const parsedUri = new uri.URI(bundleUrl);
    const isResourceJSBundle = !parsedUri.scheme && bundleUrl.indexOf('/') !== 0;
    return { scriptURL: isResourceJSBundle ? `assets://${bundleUrl}` : bundleUrl };
  }
}