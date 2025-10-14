/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  AnyJSBundleProvider,
  MetroJSBundleProvider,
  ResourceJSBundleProvider,
  RNInstance,
  RNOHCoreContext,
  SurfaceHandle,
  Tag
} from 'rnoh/ts';
import { RNAbility } from 'rnoh';
import { createRNPackages } from '../RNPackagesFactory';


export default class EntryAbility extends RNAbility {
  protected async onPrepareRNInstance(rnohCoreContext: RNOHCoreContext): Promise<void> {
    super.onPrepareRNInstance(rnohCoreContext)
    const rnInstance: RNInstance = await rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages,
      enableCAPIArchitecture: true,
      backPressHandler: () => {
      },
      name: "app_name"
    })
    rnInstance.runJSBundle(new AnyJSBundleProvider([new MetroJSBundleProvider(),
      new ResourceJSBundleProvider(this.context.resourceManager, 'bundle.harmony.js')]))
    AppStorage.setOrCreate("AppRNInstance", rnInstance);

    const surfaceByAppKey: Record<string, SurfaceHandle> = {};
    AppStorage.setOrCreate("ReactSurfaceByAppKey", surfaceByAppKey);
  }

  protected getPagePath() {
    return 'pages/Index';
  }

  getRNOHWorkerScriptUrl() {
    return "multi_surface/ets/workers/RNOHWorker.ets"
  }
}
