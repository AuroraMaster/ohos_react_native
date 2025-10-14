/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type common from '@ohos.app.ability.common';
import WindowUtils from '@ohos.window';
import display from '@ohos.display';
import { EventEmitter } from './EventEmitter';

export type SafeAreaInsets = {
  top: number,
  left: number,
  right: number,
  bottom: number
}

declare function px2vp(arg: number): number

export class SafeAreaInsetsProvider {
  private currentInsets: SafeAreaInsets = {
    top: 0,
    left: 0,
    right: 0,
    bottom: 0
  };

  public eventEmitter: EventEmitter<{ "SAFE_AREA_INSETS_CHANGE": [SafeAreaInsets] }> = new EventEmitter()

  public get safeAreaInsets() {
    return { ...this.currentInsets }
  }

  constructor(private uiAbilityContext: common.UIAbilityContext) {
    this.createSafeAreaInsets().then((insets) => this.updateInsets(insets));
    WindowUtils.getLastWindow(uiAbilityContext).then((window) => {
      window.on("avoidAreaChange", this.onSafeAreaChange.bind(this))
    })
  }

  async createSafeAreaInsets(): Promise<SafeAreaInsets> {
    const retrieveSafeAreaInsets = async () => {
      const win = await WindowUtils.getLastWindow(this.uiAbilityContext)
        const [displayCutoutInfo, systemAvoidArea, cutoutAvoidArea, navigationAvoidArea] = await Promise.all([
          display.getDefaultDisplaySync().getCutoutInfo(),
          win.getWindowAvoidArea(WindowUtils.AvoidAreaType.TYPE_SYSTEM),
          win.getWindowAvoidArea(WindowUtils.AvoidAreaType.TYPE_CUTOUT),
          win.getWindowAvoidArea(WindowUtils.AvoidAreaType.TYPE_NAVIGATION_INDICATOR),
        ])
        const waterfallAvoidArea: WindowUtils.AvoidArea = {
          visible: true,
          leftRect: displayCutoutInfo.waterfallDisplayAreaRects.left,
          rightRect: displayCutoutInfo.waterfallDisplayAreaRects.right,
          topRect: displayCutoutInfo.waterfallDisplayAreaRects.top,
          bottomRect: displayCutoutInfo.waterfallDisplayAreaRects.bottom
        }

        const avoidAreas = [cutoutAvoidArea, waterfallAvoidArea, systemAvoidArea, navigationAvoidArea]
        const insets = getSafeAreaInsetsFromAvoidAreas(avoidAreas, win.getWindowProperties().windowRect)
        return mapProps(insets, (val) => px2vp(val))
    }

    /**
     * SafeAreaView sometimes fails to obtain the width of the screen borders due to getLastWindow API throwing a BusinessError.
     * Retrying isn't a perfect solution but it resolves the issue for now.
     */
    try {
      return await retrieveSafeAreaInsets();
    } catch (e) {
      return await retrieveSafeAreaInsets();
    }
  }

  private updateInsets(insets: SafeAreaInsets) {
    this.currentInsets = insets;
    this.eventEmitter.emit("SAFE_AREA_INSETS_CHANGE", insets);
  }

  public onSafeAreaChange() {
    this.createSafeAreaInsets().then((insets) => this.updateInsets(insets))
  }
}

function getSafeAreaInsetsFromAvoidAreas(avoidAreas: WindowUtils.AvoidArea[], windowSize: {
  width: number,
  height: number
}): SafeAreaInsets {
  return avoidAreas.reduce((currentInsets, avoidArea) => {
    return {
      top: Math.max(currentInsets.top, avoidArea.topRect.height + avoidArea.topRect.top),
      left: Math.max(currentInsets.left, avoidArea.leftRect.width + avoidArea.leftRect.left),
      right: Math.max(currentInsets.right,
        avoidArea.rightRect.left > 0 ? windowSize.width - avoidArea.rightRect.left : 0),
      bottom: Math.max(currentInsets.bottom,
        avoidArea.bottomRect.top > 0 ? windowSize.height - avoidArea.bottomRect.top : 0),
    }
  }, {
    top: 0,
    left: 0,
    right: 0,
    bottom: 0
  })
}


function mapProps<TObj extends Record<string, any>>(obj: TObj,
  cb: <TKey extends keyof TObj>(value: TObj[TKey], key: TKey) => TObj[TKey]) {
  return Object.entries(obj).reduce((acc, [key, value]) => {
    acc[key as keyof TObj] = cb(value, key)
    return acc
  }, {} as Partial<TObj>) as TObj
}


