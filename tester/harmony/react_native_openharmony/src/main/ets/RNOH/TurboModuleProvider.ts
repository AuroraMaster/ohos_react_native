/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type {
  AnyThreadTurboModule,
  UITurboModule,
  WorkerTurboModule
} from './TurboModule';
import type { TurboModulesFactory } from './RNPackage';
import { RNOHLogger } from './RNOHLogger';

export class TurboModuleProvider<TTurboModule extends UITurboModule | WorkerTurboModule | AnyThreadTurboModule = UITurboModule> {
  private cachedTurboModuleByName: Record<string, TTurboModule> = {};
  private logger: RNOHLogger

  constructor(private turboModulesFactories: (TurboModulesFactory<TTurboModule, unknown>)[], logger: RNOHLogger) {
    this.logger = logger.clone("TurboModuleProvider");
  }

  getModule<T extends TTurboModule>(name: string): T {
    if (!(name in this.cachedTurboModuleByName)) {
      for (const tmFactory of this.turboModulesFactories) {
        if (tmFactory.hasTurboModule(name)) {
          this.cachedTurboModuleByName[name] = tmFactory.createTurboModule(name);
          if (this.cachedTurboModuleByName[name] === null) {
            throw new Error(`Couldn't create "${name}" Turbo Module`);
          }
        }
      }
    }
    return this.cachedTurboModuleByName[name] as T;
  }

  hasModule(name: string) {
    for (const tmFactory of this.turboModulesFactories) {
      if (tmFactory.hasTurboModule(name)) {
        return true;
      }
    }
    return false;
  }

  onDestroy() {
    Object.entries(this.cachedTurboModuleByName).forEach(([name, turboModule]) => {
      try {
        turboModule.__onDestroy__()
      } catch {
        this.logger.error("Error while cleaning up TurboModule " + name);
      }
    })
    for (const factory of this.turboModulesFactories) {
      factory.onDestroy();
    }
  }
}
