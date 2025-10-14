/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import type { TurboModuleContext } from '../../RNOH/TurboModule';
import { TurboModule } from '../../RNOH/TurboModule';
import { RNOHLogger } from '../../RNOH/RNOHLogger';
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';

const ACTION_SHARED = 'sharedAction';

export class ShareTurboModule extends TurboModule {
  public static readonly NAME = 'ShareModule';

  private logger: RNOHLogger;

  constructor(protected ctx: TurboModuleContext) {
    super(ctx);
    this.logger = this.ctx.logger.clone('ShareTurboModule');
  }

  async share(
    content: { title?: string; message?: string; url?: string },
    dialogTitle?: string,
  ): Promise<{ action: string }> {
    if (!content) {
      this.logger.error('Content is null');
      return Promise.reject('Content cannot be null');
    }

    let sharedData: systemShare.SharedData = new systemShare.SharedData(undefined);
    let sharedRecord: systemShare.SharedRecord;
    if (content.message) {
      sharedRecord = {
        utd: utd.UniformDataType.PLAIN_TEXT,
        content: content.message,
        title: content.title
      }
      sharedData.addRecord(sharedRecord);
    }
    if (content.url) {
      sharedRecord = {
        utd: utd.UniformDataType.HYPERLINK,
        content: content.url,
        title: content.title
      }
      sharedData.addRecord(sharedRecord);
    }

    if (sharedData.getRecords().length == 0) {
      this.logger.error('Invalid content: either message or url must be provided');
      return Promise.reject('Either message or url must be provided');
    }

    try {
      const controller: systemShare.ShareController = new systemShare.ShareController(sharedData);
      const context: common.UIAbilityContext = this.ctx.uiAbilityContext as common.UIAbilityContext;
      controller.on('dismiss', () => {
        this.logger.info('Share panel closed');
      })
      await controller.show(context, {
        previewMode: systemShare.SharePreviewMode.DETAIL,
        selectionMode: systemShare.SelectionMode.BATCH
      });
    } catch (error) {
      this.logger.error('Open share dialog error: ' + error);
      return Promise.reject('Failed to open share dialog');
    }
    return Promise.resolve({ action: ACTION_SHARED });
  }
}
