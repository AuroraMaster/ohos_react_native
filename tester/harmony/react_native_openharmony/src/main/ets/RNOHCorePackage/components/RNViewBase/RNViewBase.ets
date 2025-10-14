/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { Corners, Descriptor, Edges, RNComponentContext, RNOHContext, RNOHError, TransformMatrix } from '../../../RNOH';
import { ViewBaseProps, ViewRawProps } from './types';
import { ViewDescriptorWrapper, ViewDescriptorWrapperBase } from './ViewDescriptorWrapper';
import { RNViewManager } from '../../componentManagers';
import { areFirstLevelElementsEqual, areFirstLevelPropsEqual } from './etsHacks';
import matrix4 from '@ohos.matrix4';
import { RN_PULL_TO_REFRESH_VIEW_NAME } from "../RNPullToRefreshView"

export interface ViewBaseState {}

export type ViewBaseDescriptor<TType = string,
TProps extends ViewBaseProps = ViewBaseProps,
TState extends ViewBaseState = ViewBaseState,
TRawProps extends ViewRawProps = ViewRawProps> = Descriptor<TType, TProps, TState, TRawProps>;


/**
 * @deprecated: Use ViewBaseDescriptor instead.
 */
export type ViewDynamicDescriptor = Descriptor<string, ViewRawProps>

export abstract class BaseAttributeModifier<TPropertyHolder, TAttribute> implements AttributeModifier<TAttribute> {
  public propertyHolder: TPropertyHolder;

  constructor(propertyHolder: TPropertyHolder) {
    this.propertyHolder = propertyHolder;
  }

  abstract applyNormalAttribute(instance: TAttribute): void;

  protected maybeAssignAttribute<TPropertyValue>(
    extractPropertyValue: (propertyHolder: TPropertyHolder) => TPropertyValue,
    assign: (propertyValue: TPropertyValue) => void,
    defaultValue: TPropertyValue,
  ): void {
    const newPropertyValue = extractPropertyValue(this.propertyHolder)
    let isNewPropertyValueEqualToDefault = newPropertyValue === defaultValue || Number.isNaN(newPropertyValue)
    if (Array.isArray(defaultValue) && Array.isArray(newPropertyValue)) {
      isNewPropertyValueEqualToDefault = areFirstLevelElementsEqual(newPropertyValue, defaultValue)
    } else if (typeof defaultValue === "object" && typeof newPropertyValue === "object") {
      isNewPropertyValueEqualToDefault = areFirstLevelPropsEqual(newPropertyValue, defaultValue)
    }
    if (!isNewPropertyValueEqualToDefault) {
      assign(newPropertyValue)
    }
  }
}


export class RNViewBaseAttributeModifier extends BaseAttributeModifier<ViewDescriptorWrapper, CommonAttribute> {
  private ctx: RNComponentContext;

  constructor(propertyHolder: ViewDescriptorWrapper, ctx: RNComponentContext) {
    super(propertyHolder)
    this.ctx = ctx
  }

  applyNormalAttribute(instance: CommonAttribute): void {
    this.maybeAssignAttribute((dw => dw.width), (val) => instance.width(val), undefined)
    this.maybeAssignAttribute((dw => dw.height), (val) => instance.height(val), undefined)
    this.maybeAssignAttribute((dw => dw.backgroundColor), (val) => instance.backgroundColor(val), "#00000000")
    this.maybeAssignAttribute<Position>((dw => dw.positionRelativeToParent), (val) => instance.position(val), {
      x: 0,
      y: 0
    })

    this.maybeAssignAttribute<Edges<number | undefined>>((dw => dw.borderWidth), (val) => instance.borderWidth(val), {
      top: undefined,
      left: undefined,
      right: undefined,
      bottom: undefined,
    })
    this.maybeAssignAttribute((dw => dw.borderColor), (val) => instance.borderColor(val), {
      top: "#FF000000",
      left: "#FF000000",
      right: "#FF000000",
      bottom: "#FF000000"
    })
    this.maybeAssignAttribute<Corners<number | undefined>>((dw => dw.borderRadius), (val) => instance.borderRadius(val),
      {
        topLeft: undefined,
        topRight: undefined,
        bottomLeft: undefined,
        bottomRight: undefined
      })
    this.maybeAssignAttribute((dw => dw.borderStyle), (val) => {
      let borderStyle: BorderStyle = BorderStyle.Solid
      switch (val) {
        case "dashed":
          borderStyle = BorderStyle.Dashed
          break;
        case "dotted":
          borderStyle = BorderStyle.Dotted
          break;
        default:
          borderStyle = BorderStyle.Solid
          break;
      }
      instance.borderStyle(borderStyle)
    }, "solid")
    this.maybeAssignAttribute(dw => dw.opacity, val => instance.opacity(val), 1)
    this.maybeAssignAttribute(dw => dw.rawTransformationMatrix, val => {
      if (!val) {
        return;
      }
      const rawTransformationMatrix = val.slice()
      rawTransformationMatrix[12] = vp2px(rawTransformationMatrix[12]);
      rawTransformationMatrix[13] = vp2px(rawTransformationMatrix[13]);
      rawTransformationMatrix[14] = vp2px(rawTransformationMatrix[14]);
      instance.transform(matrix4.init(rawTransformationMatrix as TransformMatrix))
    }, [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1])
    this.maybeAssignAttribute(dw => dw.pointerEvents, val => {
      instance.hitTestBehavior(["box-none", "none"].includes(val) ? HitTestMode.None : HitTestMode.Default)
    }, "auto")
    this.maybeAssignAttribute(dw => dw.isClipping, val => instance.clip(val), false)
    this.maybeAssignAttribute(dw => dw.shadow, val => instance.shadow(val), undefined)
    this.maybeAssignAttribute(dw => dw.accessibilityGroup, val => instance.accessibilityGroup(val), false)
    this.maybeAssignAttribute(dw => dw.accessibilityText, val => instance.accessibilityText(val), undefined)
    this.maybeAssignAttribute(dw => dw.accessibilityLevel, val => instance.accessibilityLevel(val), "auto")
    this.maybeAssignAttribute(dw => dw.accessibilityDescription, val => instance.accessibilityDescription(val),
      undefined)

  }
}

@Component
export struct RNViewBase {
  public ctx!: RNOHContext
  public tag: number = 0
  public componentManager!: RNViewManager;
  @Prop public customAlign: Alignment = Alignment.TopStart;
  @Prop public controlsFocus: boolean = true;
  @BuilderParam public renderChildren: () => void
  @State private descriptorWrapper: ViewDescriptorWrapperBase | undefined = undefined
  @State private rnViewAttributeModifier: RNViewBaseAttributeModifier | undefined = undefined
  private unregisterDescriptorChangesListener?: () => void = undefined
  private unregisterComponentManager?: () => void = undefined
  private unregisterCommandCallback?: () => void = undefined

  aboutToAppear() {
    const descriptor = this.ctx.descriptorRegistry.getDescriptor<ViewBaseDescriptor>(this.tag)
    if (!this.componentManager) {
      this.componentManager = new RNViewManager(this.tag, this.ctx)
      this.unregisterComponentManager =
        this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager)
    }
    this.onDescriptorChange(descriptor)
    this.maybeLogStartUpTime()
    this.unregisterDescriptorChangesListener = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (newDescriptor) => {
        this.onDescriptorChange(newDescriptor)
      }
    )
  }

  private onDescriptorChange(descriptor: ViewBaseDescriptor) {
    if (!descriptor) {
      this.ctx.logger.error(new RNOHError({
        whatHappened: `Received undefined descriptor. (tag = ${this.tag})`,
        howCanItBeFixed: []
      }))
      return;
    }
    let descriptorWrapper =
      this.ctx.descriptorRegistry.findDescriptorWrapperByTag<ViewDescriptorWrapperBase>(descriptor.tag)

    if (descriptor && this.isWrappingPullToRefresh(descriptor)) {
      let parentTag = descriptor.parentTag ?? this.tag;
      let parentDescriptor = this.ctx.descriptorRegistry.getDescriptor(parentTag);

      // Set layout metrics of PullToRefreshView to current RNView
      descriptor.layoutMetrics = parentDescriptor.layoutMetrics
    }

    if (!descriptorWrapper || !(descriptorWrapper instanceof ViewDescriptorWrapperBase)) {
      descriptorWrapper = new ViewDescriptorWrapperBase(descriptor)
    }
    if (!descriptorWrapper) {
      this.ctx.logger.error(new RNOHError({
        whatHappened: `Couldn't get or create DescriptorWrapper. (tag = ${this.tag})`,
        howCanItBeFixed: []
      }))
      return;
    }
    if (descriptorWrapper.parentTag) {
      this.componentManager.setParentTag(descriptorWrapper.parentTag);
    }
    if (this.ctx instanceof RNComponentContext) {
      this.rnViewAttributeModifier = new RNViewBaseAttributeModifier(descriptorWrapper, this.ctx)
    }
    this.descriptorWrapper = descriptorWrapper
  }

  private maybeLogStartUpTime() {
    if ((this.descriptorWrapper?.hints ?? []).includes("ready")) {
      this.ctx.markReadiness()
    }
  }

  private isWrappingPullToRefresh(descriptor: ViewBaseDescriptor) {
    let parentTag = descriptor.parentTag ?? this.tag;
    let parentDescriptor = this.ctx.descriptorRegistry.getDescriptor(parentTag);

    if (parentDescriptor?.type === RN_PULL_TO_REFRESH_VIEW_NAME) {
      return true
    }

    return false
  }

  aboutToDisappear() {
    this.unregisterDescriptorChangesListener?.()
    this.unregisterComponentManager?.()
    this.unregisterCommandCallback?.()
  }

  onClickEvent(event: ClickEvent): void {
    if (this.controlsFocus && event.source !== SourceType.TouchScreen && event.source !== SourceType.Mouse) {
      this.ctx.rnInstance.emitComponentEvent(this.tag, "onClick", {})
    }
  }

  build() {
    Stack() {
      /**
       * Focus hack: container components cannot be focused if they don't contain focusable children.
       * An invisible button is added to a focusable View to avoid this problem.
       */
      if (this.controlsFocus && this.descriptorWrapper?.focusable) {
        Button().width(0).height(0).padding(0).margin(0)
      }
      this.renderChildren()
    }
    .align(this.customAlign)
    .id(this.tag.toString())
    .onClick((event: ClickEvent) => this.onClickEvent(event))
    .attributeModifier(this.rnViewAttributeModifier)
  }
}
