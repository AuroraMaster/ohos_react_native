/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { DisplayMode } from './CppBridgeUtils';
import type { Tag } from './DescriptorBase';
import type { NapiBridge } from './NapiBridge';
import type { RNInstance, SurfaceContext } from './RNInstance';
import { NodeContent } from '@ohos.arkui.node';
import { UIContext } from '@kit.ArkUI';

export type SurfaceProps = Record<string, any>;

function getConstraints(ctx: SurfaceContext): {
  minWidth: number
  minHeight: number
  maxWidth: number
  maxHeight: number
} {
  if ('width' in ctx) {
    return {
      minWidth: ctx.width,
      minHeight: ctx.height,
      maxWidth: ctx.width,
      maxHeight: ctx.height,
    }
  } else {
    return ctx
  }
}

export class SurfaceHandle {
  private destroyed: boolean = false;
  private displayMode: DisplayMode = DisplayMode.Suspended;
  private props: SurfaceProps
  private startingPromise: Promise<unknown> | undefined = undefined
  private running: boolean = false
  private surfaceCtx: SurfaceContext = null;

  constructor(
    private rnInstance: RNInstance,
    private tag: Tag,
    private appKey: string,
    private defaultProps: SurfaceProps,
    private napiBridge: NapiBridge,
    private onDestroyed: (surfaceHandle: SurfaceHandle) => void,
  ) {
    this.rnInstance.descriptorRegistry.createRootDescriptor(this.tag);
    this.napiBridge.createSurface(this.rnInstance.getId(), this.tag, this.appKey);
    this.props = defaultProps;
  }

  public getTag(): Tag {
    return this.tag;
  }

  public start(ctx: SurfaceContext, props: SurfaceProps) {
    if (this.destroyed) {
      throw new Error("start called on a destroyed surface");
    }

    this.surfaceCtx = ctx;
    this.props = { ...this.defaultProps, ...props };
    const {
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
    } = getConstraints(ctx);
    this.napiBridge.startSurface(
      this.rnInstance.getId(),
      this.tag,
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
      ctx.surfaceOffsetX,
      ctx.surfaceOffsetY,
      ctx.pixelRatio,
      ctx.isRTL,
      this.props);
    this.running = true
  }

  public async stop() {
    if (this.destroyed) {
      throw new Error("stop called on a destroyed surface");
    }
    await this.napiBridge.stopSurface(this.rnInstance.getId(), this.tag);
    this.running = false
  }

  public updateConstraints(
    ctx: SurfaceContext
  ) {
    if (this.destroyed) {
      throw new Error("updateConstraints called on a destroyed surface");
    }
    const {
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
    } = getConstraints(ctx);
    const {
      surfaceOffsetX,
      surfaceOffsetY,
      pixelRatio,
      isRTL,
    } = ctx;
    this.surfaceCtx = {
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
      surfaceOffsetX,
      surfaceOffsetY,
      isRTL,
      pixelRatio
    };

    this.napiBridge.updateSurfaceConstraints(
      this.rnInstance.getId(),
      this.tag,
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
      surfaceOffsetX,
      surfaceOffsetY,
      pixelRatio,
      isRTL,
    );
  }

  public updateRTL(isRTL: boolean) {
    const {
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
    } = getConstraints(this.surfaceCtx);
    const {
      surfaceOffsetX,
      surfaceOffsetY,
      pixelRatio,
    } = this.surfaceCtx;

    this.surfaceCtx = {
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
      surfaceOffsetX,
      surfaceOffsetY,
      isRTL,
      pixelRatio
    };

    this.napiBridge.updateSurfaceConstraints(
      this.rnInstance.getId(),
      this.tag,
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
      surfaceOffsetX,
      surfaceOffsetY,
      pixelRatio,
      isRTL,
    );
  }

  measure(ctx: SurfaceContext) {
    const {
      minWidth,
      minHeight,
      maxWidth,
      maxHeight,
    } = getConstraints(ctx);
    const {
      surfaceOffsetX,
      surfaceOffsetY,
      pixelRatio,
      isRTL,
    } = ctx
    return this.napiBridge.measureSurface(this.rnInstance.getId(), this.tag, minWidth, minHeight, maxWidth, maxHeight,
      surfaceOffsetX,
      surfaceOffsetY, pixelRatio, isRTL)
  }

  public getDisplayMode(): DisplayMode {
    return this.displayMode;
  }

  public setDisplayMode(displayMode: DisplayMode) {
    if (this.destroyed) {
      throw new Error("setDisplayMode called on a destroyed surface");
    }
    this.napiBridge.setSurfaceDisplayMode(this.rnInstance.getId(), this.tag, displayMode);
  }

  public getProps(): SurfaceProps {
    return this.props;
  }

  public setProps(props: SurfaceProps) {
    this.props = { ...this.defaultProps, ...props };
    this.napiBridge.setSurfaceProps(this.rnInstance.getId(), this.tag, this.props);
  }

  public destroy() {
    if (this.destroyed) {
      return;
    }
    if (this.running) {
      throw new Error("Surface must be stopped before can be destroyed")
    }
    this.napiBridge.destroySurface(this.rnInstance.getId(), this.tag);
    this.destroyed = true
    this.rnInstance.descriptorRegistry.deleteRootDescriptor(this.tag);
    this.onDestroyed(this);
  }

  public isRunning() {
    return this.running;
  }

  public isDestroyed() {
    return this.destroyed;
  }

  public attachRootView(instanceId: number, surfaceId: number, nodeContent: NodeContent) {
    return this.napiBridge.attachRootView(instanceId, surfaceId, nodeContent);
  }

  public detachRootView(instanceId: number, surfaceId: number) {
    return this.napiBridge.detachRootView(instanceId, surfaceId);
  }

    public setUIContext(instanceId: number, context: UIContext) {
    return this.napiBridge.setUIContext(instanceId, context);
  }
}