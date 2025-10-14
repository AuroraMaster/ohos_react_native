/**
 * MIT License
 * 
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * 
 * This code is licensed under the MIT License.
 * Please see the LICENSE-MIT file for more information.
 */
/**
* This file location relative to the package root shouldn't change to avoid introducing breaking changes.
*/
import { FatalRNOHError } from "./RNOHError"
import { UITurboModuleContext } from "./RNOHContext"
import { EtsUITurboModuleContext } from "./EtsRNOHContext"
import { UITurboModule } from '.'


/**
 * @thread: MAIN
 */
export class EtsUITurboModule extends UITurboModule {
  protected ctx: EtsUITurboModuleContext

  constructor(ctx: UITurboModuleContext) {
    super(ctx)
    if (!(ctx instanceof EtsUITurboModuleContext)) {
      throw new FatalRNOHError({
        whatHappened: "Received context not compatible with EtsUITurboModule",
        howCanItBeFixed: []
      });
    }
    this.ctx = ctx as EtsUITurboModuleContext;
  }
}