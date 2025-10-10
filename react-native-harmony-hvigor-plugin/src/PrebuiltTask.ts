/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNOHModulePluginOptions, RNOHModulePluginError, ILogger } from './types';
import { SyncTask } from './tasks/sync/SyncTask';
import { CommandExecutor } from './common/CommandExecutor';

export type { CodegenConfig, MetroConfig, AutolinkingConfig, FS } from './types';
export { ValidationError } from './tasks/sync/SyncTask';

/**
 * @api
 * @deprecated Use `RNOHModulePluginOptions` instead.
 */
export type RNOHHvigorPluginOptions = RNOHModulePluginOptions;

/**
 * @api
 * @deprecated Use `RNOHModulePluginError` instead.
 */
export class RNOHHvigorPluginError extends RNOHModulePluginError {}

/**
 * @deprecated Use `CommandExecutor` instead.
 */
export class CliExecutor extends CommandExecutor {}

/**
 * @deprecated Use `ILogger` instead.
 */
export interface Logger extends ILogger {};

/**
 * @deprecated Use `SyncTask` instead.
 */
export class PrebuiltTask extends SyncTask {}
