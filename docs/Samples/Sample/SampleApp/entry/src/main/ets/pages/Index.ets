/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  ResourceJSBundleProvider,
  RNOHCoreContext,
  RNInstance,
  LogBoxDialog,
  RNOHErrorDialog,
  LogBoxTurboModule,
  ComponentBuilderContext,
  RNComponentContext,
  RNOHContext,
  buildRNComponentForTag,
} from '@rnoh/react-native-openharmony';
import MultiHome from './MultiHome';
import { createRNPackages, LoadManager, buildCustomComponent } from '../rn';
import { ENABLE_CAPI_ARCHITECTURE } from '../rn';
import { arkTsComponentNames } from '../rn/LoadBundle';

@Entry()
@Component
struct Index {
  @StorageLink('RNOHCoreContext') rnohCoreContext: RNOHCoreContext | undefined = undefined;
  @State isBundleReady: boolean = false;
  private isMetroAvailable: boolean = false;
  private cleanUpCallbacks: (() => void)[] = [];
  private logBoxDialogController!: CustomDialogController;

  @Builder
  logBoxBuilder(_: ComponentBuilderContext) {
  }

  aboutToAppear() {
    if (!this.rnohCoreContext) {
      return;
    }
    this.loadMetroBundle()
  }

  aboutToDisappear() {
    LoadManager.resetLoadedData();
    this.cleanUpCallbacks.forEach(cleanUp => cleanUp());
  }

  subscribeReload() {
    if (!this.rnohCoreContext) {
      return;
    }
    this.cleanUpCallbacks.forEach(cleanUp => cleanUp());
    this.cleanUpCallbacks.push(this.rnohCoreContext!.devToolsController.eventEmitter.subscribe("RELOAD", async () => {
      this.isBundleReady = false;
      this.isMetroAvailable = false;
      LoadManager.resetLoadedData();
      this.loadMetroBundle();
    }))
  }

  subscribeLogBox() {
    this.logBoxDialogController = new CustomDialogController({
      cornerRadius: 0,
      customStyle: true,
      alignment: DialogAlignment.TopStart,
      backgroundColor: Color.Transparent,
      builder: LogBoxDialog({
        ctx: new RNComponentContext(
          RNOHContext.fromCoreContext(this.rnohCoreContext!, LoadManager.metroInstance),
          wrapBuilder(buildCustomComponent),
          wrapBuilder(buildRNComponentForTag),
          new Map()
        ),
        rnInstance: LoadManager.metroInstance,
        initialProps: {},
        buildCustomComponent: this.logBoxBuilder,
      })
    })

    this.cleanUpCallbacks.push(LoadManager.metroInstance.getTurboModule<LogBoxTurboModule>(LogBoxTurboModule.NAME).eventEmitter.subscribe("SHOW",
      () => {
        this.logBoxDialogController.open();
      }))
    this.cleanUpCallbacks.push(LoadManager.metroInstance.getTurboModule<LogBoxTurboModule>(LogBoxTurboModule.NAME).eventEmitter.subscribe("HIDE",
      () => {
        this.logBoxDialogController.close();
      }))
  }

  loadMetroBundle() {
    LoadManager.loadMetroBundle(this.getUIContext()).then((flag: boolean) => {
      console.log('SampleApp --- metro 服务是否可用 = ', JSON.stringify(flag));
      AppStorage.setOrCreate('isMetroAvailable', flag);
      this.isMetroAvailable = flag;

      this.register().then((instanceMap: Map<string, RNInstance>) => {
        this.isBundleReady = true;
        console.log("registered instance", JSON.stringify(instanceMap));
      });
    })
  }

  async register(): Promise<Map<string, RNInstance>> {
    if (!this.rnohCoreContext) {
      return new Map();
    }

    if (this.isMetroAvailable) {
      this.isBundleReady = true;
      this.subscribeReload();
      this.subscribeLogBox();
      return new Map();
    }

    let instanceMap: Map<string, RNInstance> = new Map();
    const cpInstance: RNInstance = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: ENABLE_CAPI_ARCHITECTURE,
      arkTsComponentNames: arkTsComponentNames
    });
    const ctxCp: RNComponentContext = new RNComponentContext(
      RNOHContext.fromCoreContext(this.rnohCoreContext!, cpInstance),
      wrapBuilder(buildCustomComponent),
      wrapBuilder(buildRNComponentForTag),
      new Map()
    );
    const bpInstance: RNInstance = await this.rnohCoreContext.createAndRegisterRNInstance({
      createRNPackages: createRNPackages,
      enableNDKTextMeasuring: true,
      enableBackgroundExecutor: false,
      enableCAPIArchitecture: ENABLE_CAPI_ARCHITECTURE,
      arkTsComponentNames: arkTsComponentNames
    });
    const ctxBp: RNComponentContext = new RNComponentContext(
      RNOHContext.fromCoreContext(this.rnohCoreContext!, bpInstance),
      wrapBuilder(buildCustomComponent),
      wrapBuilder(buildRNComponentForTag),
      new Map()
    );
    LoadManager.cpInstance = cpInstance;
    LoadManager.bpInstance = bpInstance;
    await cpInstance.runJSBundle(new ResourceJSBundleProvider(getContext()
      .resourceManager, 'bundle/basic/basic.harmony.bundle'));
    instanceMap.set('CPReactInstance', cpInstance);
    instanceMap.set('BPReactInstance', bpInstance);
    return instanceMap;
  }

  build() {
    Stack() {
      if (this.rnohCoreContext?.isDebugModeEnabled && this.isMetroAvailable) {
        RNOHErrorDialog();
      }
      if (this.isBundleReady) {
        MultiHome();
      }
    }
  }
}