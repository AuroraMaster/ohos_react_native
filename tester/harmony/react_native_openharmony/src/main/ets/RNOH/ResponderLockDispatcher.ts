/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Tag } from "./DescriptorBase"
import { ComponentManagerRegistry } from "./ComponentManagerRegistry"
import { RNComponentCommandHub, RNOHComponentCommand } from "./RNComponentCommandHub"
import { RNOHLogger } from "./RNOHLogger"

type Origin = string

export class ResponderLockDispatcher {
  /**
   * Native responders can be blocked by React Native or by third-party packages like react-native-gesture-handler.
   * We need to keep locks by origin, because React Native can in some situations not release the lock for one component -
   * - that's why we use "boolean" instead of "number".
   */
  private isLockedByOriginByTag: Map<Tag, Map<Origin, boolean>> = new Map()
  private logger: RNOHLogger

  constructor(private componentManagerRegistry: ComponentManagerRegistry, private componentCommandHub: RNComponentCommandHub, logger: RNOHLogger) {
    this.logger = logger.clone("ResponderLockDispatcher")
  }

  public onBlockResponder(tag: Tag, origin: Origin) {
    const tags = this.componentManagerRegistry.getComponentManagerLineage(tag).map(d => d.getTag())
    tags.forEach((tag) => {
      const currentNumberOfLocks = this.getTotalNumberOfLocks(tag)
      if (currentNumberOfLocks === 0) {
        this.componentCommandHub.dispatchCommand(tag, RNOHComponentCommand.BLOCK_NATIVE_RESPONDER, undefined)
      }
      if (!this.isLockedByOriginByTag.has(tag)) {
        this.isLockedByOriginByTag.set(tag, new Map())
      }
      this.isLockedByOriginByTag.get(tag).set(origin, true)
    })
  }

  private getTotalNumberOfLocks(tag: Tag): number {
    const isLockedByOrigin = this.isLockedByOriginByTag.get(tag)
    if (!isLockedByOrigin) {
      return 0
    }
    return Array.from(isLockedByOrigin.values()).filter(isLocked => isLocked).length
  }

  public onUnblockResponder(origin: Origin) {
    for (const tag of this.isLockedByOriginByTag.keys()) {
      const currentNumberOfLocks = this.getTotalNumberOfLocks(tag)
      if (currentNumberOfLocks === 0) {
        continue;
      }
      this.isLockedByOriginByTag.get(tag).set(origin, false)
      const newNumberOfLocks = this.getTotalNumberOfLocks(tag)
      if (newNumberOfLocks === 0) {
        this.componentCommandHub.dispatchCommand(tag, RNOHComponentCommand.UNBLOCK_NATIVE_RESPONDER, undefined)
        this.isLockedByOriginByTag.delete(tag)
      }
    }
  }
}