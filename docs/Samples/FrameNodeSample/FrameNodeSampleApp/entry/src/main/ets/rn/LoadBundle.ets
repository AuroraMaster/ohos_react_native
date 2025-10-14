/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  ResourceJSBundleProvider,
  RNInstance,
  RNOHCoreContext,
  RNOHContext,
  RNComponentContext,
  MetroJSBundleProvider,
  ComponentBuilderContext,
  buildRNComponentForTag,
} from '@rnoh/react-native-openharmony';
import { createRNPackages } from './RNPackagesFactory';
import { ENABLE_CAPI_ARCHITECTURE } from './config';

export const arkTsComponentNames = [];

@Builder
export function buildCustomComponent(ctx: ComponentBuilderContext) {
  Stack() {
  }
  .position({ x: 0, y: 0 })
}

export class LoadManager {
  private static loadedBundle: Set<string> = new Set();
  public static metroInstance: RNInstance;
  public static cpInstance: RNInstance;
  public static bpInstance: RNInstance;
  private static shouldResetMetroInstance: Boolean = true;
  static ctx: RNComponentContext;

  public static loadBundle(instance: RNInstance, bundlePath: string, useBundleCache: boolean = true): Promise<void> {
    if (LoadManager.loadedBundle.has(bundlePath) && useBundleCache) {
      return Promise.resolve();
    }
    if (useBundleCache) {
      LoadManager.loadedBundle.add(bundlePath);
    }
    return instance.runJSBundle(new ResourceJSBundleProvider(getContext().resourceManager, bundlePath));
  }

  public static async loadMetroBundle(uiContext: UIContext): Promise<boolean> {
    const rnohCoreContext: RNOHCoreContext | undefined = AppStorage.get('RNOHCoreContext');
    if (LoadManager.shouldResetMetroInstance && rnohCoreContext) {
      LoadManager.metroInstance = await rnohCoreContext.createAndRegisterRNInstance({
        createRNPackages: createRNPackages,
        enableNDKTextMeasuring: true,
        enableBackgroundExecutor: false,
        enableCAPIArchitecture: ENABLE_CAPI_ARCHITECTURE,
        arkTsComponentNames: arkTsComponentNames
      }
      );
      LoadManager.ctx = new RNComponentContext(
        RNOHContext.fromCoreContext(rnohCoreContext!, LoadManager.metroInstance),
        wrapBuilder(buildCustomComponent),
        wrapBuilder(buildRNComponentForTag),
        new Map()
      );

      const provider = new MetroJSBundleProvider();
      await LoadManager.metroInstance.runJSBundle(provider);

      const jsBundleExecutionStatus: string =
        LoadManager.metroInstance.getBundleExecutionStatus(provider.getURL()) as string;
      LoadManager.shouldResetMetroInstance = false;
      if (jsBundleExecutionStatus === "DONE") {
        return true;
      } else {
        return false;
      }
    }
    return true;
  }

  public static resetLoadedData() {
    LoadManager.loadedBundle.clear();
    LoadManager.shouldResetMetroInstance = true;
    const rnohCoreContext: RNOHCoreContext | undefined = AppStorage.get('RNOHCoreContext');
  }
}
