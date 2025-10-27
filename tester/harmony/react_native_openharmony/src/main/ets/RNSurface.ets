/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { RNComponentFactory, RNView } from './RNOHCorePackage';
import {
  ComponentBuilderContext,
  CustomRNComponentFrameNodeFactory,
  Descriptor,
  DisplayMode,
  RNComponentContext,
  RNInstance,
  RNInstanceImpl,
  RNOHContext,
  RNOHError,
  RNOHLogger
} from './RNOH';
import {
  ViewBaseProps,
  ViewDescriptorWrapper,
  ViewDescriptorWrapperBase,
  ViewRawProps
} from './RNOHCorePackage/components/RNViewBase';
import { TouchDispatcher } from './RNOH/TouchDispatcher';
import { SurfaceHandle, SurfaceProps } from './RNOH/SurfaceHandle';
import type { SurfaceContext } from './RNOH/RNInstance';
import { NodeContent } from '@ohos.arkui.node'
import I18n from '@ohos.i18n';

interface RootViewState {}

export type RootDescriptor = Descriptor<"RootView", ViewBaseProps, RootViewState, ViewRawProps>;

export interface SurfaceConfig {}

export interface SurfaceConfig1 extends SurfaceConfig {
  surfaceHandle: SurfaceHandle,
  isSurfaceContentAdaptive?: boolean
}

export interface SurfaceConfig2 extends SurfaceConfig {
  appKey: string,
  initialProps: SurfaceProps,
  isSurfaceContentAdaptive?: boolean
}

interface RootComponentDimensionsChangedPayload {
  surfaceId: number,
  width: number,
  height: number
}

interface Size {
  width: number
  height: number
}

/**
 * @api
 */
interface Constraints {
  minWidth?: number
  minHeight?: number
  maxWidth?: number
  maxHeight?: number
}

@Component
struct ContentAdaptiveSurface {
  /**
   * Required.
   */
  public ctx!: RNOHContext | RNComponentContext;
  /**
   * Required
   */
  public surfaceConfig!: SurfaceConfig;
  /**
   * Not Required,
   * Used to return the corresponding RNInstance and root tag for this 
   * specific RNSurface. 
   */
  public updateViewRootTag: ((rnInstance:RNInstance, rootViewTag: number) => void) | undefined;
  /**
   * Optional
   * If provided, those constraints will be used instead of those provided by ArkUI. Useful, if maxWidth or maxHeight
   * must be different than the default values with .width(...) and .height(...).
   *
   * NOTE: you may need to remove "flex: 1" style from AppContainer in your JS bundle to adjust RNSurface to its content.
   * In RN@0.72.5, this is not possible without patching RN code. RN@0.75.0 introduced a new method:
   * AppRegistry.setRootViewStyleProvider which should allow removing that flex without creating the patch.
   */
  public constraints: Constraints | undefined = undefined
  /**
   * Required if ctx isn't RNComponentContext
   * @deprecated: provide RNComponentContext to RNSurface::ctx instead
   */
  @BuilderParam buildCustomComponent: (ctx: ComponentBuilderContext) => void;
  // -------------------------------------------------------------------------------------------------------------------
  private surfaceOffsetX: number = 0;
  private surfaceOffsetY: number = 0;
  @State private rootComponentSize: Size | undefined = undefined;
  @State private shouldStartSurface: boolean = false;
  private surfaceHandle!: SurfaceHandle;
  private cleanUpCallbacks: (() => void)[] = [];
  private touchDispatcher!: TouchDispatcher;
  private shouldStopSurface: boolean = false;
  private surfaceContext: SurfaceContext | undefined = undefined;
  private initialProps!: SurfaceProps;
  private logger!: RNOHLogger
  private rootViewNodeContent: NodeContent = new NodeContent();
  private timerId: undefined | number = undefined;
  private isWindowRectDrag: boolean = false;
  private currentMeasuredSize: Size | undefined = undefined;
  /**
  * Note: You may need to change this delay along with RNInstancesCoordinator's 
  * onWindowSizeChange delay, as both have an effect on window changes. 
  */
  private readonly delayTime: number = 100;

  aboutToAppear() {
    this.logger = this.ctx.logger.clone("RNSurface")
    if (!(this.ctx instanceof RNComponentContext)) {
      this.ctx.reportRNOHError(new RNOHError({
        whatHappened: "RNSurface received RNOHContext instead of RNComponentContext. Custom RN components won't be visible.",
        howCanItBeFixed: ["Provide RNComponentContext instead of RNOHContext"]
      }))
    }
    const stopTracing = this.logger.clone("aboutToAppear").startTracing();
    (this.ctx.rnInstance as RNInstanceImpl).setUIContext(this.getUIContext());
    this.ctx.rnInstance.setFrameNodeFactory(new CustomRNComponentFrameNodeFactory(this.getUIContext(), this.ctx.logger,
      () => {
        return this.ctx as RNComponentContext
      }))
    if (Object.keys(this.surfaceConfig).includes("surfaceHandle")) {
      this.surfaceHandle = (this.surfaceConfig as SurfaceConfig1).surfaceHandle;
      this.surfaceHandle.setDisplayMode(DisplayMode.Visible);
    } else {
      this.surfaceHandle = this.ctx.rnInstance.createSurface((this.surfaceConfig as SurfaceConfig2).appKey);
      this.initialProps = (this.surfaceConfig as SurfaceConfig2).initialProps;
      this.shouldStartSurface = true;
      this.shouldStopSurface = true;
    }
    const tag = this.surfaceHandle.getTag();
    this.updateViewRootTag && this.updateViewRootTag(this.ctx.rnInstance, tag);
    this.touchDispatcher = new TouchDispatcher(tag, this.ctx.rnInstance, this.ctx.logger)
    this.cleanUpCallbacks.push(this.ctx.rnInstance.cppEventEmitter.subscribe("RNOH::ROOT_COMPONENT_DIMENSIONS_CHANGED",
      (payload: RootComponentDimensionsChangedPayload) => {
        if (payload.surfaceId === this.surfaceHandle.getTag()) {
          this.rootComponentSize = { width: payload.width, height: payload.height }
        }
      }))
    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.subscribeToLifecycleEvents("WINDOW_REACT_DRAG", 
      (isWindowRectDrag: boolean) => {
        this.isWindowRectDrag = isWindowRectDrag;
      })
    )
    this.surfaceHandle.attachRootView(this.ctx.rnInstance.getId(), this.surfaceHandle.getTag(),
      this.rootViewNodeContent);
    this.surfaceHandle.setUIContext(this.ctx.rnInstance.getId(), this.getUIContext());
    stopTracing()
  }

  aboutToDisappear() {
    const surfaceHandle = this.surfaceHandle;
    if (this.timerId) {
      clearTimeout(this.timerId);
      this.timerId = undefined;
    }
    (async () => {
      const stopTracing = this.logger.clone("aboutToDisappear").startTracing()
      this.cleanUpCallbacks.forEach(cb => cb())
      if (this.shouldStopSurface) {
        if (!surfaceHandle.isDestroyed()) {
          // NOTE: when the app is stopping, it's possible the ability cleaned up the surface
          // before us. This is fine.
          await surfaceHandle.stop();
          surfaceHandle.destroy();
        }
      } else if (surfaceHandle.getDisplayMode() === DisplayMode.Visible) {
        surfaceHandle.setDisplayMode(DisplayMode.Suspended);
      }
      this.surfaceHandle.detachRootView(this.ctx.rnInstance.getId(), this.surfaceHandle.getTag())
      stopTracing()
    })()

  }

  updateSurfaceConstraints(constraints: Constraints): Size {
    const isRTL = I18n.isRTL(this.ctx.uiAbilityContext.config.language)
    this.surfaceContext = {
      minWidth: this.constraints?.minWidth ?? constraints?.minWidth ?? 0,
      minHeight: this.constraints?.minHeight ?? constraints?.minHeight ?? 0,
      maxWidth: this.constraints?.maxWidth ?? constraints?.maxWidth ?? 0,
      maxHeight: this.constraints?.maxHeight ?? constraints?.maxHeight ?? 0,
      surfaceOffsetX: this.surfaceOffsetX,
      surfaceOffsetY: this.surfaceOffsetY,
      pixelRatio: vp2px(1),
      isRTL,
    }
    if (this.shouldStartSurface) {
      this.surfaceHandle.setDisplayMode(DisplayMode.Visible)
      this.surfaceHandle.start(this.surfaceContext, this.initialProps)
      this.shouldStartSurface = false
    } else {
      this.surfaceHandle.updateConstraints(this.surfaceContext);
    }
    return this.surfaceHandle.measure(this.surfaceContext)
  }

  override onMeasureSize(selfLayoutInfo: GeometryInfo, children: Array<Measurable>,
    constraint: ConstraintSizeOptions): SizeResult {
    if (this.surfaceHandle.isDestroyed()) {
      return {width: 0, height: 0};
    }
    if (!this.isWindowRectDrag) {
      this.currentMeasuredSize =
        this.updateSurfaceConstraints({
          minWidth: constraint.maxWidth as number, // NOTE: width: 100% doesn't work on the root component hence maxWidth is used here
          minHeight: constraint.minHeight as number,
          maxWidth: constraint.maxWidth as number,
          maxHeight: constraint.maxHeight as number,
        });
    } else {
      clearTimeout(this.timerId);
      this.timerId = setTimeout(() =>{
        this.currentMeasuredSize =
          this.updateSurfaceConstraints({
            minWidth: constraint.maxWidth as number, // NOTE: width: 100% doesn't work on the root component hence maxWidth is used here
            minHeight: constraint.minHeight as number,
            maxWidth: constraint.maxWidth as number,
            maxHeight: constraint.maxHeight as number,
          });
      }, this.delayTime);
    }
    const measuredSize = this.currentMeasuredSize;
    const newSize: Size =
      { width: measuredSize?.width ?? selfLayoutInfo.width, height: measuredSize?.height ?? selfLayoutInfo.height }
    children.forEach((child) => {
      child.measure({
        minHeight: newSize.height,
        minWidth: newSize.width,
        maxWidth: newSize.width,
        maxHeight: newSize.height
      });
    });
    return newSize;
  }

  override onPlaceChildren(selfLayoutInfo: GeometryInfo, children: Array<Layoutable>,
    constraint: ConstraintSizeOptions) {
    // If this method is not overridden, child components aren't visible.
    children.forEach((child) => {
      child.layout({ x: 0, y: 0 })
    })
  }

  handleTouch(event: TouchEvent) {
    if (this.ctx.rnInstance.getArchitecture() === "ARK_TS") {
      this.logger.clone("handleTouch").debug()
      this.touchDispatcher.handleTouchEvent(event)
    }
  }

  build() {
    Stack() {
      if (this.ctx.rnInstance.getArchitecture() === "C_API") {
        ContentSlot(this.rootViewNodeContent)
      } else {
        if (this.ctx instanceof RNComponentContext) {
          RNView({
            ctx: this.ctx,
            tag: this.surfaceHandle.getTag()
          }) //  NOTE: This is wrong. RNOH should allow replacing RootComponent.
        } else {
          RNComponentFactory({
            ctx: this.ctx,
            tag: this.surfaceHandle.getTag(),
            buildCustomComponent: this.buildCustomComponent
          })
        }
      }
    }
    /**
     * width and height are used here only to trigger layout calculations. Actual values are set in onMeasureSize method.
     */
    .width(this.rootComponentSize?.width)
    .height(this.rootComponentSize?.height)
    .onAreaChange((oldArea, newArea) => {
      this.surfaceOffsetX = newArea.globalPosition.x as number
      this.surfaceOffsetY = newArea.globalPosition.y as number
      this.surfaceContext!.surfaceOffsetX = this.surfaceOffsetX
      this.surfaceContext!.surfaceOffsetY = this.surfaceOffsetY
      this.surfaceHandle.updateConstraints(this.surfaceContext);
    })
    .onTouch((e) => this.handleTouch(e))
  }
}

@Component
struct ParentBoundSurface {
  /**
   * Required.
   */
  public ctx!: RNOHContext | RNComponentContext;
  /**
   * Required
   */
  public surfaceConfig!: SurfaceConfig;
  /**
   * Not Required,
   * Used to return the corresponding RNInstance and root tag for this 
   * specific RNSurface. 
   */
  public updateViewRootTag: ((rnInstance:RNInstance, rootViewTag: number) => void) | undefined;
  /**
   * Required if ctx isn't RNComponentContext
   * @deprecated: provide RNComponentContext to RNSurface::ctx instead
   */
  @BuilderParam buildCustomComponent: (ctx: ComponentBuilderContext) => void;
  // -------------------------------------------------------------------------------------------------------------------
  private surfaceWidth: number = 0;
  private surfaceHeight: number = 0;
  private surfaceOffsetX: number = 0;
  private surfaceOffsetY: number = 0;
  @State private descriptor: RootDescriptor = Object() as RootDescriptor;	
  @State private descriptorWrapper: ViewDescriptorWrapperBase | undefined = undefined
  @State private shouldStartSurface: boolean = false;
  private surfaceHandle!: SurfaceHandle;
  private cleanUpCallbacks: (() => void)[] = [];
  private touchDispatcher!: TouchDispatcher;
  private shouldStopSurface: boolean = false;
  private initialProps!: SurfaceProps;
  private logger!: RNOHLogger
  private rootViewNodeContent: NodeContent = new NodeContent();
  private timerId: undefined | number = undefined;
  /**
  * Note: You may need to change this delay along with RNInstancesCoordinator's 
  * onWindowSizeChange delay, as both have an effect on window changes. 
  */
  private readonly delayTime: number = 100;

  aboutToAppear() {
    this.logger = this.ctx.logger.clone("RNSurface")
    if (!(this.ctx instanceof RNComponentContext)) {
      this.ctx.reportRNOHError(new RNOHError({
        whatHappened: "RNSurface received RNOHContext instead of RNComponentContext. Custom RN components won't be visible.",
        howCanItBeFixed: ["Provide RNComponentContext instead of RNOHContext"]
      }))
    }
    const stopTracing = this.logger.clone("aboutToAppear").startTracing();
    (this.ctx.rnInstance as RNInstanceImpl).setUIContext(this.getUIContext());
    this.ctx.rnInstance.setFrameNodeFactory(new CustomRNComponentFrameNodeFactory(this.getUIContext(), this.ctx.logger,
      () => {
        return this.ctx as RNComponentContext
      }))
    if (Object.keys(this.surfaceConfig).includes("surfaceHandle")) {
      this.surfaceHandle = (this.surfaceConfig as SurfaceConfig1).surfaceHandle;
      this.surfaceHandle.setDisplayMode(DisplayMode.Visible);
    } else {
      this.surfaceHandle = this.ctx.rnInstance.createSurface((this.surfaceConfig as SurfaceConfig2).appKey);
      this.initialProps = (this.surfaceConfig as SurfaceConfig2).initialProps;
      this.shouldStartSurface = true;
      this.shouldStopSurface = true;
    }
    const tag = this.surfaceHandle.getTag();
    this.updateViewRootTag && this.updateViewRootTag(this.ctx.rnInstance, tag);
    this.onUpdateComponent(this.ctx.rnInstance.descriptorRegistry.getDescriptor(tag))
    this.touchDispatcher = new TouchDispatcher(tag, this.ctx.rnInstance, this.ctx.logger)
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(	
      tag,	
      (newDescriptor) => this.onUpdateComponent(newDescriptor)	
    ))
    this.surfaceHandle.attachRootView(this.ctx.rnInstance.getId(), this.surfaceHandle.getTag(),
      this.rootViewNodeContent);
    stopTracing()
  }

  aboutToDisappear() {
    const surfaceHandle = this.surfaceHandle;
    if (this.timerId) {
      clearTimeout(this.timerId);
      this.timerId = undefined;
    }
    (async () => {
      const stopTracing = this.logger.clone("aboutToDisappear").startTracing()
      this.cleanUpCallbacks.forEach(cb => cb())
      if (this.shouldStopSurface) {
        if (!surfaceHandle.isDestroyed()) {
          // NOTE: when the app is stopping, it's possible the ability cleaned up the surface
          // before us. This is fine.
          await surfaceHandle.stop();
          surfaceHandle.destroy();
        }
      } else if (surfaceHandle.getDisplayMode() === DisplayMode.Visible) {
        surfaceHandle.setDisplayMode(DisplayMode.Suspended);
      }
      this.surfaceHandle.detachRootView(this.ctx.rnInstance.getId(), this.surfaceHandle.getTag())
      stopTracing()
    })()

  }

  onUpdateComponent(newDescriptor: Descriptor) {	
    this.descriptor = newDescriptor as RootDescriptor	
    this.descriptorWrapper = new ViewDescriptorWrapper(newDescriptor)
  }

  handleAreaChange(_oldValue: Area, newValue: Area) {
    const stopTracing = this.logger.clone("handleAreaChange").startTracing()
    this.surfaceWidth = newValue.width as number
    this.surfaceHeight = newValue.height as number
    this.surfaceOffsetX = newValue.globalPosition.x as number
    this.surfaceOffsetY = newValue.globalPosition.y as number
    this.updateSurfaceConstraints()
    stopTracing()
  }

  updateSurfaceConstraints() {
    const isRTL = I18n.isRTL(this.ctx.uiAbilityContext.config.language)
    if (this.shouldStartSurface) {
      this.surfaceHandle.setDisplayMode(DisplayMode.Visible)
      this.surfaceHandle.start(	
        {
          width: this.surfaceWidth,
          height: this.surfaceHeight,
          surfaceOffsetX: this.surfaceOffsetX,
          surfaceOffsetY: this.surfaceOffsetY,
          pixelRatio: vp2px(1),
          isRTL,
        },
        this.initialProps)
      this.shouldStartSurface = false
    } else {
      this.surfaceHandle.updateConstraints({	
        width: this.surfaceWidth,
        height: this.surfaceHeight,
        surfaceOffsetX: this.surfaceOffsetX,
        surfaceOffsetY: this.surfaceOffsetY,
        pixelRatio: vp2px(1),
        isRTL,
      });
    }
  }

  handleTouch(event: TouchEvent) {
    if (this.ctx.rnInstance.getArchitecture() === "ARK_TS") {
      this.logger.clone("handleTouch").debug()
      this.touchDispatcher.handleTouchEvent(event)
    }
  }

  build() {
    Stack() {
      if (this.ctx.rnInstance.getArchitecture() === "C_API") {
        ContentSlot(this.rootViewNodeContent)
      } else {
        if (this.ctx instanceof RNComponentContext) {
          RNView({
            ctx: this.ctx,
            tag: this.surfaceHandle.getTag()
          }) //  NOTE: This is wrong. RNOH should allow replacing RootComponent.
        } else {
          RNComponentFactory({
            ctx: this.ctx,
            tag: this.surfaceHandle.getTag(),
            buildCustomComponent: this.buildCustomComponent
          })
        }
      }
    }
    .width("100%")	
    .height("100%")	
    .onAreaChange((oldArea, newArea) => this.handleAreaChange(oldArea, newArea))
    .onTouch((e) => this.handleTouch(e))
  }
}


/**
 * @api: RN_APP_DEVELOPER
 */
@Component
export struct RNSurface {
  /**
   * Required.
   */
  public ctx!: RNOHContext | RNComponentContext;
  /**
   * Required
   */
  public surfaceConfig!: SurfaceConfig;
  /**
   * Not Required,
   * Used to return the corresponding RNInstance and root tag for this 
   * specific RNSurface. 
   */
  public updateViewRootTag: ((rnInstance:RNInstance, rootViewTag: number) => void) | undefined;
  /**
   * Optional
   * If provided, those constraints will be used instead of those provided by ArkUI. Useful, if maxWidth or maxHeight
   * must be different than the default values with .width(...) and .height(...).
   *
   * NOTE: you may need to remove "flex: 1" style from AppContainer in your JS bundle to adjust RNSurface to its content.
   * In RN@0.72.5, this is not possible without patching RN code. RN@0.75.0 introduced a new method:
   * AppRegistry.setRootViewStyleProvider which should allow removing that flex without creating the patch.
   */
  public constraints: Constraints | undefined = undefined
  /**
   * Required if ctx isn't RNComponentContext
   * @deprecated: provide RNComponentContext to RNSurface::ctx instead
   */
  @BuilderParam buildCustomComponent: (ctx: ComponentBuilderContext) => void
  // -------------------------------------------------------------------------------------------------------------------
  private isSurfaceContentAdaptive: boolean | undefined = undefined

  aboutToAppear() {
    if (Object.keys(this.surfaceConfig).includes("surfaceHandle")) {
      this.isSurfaceContentAdaptive = (this.surfaceConfig as SurfaceConfig1).isSurfaceContentAdaptive;
    } else {
      this.isSurfaceContentAdaptive = (this.surfaceConfig as SurfaceConfig2).isSurfaceContentAdaptive;
    }
  }

  build() {
    if (this.isSurfaceContentAdaptive == undefined ||
          this.isSurfaceContentAdaptive) {
      ContentAdaptiveSurface({
        ctx: this.ctx,
        surfaceConfig: this.surfaceConfig,
        updateViewRootTag: this.updateViewRootTag,
        constraints: this.constraints,
        buildCustomComponent: this.buildCustomComponent
      })
    } else {
      ParentBoundSurface({
        ctx: this.ctx,
        surfaceConfig: this.surfaceConfig,
        updateViewRootTag: this.updateViewRootTag,
        buildCustomComponent: this.buildCustomComponent
      })
    } 
  }
}

