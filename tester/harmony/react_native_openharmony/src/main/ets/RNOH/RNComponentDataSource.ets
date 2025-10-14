/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { DescriptorWrapper, Tag } from "./DescriptorBase"
import { RNComponentContext } from './EtsRNOHContext';
import { RNOHLogger } from "./RNOHLogger"

export class RNComponentDataSource implements IDataSource {
  protected listeners: Set<DataChangeListener> = new Set()
  protected ctx: RNComponentContext
  protected tag: Tag
  protected descriptorWrapper: DescriptorWrapper | undefined = undefined
  protected cleanUpCallbacks: (() => void)[] = []
  protected logger: RNOHLogger

  public constructor(ctx: RNComponentContext, tag: Tag) {
    this.ctx = ctx
    this.tag = tag
    this.logger = ctx.logger.clone("RNComponentDataSource")
  }

  public registerDataChangeListener(listener: DataChangeListener): void {
    if (this.listeners.size === 0) {
      this.startListeningToDescriptorMutations();
    }
    this.listeners.add(listener);
  }

  public unregisterDataChangeListener(listener: DataChangeListener): void {
    this.listeners.delete(listener);
    if (this.listeners.size === 0) {
      this.stopListeningToDescriptorMutations();
    }
  }

  public totalCount(): number {
    return this.descriptorWrapper?.childrenTags.length ?? 0;
  }

  public getData(index: number): DescriptorWrapper {
    if (index < 0 || index >= this.descriptorWrapper!.childrenTags.length) {
      this.logger.fatal("Invalid childIndex")
    }
    const childrenTag = this.descriptorWrapper?.childrenTags[index]
    if (childrenTag === undefined) {
      this.logger.error("Couldn't find childrenTag")
      throw new Error("Couldn't find childrenTag")
    }
    const result = this.ctx.descriptorRegistry.findDescriptorWrapperByTag(childrenTag)
    if (!result) {
      this.logger.error("Couldn't find descriptorWrapper")
      throw new Error("Couldn't find descriptorWrapper")
    }
    return result
  }

  private startListeningToDescriptorMutations() {
    this.descriptorWrapper = this.ctx.descriptorRegistry.findDescriptorWrapperByTag(this.tag) ?? undefined

    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag, () => {
      this.descriptorWrapper = this.ctx.descriptorRegistry.findDescriptorWrapperByTag(this.tag) ?? undefined
    }))


    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorMutations(this.tag, (args) => {
      this.logger.debug(args.descriptorMutationType)

      switch (args.descriptorMutationType) {
        case "INSERT_CHILD":
          this.listeners.forEach(listener => listener.onDataAdd(args.childIndex))
          break;
        case "UPDATE_CHILD":
          this.listeners.forEach(listener => listener.onDataChange(args.childIndex))
          break;
        case "REMOVE_CHILD":
          this.listeners.forEach(listener => listener.onDataDelete(args.childIndex))
          break;
      }
    }))
  }

  private stopListeningToDescriptorMutations() {
    this.cleanUpCallbacks.forEach(cb => cb())
    this.descriptorWrapper = undefined
  }
}
