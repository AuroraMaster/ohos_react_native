/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  RNOHCoreContext,
  RNOHContext,
  SurfaceProps,
  JSBundleProvider,
  ResourceJSBundleProvider,
  RNSurface,
  RNInstance,
  CustomRNComponentFrameNodeFactory,
  RNComponentContext,
  buildRNComponentForTag
} from "@rnoh/react-native-openharmony";
import { createRNPackages, buildCustomComponent, ENABLE_CAPI_ARCHITECTURE } from '../rn';
import ArrayList from '@ohos.util.ArrayList';
import { arkTsComponentNames } from './LoadBundle';

interface CachedInstance {
  rnInstance: RNInstance,
  status?: string
}

const wrappedCustomRNComponentBuilder = wrapBuilder(buildCustomComponent);

@Component
export struct BasePrecreateView {
  public appKey: string = 'app_name';
  public bundlePath: string = 'bundle.harmony.js';
  public initialProps: SurfaceProps = {};
  @StorageLink('rnInstances') rnInstances: ArrayList<CachedInstance> = new ArrayList();
  @StorageLink('RNOHCoreContext') private rnohCoreContext: RNOHCoreContext | undefined = undefined;
  @State private shouldShow: boolean = false;
  private rnInstance!: RNInstance;
  private shouldDestroyRNInstance: boolean = false;
  private cleanUpCallbacks: (() => void)[] = [];
  private jsBundleProvider: JSBundleProvider | undefined = undefined;

  aboutToAppear() {
    this.init();
    this.precreateInstance();
  }

  aboutToDisappear() {
    this.cleanUp();
  }

  private async init() {
    this.jsBundleProvider = new ResourceJSBundleProvider(getContext().resourceManager, this.bundlePath);
    console.log("BasePrecreateView Begin ======================================================= ");
    const instance = await this.getOrCreateRNInstance();
    this.rnInstance = instance.rnInstance;
    console.log("BasePrecreateView init RNInstance ID = ", this.rnInstance.getId());

    const jsBundleExecutionStatus = instance.status;
    if (this.jsBundleProvider && jsBundleExecutionStatus === undefined) {
      console.log("BasePrecreateView RNInstance 未预加载");
      await this.rnInstance.runJSBundle(this.jsBundleProvider);
      this.shouldShow = true;
      return;
    }
    if (jsBundleExecutionStatus !== "DONE") {
      this.cleanUpCallbacks.push(this.rnInstance.subscribeToLifecycleEvents("JS_BUNDLE_EXECUTION_FINISH", (args) => {
        this.shouldShow = true;
        console.log("BasePrecreateView 已等待预创建的 RNInstance 完成 JSBundle 加载，展示页面");
      }))
    } else {
      this.shouldShow = true;
      console.log("BasePrecreateView 预创建的 RNInstance 已完成 JSBundle 加载，直接展示页面");
    }
  }

  private async getOrCreateRNInstance(): Promise<CachedInstance> {
    // 获取上次 disappear 时预创建的 RNInstance
    this.shouldDestroyRNInstance = true;
    if (this.rnInstances.length > 0) {
      console.log("BasePrecreateView getOrCreateRNInstance 获取预创建的 RNInstance：", this.rnInstances[0]);
      const instance = this.rnInstances.removeByIndex(0);
      console.log("BasePrecreateView status = ", instance.status);
      return Promise.resolve(instance);
    } else {
      const instance = await this.rnohCoreContext!.createAndRegisterRNInstance({
        createRNPackages: createRNPackages,
        enableNDKTextMeasuring: true,
        enableBackgroundExecutor: false,
        enableCAPIArchitecture: ENABLE_CAPI_ARCHITECTURE,
        arkTsComponentNames: arkTsComponentNames
      });
      const ctxInstance: RNComponentContext = new RNComponentContext(
        RNOHContext.fromCoreContext(this.rnohCoreContext!, instance),
        wrapBuilder(buildCustomComponent),
        wrapBuilder(buildRNComponentForTag),
        new Map()
      );
      return { rnInstance: instance };
    }
  }

  private async precreateInstance() {
    const instances: ArrayList<CachedInstance> = this.rnInstances;
    const rnohCoreContext: RNOHCoreContext = this.rnohCoreContext!;
    const provider: JSBundleProvider = this.jsBundleProvider!;
    // 预创建 RNInstance
    if (instances.length === 0) {
      const rnInstance = await rnohCoreContext.createAndRegisterRNInstance({
        createRNPackages: createRNPackages,
        enableNDKTextMeasuring: true,
        enableBackgroundExecutor: false,
        enableCAPIArchitecture: ENABLE_CAPI_ARCHITECTURE,
        arkTsComponentNames: arkTsComponentNames
      });
      const ctxRNInstance: RNComponentContext = new RNComponentContext(
        RNOHContext.fromCoreContext(this.rnohCoreContext!, rnInstance),
        wrapBuilder(buildCustomComponent),
        wrapBuilder(buildRNComponentForTag),
        new Map()
      );
      const instance: CachedInstance = {
        rnInstance: rnInstance,
        status: undefined
      }
      console.log("BasePrecreateView precreateInstance rnInstances = ", instances.length);
      console.log("BasePrecreateView precreateInstance RNInstance ID = ", rnInstance.getId());
      instances.add(instance);
      await rnInstance.runJSBundle(provider);
      instance.status = "DONE";

      console.log("BasePrecreateView status = ", instance.status);
      console.log("BasePrecreateView 预加载 JSBundle 完成，RNInstance ID = " + rnInstance.getId() +
        " instances.count = " + instances.length);
      console.log("BasePrecreateView End ======================================================= ");
    }
  }

  private async cleanUp() {
    this.shouldShow = false;
    if (this.shouldDestroyRNInstance) {
      this.rnohCoreContext!.destroyAndUnregisterRNInstance(this.rnInstance);
    }
    this.cleanUpCallbacks.forEach(cleanUp => cleanUp());
  }

  build() {
    Stack() {
      if (this.shouldShow) {
        RNSurface({
          ctx: new RNComponentContext(
            RNOHContext.fromCoreContext(this.rnohCoreContext!, this.rnInstance),
            wrappedCustomRNComponentBuilder,
            wrapBuilder(buildRNComponentForTag),
            new Map()
          ),
          surfaceConfig: {
            initialProps: this.initialProps,
            appKey: this.appKey,
          },
        })
      }
    }.width("100%")
    .height("100%")
  }
}
