/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

export class NavigationManager {
  private static sInstance: NavigationManager = new NavigationManager();
  pathStack: NavPathStack | undefined;

  static getInstance() {
    return NavigationManager.sInstance;
  }

  createNavPathStack(): NavPathStack {
    if (this.pathStack == undefined) {
      this.pathStack = new NavPathStack();
    }
    return this.pathStack;
  }

  pushDestinationByName(name: string, param?: Object, animated?: boolean): void {
    this.pathStack?.pushDestinationByName(name, param, animated);
  }

  popDestination(animated?: boolean): void {
    this.pathStack?.pop(animated);
  }
}