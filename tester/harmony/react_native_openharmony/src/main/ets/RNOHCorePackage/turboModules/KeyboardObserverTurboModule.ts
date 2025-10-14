/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import window from '@ohos.window';
import type { TurboModuleContext } from "../../RNOH/TurboModule";
import { TurboModule } from "../../RNOH/TurboModule";

declare function px2vp(px: number): number;

export class KeyboardObserverTurboModule extends TurboModule {
  public static readonly NAME = 'KeyboardObserver';
  private static keyboardHeight:number;
  private cleanUpCallbacks: (() => void)[] = [];

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    this.subscribeListeners();
  }

  private createKeyboardEvent(screenX: number, startScreenY: number, endScreenY: number, width: number, startHeight: number, endHeight: number)  {
    return {
      easing: 'keyboard',
      duration: 0,
      endCoordinates: {
        screenX: px2vp(screenX),
        screenY: px2vp(endScreenY),
        height: px2vp(endHeight),
        width: px2vp(width),
      },
      startCoordinates: {
        screenX: px2vp(screenX),
        screenY: px2vp(startScreenY),
        height: px2vp(startHeight),
        width: px2vp(width),
      }
    }
  }

  private async subscribeListeners() {
    const windowInstance = await this.ctx.rnInstance.getRNWindow();
    // using inputMethodEngine.on('keyboardShow') and .on('keyboardHide') would be preferable, but it doesn't work at the time of writing.
    const onWindowKeyboardHeightChange = async (keyboardHeight) => {
      if (keyboardHeight > 0) {
        const windowRect = windowInstance.getWindowProperties().windowRect;
        const keyboardAvoidArea = windowInstance.getWindowAvoidArea(window.AvoidAreaType.TYPE_KEYBOARD).bottomRect;
        KeyboardObserverTurboModule.keyboardHeight = keyboardAvoidArea.height;
        this.ctx.rnInstance.emitDeviceEvent('keyboardDidShow',
          this.createKeyboardEvent(keyboardAvoidArea.left, windowRect.height, keyboardAvoidArea.top, windowRect.width, 0, keyboardHeight));
        this.ctx.rnInstance.postMessageToCpp("KEYBOARD_VISIBLE", {keyboardHeight: px2vp(keyboardAvoidArea.height)});
      } else {
        const windowRect = windowInstance.getWindowProperties().windowRect;
        const keyboardAvoidArea = windowInstance.getWindowAvoidArea(window.AvoidAreaType.TYPE_KEYBOARD).bottomRect;
        this.ctx.rnInstance.emitDeviceEvent('keyboardDidHide',
          this.createKeyboardEvent(keyboardAvoidArea.left, windowRect.height-KeyboardObserverTurboModule.keyboardHeight, windowRect.height, windowRect.width, KeyboardObserverTurboModule.keyboardHeight, 0));
      }
    }

    windowInstance.on('keyboardHeightChange', onWindowKeyboardHeightChange);
    this.cleanUpCallbacks.push(() => {
      windowInstance.off("keyboardHeightChange", onWindowKeyboardHeightChange);
    });
  }

  __onDestroy__() {
    super.__onDestroy__();
    this.cleanUpCallbacks.forEach(cb => cb());
    this.cleanUpCallbacks = [];
  }
}

