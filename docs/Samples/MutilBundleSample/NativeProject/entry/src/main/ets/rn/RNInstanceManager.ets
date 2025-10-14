/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * @module RNInstanceManager RN实例管理器 一个 RN 页面对应一个  RNInstance
 * [1] 创建 RN 实例
 * [2] 获取 RN 实例
 * [3] 删除 RN 实例
 * [4] 预创建 RN 实例 + 预加载 jsBundle
 */
import { hilog } from '@kit.PerformanceAnalysisKit';
import router from '@ohos.router';
import {
  RNInstance,
  RNInstanceOptions,
  RNOHCoreContext,
  JSBundleProvider,
} from '@rnoh/react-native-openharmony';
import { createRNPackages } from '../RNPackagesFactory';
import { ObjectUtil } from './util';
import { arkTsComponentNames } from './CustomRNComponent';

// 通用 RN 实例配置
const CommonRnConfig: RNInstanceOptions = {
  createRNPackages,
  arkTsComponentNames,
  enableNDKTextMeasuring: true,
  enableBackgroundExecutor: false,
  enableCAPIArchitecture: true,
  enableDebugger: true
};

export class RNInstanceManager {
  // RNInstance 容器, 一个 RN 页面对应一个  RNInstance;
  private static instanceMap: Map<string, RNInstance> = new Map();

  public static getOrCreateRNInstance(page: string, options?: RNInstanceOptions): Promise<RNInstance> | undefined {
    const rnohCoreContext: RNOHCoreContext | undefined = AppStorage.get('RNOHCoreContext');
    if (!rnohCoreContext) {
      return;
    }
    if (RNInstanceManager.instanceMap.has(page)) {
      return Promise.resolve(RNInstanceManager.instanceMap.get(page) as RNInstance);
    }
    const option = ObjectUtil.Assign(CommonRnConfig, {
      backPressHandler: () => {
        // 根据自己的业务规则处理返回事件;
        if (router.getState().name === page) {
          router.back();
        }
      }
    })
    return rnohCoreContext.createAndRegisterRNInstance(option).then((instance: RNInstance) => {
      RNInstanceManager.instanceMap.set(page, instance);
      return instance;
    })
  }

  public static getInstance(page: string): RNInstance | undefined {
    return RNInstanceManager.instanceMap.get(page);
  }

  public static deleteInstance(page: string): boolean {
    if (RNInstanceManager.instanceMap.has(page)) {
      RNInstanceManager.instanceMap.delete(page);
      return true;
    }
    return false;
  }

  public static async prepareRN(page: string, provider: JSBundleProvider) {
    const rnInstance = await RNInstanceManager.getOrCreateRNInstance(page);
    await rnInstance?.runJSBundle(provider);
  }
}
