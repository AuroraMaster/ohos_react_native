/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { CustomComponentBuilder, RNComponentContext, RNOHContext, Size } from '../../RNOH';
import { Descriptor, DescriptorWrapper, Tag } from '../../RNOH/DescriptorBase';
import { ViewBaseProps } from './RNViewBase';
import { TouchDispatcher } from '../../RNOH/TouchDispatcher';
import { RNModalHostViewManager } from '../componentManagers/RNModalHostViewManager';
import { RNComponentFactory } from './RNComponentFactory';

interface ModalHostViewProps extends ViewBaseProps {
  transparent: boolean;
  visible: boolean;
  identifier: number;
};

type AnimationType = 'fade' | 'slide' | 'none'

interface ModalHostViewRawProps extends ViewBaseProps {
  animationType?: AnimationType
};

interface ModalHostViewState {
  screenSize: Size
};
type ModalHostViewDescriptor = Descriptor<"ModalHostView", ModalHostViewProps, ModalHostViewState, ModalHostViewRawProps>;

const MODAL_ANIMATION_DURATION = 500;

@CustomDialog
struct ModalHostViewDialog {
  public ctx!: RNOHContext
  public tag!: Tag
  /** @deprecated  */
  @BuilderParam public buildCustomComponent!: CustomComponentBuilder
  // ----------------------------------------------------
  @State descriptor: ModalHostViewDescriptor = Object() as ModalHostViewDescriptor
  controller: CustomDialogController
  @State showContent: boolean = false
  registerCloseModalFunction: (close: () => void) => void = () => {
  }
  unsubscribeFromDescriptorChanges: () => void | undefined = () => {
  }
  private cleanUpCallbacks: (() => void)[] = []
  touchDispatcher!: TouchDispatcher;
  componentManager!: RNModalHostViewManager;

  emitComponentEvent(eventName: string) {
    this.ctx.rnInstance.emitComponentEvent(this.tag, eventName, {});
  }

  updateState() {
    const windowPhysicalPixels = this.ctx.getDisplayMetrics()
      .windowPhysicalPixels;
    const height = px2vp(windowPhysicalPixels.height);
    const width = px2vp(windowPhysicalPixels.width);
    this.ctx.descriptorRegistry.setState<ModalHostViewState>(this.tag, {
      screenSize: { height, width }
    });
  }

  aboutToAppear() {
    this.emitComponentEvent("onShow")
    this.registerCloseModalFunction(() => {
      this.showContent = false;
    })
    this.componentManager = new RNModalHostViewManager(this.tag, this.ctx);
    this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager);
    this.descriptor = this.ctx.descriptorRegistry.getDescriptor<ModalHostViewDescriptor>(this.tag)
    this.unsubscribeFromDescriptorChanges = this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag, (descriptor) => {
      this.descriptor = (descriptor as ModalHostViewDescriptor)
      // NOTE: this should ensure we only update the Stack's dimensions
      // after React rerenders the contents,
      // to prevent flickering when opening the modal
      this.showContent = true
    })
    this.touchDispatcher = new TouchDispatcher(this.tag, this.ctx.rnInstance, this.ctx.logger);
    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.subscribeToLifecycleEvents("WINDOW_SIZE_CHANGE", (_windowSize) => {
        this.updateState();
      })
    )
    this.updateState();
  }

  aboutToDisappear() {
    this.emitComponentEvent("onRequestClose")
    this.unsubscribeFromDescriptorChanges?.()
    this.cleanUpCallbacks.forEach(cb => cb())
  }

  getTransitionEffect() {
    if (this.descriptor.rawProps.animationType === 'slide') {
      return TransitionEffect.move(TransitionEdge.BOTTOM)
        .animation({ duration: MODAL_ANIMATION_DURATION })
    } else if (this.descriptor.rawProps.animationType === 'fade') {
      return TransitionEffect.OPACITY.animation({ duration: MODAL_ANIMATION_DURATION });
    } else {
      return TransitionEffect.IDENTITY;
    }
  }

  isModalAnimated() {
    const animationType = this.descriptor.rawProps.animationType;
    return animationType === 'slide' || animationType === 'fade';
  }

  closeModalAfterAnimation() {
    if (this.isModalAnimated()) {
      setTimeout(() => {
        this.controller.close();
      }, MODAL_ANIMATION_DURATION);
    } else {
      this.controller.close()
    }
  }

  build() {
    if (this.showContent) {
      Stack() {
        if (this.ctx instanceof RNComponentContext) {
          LazyForEach(this.ctx.createComponentDataSource({ tag: this.tag }),
            (descriptorWrapper: DescriptorWrapper) => {
              (this.ctx as RNComponentContext).wrappedRNComponentBuilder.builder((this.ctx as RNComponentContext), descriptorWrapper.tag)
            },
            (descriptorWrapper: DescriptorWrapper) => descriptorWrapper.tag.toString() + "@" + descriptorWrapper.renderKey
          )
        } else {
          ForEach(this.descriptor.childrenTags, (childrenTag: Tag) => {
            RNComponentFactory({ ctx: this.ctx, tag: childrenTag, buildCustomComponent: this.buildCustomComponent })
          }, (childrenTag: Tag) => childrenTag.toString())
        }
      }.onTouch((touchEvent) => {
        if (this.showContent) {
          this.touchDispatcher.handleTouchEvent(touchEvent)
        }
      })
      .transition(this.getTransitionEffect())
      .onDisAppear(() => this.closeModalAfterAnimation())
      .position({ x: 0, y: 0 })
    }
  }
}

@Component
export struct RNModalHostView {
  public ctx!: RNOHContext
  public tag: number = 0
  /** @deprecated  */
  @BuilderParam public buildCustomComponent!: CustomComponentBuilder
  private dialogController!: CustomDialogController
  private closeModalFunction: (() => void) | undefined;

  aboutToAppear() {
    this.dialogController = new CustomDialogController({
      builder: ModalHostViewDialog({
        ctx: this.ctx,
        tag: this.tag,
        buildCustomComponent: this.buildCustomComponent,
        registerCloseModalFunction: (closeFunction) => {
          this.closeModalFunction = closeFunction
        },
      }),
      alignment: DialogAlignment.TopStart,
      customStyle: true,
      maskColor: Color.Transparent,
      closeAnimation: { duration: 0 },
      openAnimation: { duration: 0 },
      autoCancel: false,
    })

    // on the JS side, the native ModalHostView is rendered _only_ if the `visible` prop is set to true,
    // so we can completely ignore it and just open/close when this component appears/disappears
    this.dialogController.open()
  }

  aboutToDisappear() {
    this.closeModalFunction?.();
  }

  build() {
  }
}
