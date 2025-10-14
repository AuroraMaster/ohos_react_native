/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
  CustomComponentBuilder,
  Descriptor,
  DescriptorWrapper,
  Position,
  RNComponentContext,
  RNOHComponentCommand,
  RNOHContext,
  Size,
  Tag,
} from '../../../RNOH';
import { _RNScrollViewManager } from './RNScrollViewManager';
import { RNViewBase, ViewDescriptorWrapperBase, RNViewBaseAttributeModifier } from '../RNViewBase';
import { RN_PULL_TO_REFRESH_VIEW_NAME, RNPullToRefreshView } from '../RNPullToRefreshView';
import { RNComponentFactory } from '../RNComponentFactory';
import {
  CurrentOffset,
  FirstVisibleView,
  IndicatorStyle,
  ScrollEvent,
  ScrollViewProps,
  ScrollViewRawProps,
  ScrollViewState
} from './types';
import { calculateSnapTarget } from './SnapTargetUtils';


class ScrollViewDescriptorWrapper extends ViewDescriptorWrapperBase<"ScrollView", ScrollViewProps, ScrollViewState, ScrollViewRawProps> {
}

interface IAnimation {
  duration?: number;
  curve?: Curve | ICurve;
};

type Animation = IAnimation | boolean;

interface RemainingOffset { offsetRemain: number };

function getScrollSnapAlign(snapToAlignment: string | undefined): ScrollSnapAlign {
  if (snapToAlignment === 'center') {
    return ScrollSnapAlign.CENTER;
  } else if (snapToAlignment === 'end') {
    return ScrollSnapAlign.END;
  } else {
    return ScrollSnapAlign.START;
  }
}

export type ScrollViewDescriptor = Descriptor<"ScrollView", ScrollViewProps, ScrollViewState, ScrollViewRawProps>;

@Component
export struct RNScrollView {
  public ctx!: RNOHContext
  public tag: number = 0
  /** @deprecated  */
  @BuilderParam public buildCustomComponent!: CustomComponentBuilder
  @State props: ScrollViewProps = {} as ScrollViewProps
  @State contentSize: Size = {} as Size
  @State childrenTags: Tag[] = []
  @State descriptor: ScrollViewDescriptor = Object() as ScrollViewDescriptor
  @State descriptorWrapper: ScrollViewDescriptorWrapper | undefined = undefined
  @State indicatorFlashCounter: number = 0;
  private parentTag: Tag | undefined = undefined
  private scroller = new Scroller()
  private cleanUpCallbacks: (() => void)[] = []
  private scrollState: ScrollState = ScrollState.Idle
  private componentManager!: _RNScrollViewManager
  private lastScrollDispatchTime: number = 0
  private allowNextScrollEvent: boolean = false
  private contentOffset: Position = { x: 0, y: 0 }
  private recentDimOffsetDelta: number = 0
  @State shouldDisableSnapping: boolean = false
  @State shouldPreventScrolling: boolean = false
  private firstVisibleView: FirstVisibleView | undefined = undefined
  private positionBeforeScrolling: Position | undefined = undefined
  private isScrolling: boolean = false

  aboutToAppear() {
    const descriptor = this.ctx.descriptorRegistry.getDescriptor<ScrollViewDescriptor>(this.tag)
    this.onDescriptorUpdate(descriptor)
    this.cleanUpCallbacks.push(this.ctx.descriptorRegistry.subscribeToDescriptorChanges(this.tag,
      (descriptor) => this.onDescriptorUpdate(descriptor as ScrollViewDescriptor)
    ))
    this.cleanUpCallbacks.push(this.ctx.componentCommandReceiver.registerCommandCallback(
      this.tag,
      (command, args: Object[]) => {
        if (command === 'scrollTo') {
          this.scrollTo(args[0] as number, args[1] as number, args[2] as boolean);
        } else if (command === 'scrollToEnd') {
          this.scrollToEnd(args[0] as boolean);
        } else if (command === 'flashScrollIndicators') {
          this.flashScrollIndicators()
        } else if (command === RNOHComponentCommand.BLOCK_NATIVE_RESPONDER) {
          this.shouldPreventScrolling = true
        } else if (command === RNOHComponentCommand.UNBLOCK_NATIVE_RESPONDER) {
          this.shouldPreventScrolling = false
        }
      }));
    this.componentManager = new _RNScrollViewManager(this.tag, this.ctx, this.scroller)
    this.cleanUpCallbacks.push(this.ctx.componentManagerRegistry.registerComponentManager(this.tag, this.componentManager));
    this.cleanUpCallbacks.push(this.componentManager.subscribe("LOCK_SCROLLING", () => {
      this.shouldPreventScrolling = true
    }))
    this.cleanUpCallbacks.push(this.componentManager.subscribe("UNLOCK_SCROLLING", () => {
      this.shouldPreventScrolling = false
    }))
  }

  aboutToDisappear() {
    this.cleanUpCallbacks.forEach(cleanUp => cleanUp())
  }

  onDescriptorUpdate(newDescriptor: ScrollViewDescriptor) {
    const prevState = this.descriptor.state
    const prevProps = this.props
    this.props = newDescriptor.props
    this.descriptor = newDescriptor
    this.descriptorWrapper = new ScrollViewDescriptorWrapper(newDescriptor)
    const contentSizeWidth = this.descriptor.state.contentSizeWidth;
    const contentSizeHeight = this.descriptor.state.contentSizeHeight;
    this.contentSize = {
      width: contentSizeWidth,
      height: contentSizeHeight
    }
    this.childrenTags = newDescriptor.childrenTags
    this.parentTag = newDescriptor.parentTag

    const contentSizeChanged = contentSizeWidth !== prevState?.contentSizeWidth
      || contentSizeHeight !== prevState?.contentSizeHeight
    if (newDescriptor.props.contentOffsetX !== prevProps.contentOffsetX || newDescriptor.props.contentOffsetY !== prevProps.contentOffsetY) {
      this.scrollToPropsContentOffset();
    }
    else {
      if (this.scrollState === ScrollState.Idle && contentSizeChanged) {
        /**
         * When a scroll content is removed, and it appears again without scrolling, scroll should remain in the original position.
         * This behavior was observed on Android. Local contentOffset is used, to use the most recent position.
         * The contentOffset in the descriptor's state might be delayed.
         */
        const width = this.descriptorWrapper?.width ?? 0;
        const height = this.descriptorWrapper?.height ?? 0;
        const maxOffsetX = contentSizeWidth < width ? 0 : (contentSizeWidth - width);
        const maxOffsetY = contentSizeHeight < height ? 0 : (contentSizeHeight - height);
        const xOffset = (width && this.contentOffset.x > maxOffsetX) ? maxOffsetX : this.contentOffset.x;
        const yOffset = (height && this.contentOffset.y > maxOffsetY) ? maxOffsetY : this.contentOffset.y;
        this.scroller.scrollTo({
          xOffset,
          yOffset
        });
      }
    }
    if (this.descriptor.rawProps.maintainVisibleContentPosition) {
      if (contentSizeChanged) {
        const autoscrollToTopThreshold = this.descriptor.rawProps.maintainVisibleContentPosition.autoscrollToTopThreshold;
        this.maintainVisibleContentPosition(autoscrollToTopThreshold);
      }
      const minIndexForVisible = this.descriptor.rawProps.maintainVisibleContentPosition.minIndexForVisible;
      this.getFirstVisibleView(minIndexForVisible);
    }
  }

  maintainVisibleContentPosition(autoscrollToTopThreshold: number | undefined): void {
    if (!this.firstVisibleView) {
      return;
    }
    const horizontal = this.props.flexDirection === 2;
    const child = this.ctx.descriptorRegistry.getDescriptor(this.firstVisibleView.tag);
    if (!child) return;
    const newPosition = child.layoutMetrics.frame.origin;
    if (horizontal) {
      const deltaX = newPosition.x - this.firstVisibleView.position;
      if (deltaX != 0) {
        const scrollX = this.contentOffset.x;
        this.scroller.scrollTo({ xOffset: scrollX + deltaX, yOffset: this.contentOffset.y });
        if (autoscrollToTopThreshold && scrollX <= autoscrollToTopThreshold) {
          this.scroller.scrollTo({ xOffset: 0, yOffset: this.contentOffset.y, animation: true });
        }
      }
    }
    else {
      const deltaY = newPosition.y - this.firstVisibleView.position;
      if (deltaY != 0) {
        const scrollY = this.contentOffset.y;
        this.scroller.scrollTo({ xOffset: this.contentOffset.x, yOffset: scrollY + deltaY });
        if (autoscrollToTopThreshold && scrollY <= autoscrollToTopThreshold) {
          this.scroller.scrollTo({ xOffset: this.contentOffset.x, yOffset: 0, animation: true });
        }
      }
    }
  }

  getFirstVisibleView(minIndexForVisible: number): void {
    const horizontal = this.props.flexDirection === 2;
    const currentScrollPosition = horizontal ? this.contentOffset.x : this.contentOffset.y;
    const container = this.ctx.descriptorRegistry.getDescriptor(this.childrenTags[0]);
    const children = container.childrenTags;
    const containerOffset = horizontal ? container.layoutMetrics.frame.origin.x : container.layoutMetrics.frame.origin.y;
    this.firstVisibleView = undefined
    for (let i = minIndexForVisible; i < children.length; ++i) {
      const child = this.ctx.descriptorRegistry.getDescriptor(children[i]);
      const position = horizontal ? child.layoutMetrics.frame.origin.x : child.layoutMetrics.frame.origin.y;
      if (position + containerOffset >= currentScrollPosition || i == children.length - 1) {
        this.firstVisibleView = {
          position: position,
          tag: children[i],
        }
        break;
      }
    }
  }

  scrollToPropsContentOffset() {
    const xOffset = this.props.contentOffsetX;
    const yOffset = this.props.contentOffsetY;
    this.scroller.scrollTo({ xOffset, yOffset });
    this.contentOffset = { x: this.props.contentOffsetX, y: this.props.contentOffsetY }
  }

  scrollTo(xOffset: number, yOffset: number, animated: boolean = false) {
    const animation: Animation | undefined = animated ? { duration: 1000, curve: Curve.Smooth } : undefined;
    // When animation === undefined,onScrollStart and
    // onScrollStop will not be triggered,so we need to
    // initialize and destruct the parameters here.
    const currentOffset = this.scroller.currentOffset();
    if (!animation && currentOffset) {
      this.positionBeforeScrolling = {
        x: currentOffset.xOffset,
        y: currentOffset.yOffset,
      };
      this.isScrolling = false
    }
    this.scroller.scrollTo({ xOffset, yOffset, animation })
    this.contentOffset = { x: xOffset, y: yOffset };
    if (!animation) {
      this.isScrolling = false;
    }
  }

  scrollToEnd(animated: boolean) {
    if (this.descriptor.rawProps.inverted) {
      this.scrollTo(0, 0, animated);
    } else {
      const xOffset = this.props.flexDirection === 2 ? this.contentSize.width : 0;
      const yOffset = this.props.flexDirection === 2 ? 0 : this.contentSize.height;
      this.scrollTo(xOffset, yOffset, animated);
    }
  }

  flashScrollIndicators() {
    this.indicatorFlashCounter++;
    setTimeout(() => this.indicatorFlashCounter--, 500);
  }

  scrollDirection(): ScrollDirection {
    return this.props.flexDirection === 2 ? ScrollDirection.Horizontal : ScrollDirection.Vertical;
  }

  scrollBarState(): BarState {
    if (this.indicatorFlashCounter > 0) {
      return BarState.On
    }

    if (((this.props.flexDirection === 2) && !this.props.showsHorizontalScrollIndicator)
      || ((this.props.flexDirection !== 2) && !this.props.showsVerticalScrollIndicator)) {
      return BarState.Off
    }
    return this.props.persistentScrollbar ? BarState.On : BarState.Auto
  }

  scrollBarColor() {
    if (this.props.indicatorStyle === IndicatorStyle.White) {
      return Color.White;
    }
    return Color.Black;
  }

  getFriction() {
    // default deceleration rate and friction values differ between ArkUI and RN
    // so we adapt the decelerationRate accordingly to resemble iOS behaviour
    // iOS's UIScrollView supports only two values of decelerationRate
    // called 'normal' and 'fast' and maps all other to the nearest of those two
    const IOS_NORMAL = 0.998;
    const IOS_FAST = 0.99;
    const ARKUI_FAST = 2;
    const ARKUI_NORMAL = 0.6;
    // only allow one page scroll at a time
    if (this.descriptor.rawProps.pagingEnabled) {
      return 100;
    }
    if (this.props.decelerationRate < (IOS_NORMAL + IOS_FAST) / 2) {
      return ARKUI_FAST;
    }
    else {
      return ARKUI_NORMAL;
    }
  }

  getScrollSnap(): ScrollSnapOptions | undefined {
    if (this.shouldDisableSnapping) {
      return undefined
    }
    const snapToOffsets = this.descriptor.rawProps.snapToOffsets
    const snapToStart = this.descriptor.rawProps.snapToStart
    const snapToEnd = this.descriptor.rawProps.snapToEnd
    const pagingEnabled = this.descriptor.rawProps.pagingEnabled;
    if (pagingEnabled) {
      return {
        snapAlign: ScrollSnapAlign.START,
        snapPagination: '100%',
        enableSnapToStart: snapToStart,
        enableSnapToEnd: snapToEnd,
      }
    }
    if (snapToOffsets && snapToOffsets.length > 0) {
      return {
        snapAlign: ScrollSnapAlign.START,
        snapPagination: snapToOffsets,
        enableSnapToStart: snapToStart,
        enableSnapToEnd: snapToEnd,
      }
    }
    const snapToInterval = this.descriptor.rawProps.snapToInterval
    if (snapToInterval && snapToInterval > 0) {
      return {
        snapAlign: getScrollSnapAlign(this.descriptor.rawProps.snapToAlignment),
        snapPagination: snapToInterval,
      }
    }
    return undefined
  }

  getContentOffset(): Position {
    const currentOffset = this.scroller.currentOffset();
    return {
      x: currentOffset.xOffset,
      y: currentOffset.yOffset,
    };
  };

  createScrollEvent(): ScrollEvent | undefined {
    const descriptor = this.ctx.descriptorRegistry.getDescriptor<ScrollViewDescriptor>(this.tag);

    if (!descriptor) {
      // the view was unmounted while scrolling - don't report the event in that case
      return;
    }

    return {
      contentSize: {
        width: this.contentSize.width,
        height: this.contentSize.height,
      },
      contentOffset: this.getContentOffset(),
      containerSize: {
        width: this.descriptor.layoutMetrics.frame.size.width,
        height: this.descriptor.layoutMetrics.frame.size.height,
      },
      zoomScale: 1,
      responderIgnoreScroll: true,
    }
  }

  onScroll(offset: number, scrollState: ScrollState): RemainingOffset {
    this.recentDimOffsetDelta = offset
    const currentScrollState = this.scrollState;

    if (currentScrollState != scrollState) {
      if (currentScrollState === ScrollState.Scroll) {
        this.handleDragEnd();
      } else if (currentScrollState === ScrollState.Fling) {
        this.onMomentumEnd();
      }

      if (scrollState === ScrollState.Scroll) {
        this.onDragBegin();
      } else if (scrollState === ScrollState.Fling) {
        this.onMomentumBegin();
      }
    }
    this.scrollState = scrollState
    return { offsetRemain: offset }
  }

  onScrollEvent() {
    const currentOffset: CurrentOffset = this.scroller.currentOffset();
    const descriptor = this.ctx.descriptorRegistry.getDescriptor<ScrollViewDescriptor>(this.tag);

    if (!descriptor) {
      // the view was unmounted while scrolling - don't report the event in that case
      return;
    }

    if (!this.isScrolling && (this.positionBeforeScrolling) !== undefined) {
      // The offset precision has a bug. Sometimes, there is a slight deviation,
      // After the problem is solved in the os, change the values to strict equal,
      if (Math.abs(this.positionBeforeScrolling.x - currentOffset.xOffset) > 1 || Math.abs(this.positionBeforeScrolling.y - currentOffset.yOffset) > 1) {
        this.isScrolling = true;
      }
    }
    this.contentOffset = this.getContentOffset();

    const now = Date.now();
    if (this.allowNextScrollEvent || this.isScrolling && descriptor.props.scrollEventThrottle < now - this.lastScrollDispatchTime) {
      this.lastScrollDispatchTime = now
      this.ctx.rnInstance.emitComponentEvent(
        this.tag,
        "onScroll",
        this.createScrollEvent()
      )
    }
  }

  onDragBegin() {
    this.ctx.rnInstance.emitComponentEvent(
      this.tag,
      "onScrollBeginDrag",
      this.createScrollEvent()
    )
  }

  handleDragEnd() {
    // if `disableIntervalMomentum` is not explicitly set,
    // but we have `pagingEnabled`, we still use the custom snapping logic
    const disableIntervalMomentum = this.descriptor.rawProps.disableIntervalMomentum ?? this.descriptor.rawProps.pagingEnabled
    if (disableIntervalMomentum) {
      const snapTarget = calculateSnapTarget({
        currentOffset: this.scroller.currentOffset(),
        isPagingEnabled: this.descriptor.rawProps.pagingEnabled ?? false,
        offsets: this.descriptor.rawProps.snapToOffsets,
        interval: this.descriptor.rawProps.snapToInterval,
        isHorizontalScroll: this.scrollDirection() === ScrollDirection.Horizontal,
        container: {
          width: this.descriptorWrapper?.width ?? 0,
          height: this.descriptorWrapper?.height ?? 0,
        },
        content: this.contentSize,
        recentDimOffsetDelta: this.recentDimOffsetDelta
      })
      if (snapTarget) {
        this.shouldDisableSnapping = true
        const snapDurationInMs = 500
        this.scroller.scrollTo({
          xOffset: snapTarget.xOffset,
          yOffset: snapTarget.yOffset,
          animation: { duration: snapDurationInMs }
        })
        setTimeout(() => {
          this.shouldDisableSnapping = false
        }, snapDurationInMs)
      }
    }
    this.ctx.rnInstance.emitComponentEvent(
      this.tag,
      "onScrollEndDrag",
      this.createScrollEvent()
    )
  }

  onMomentumBegin() {
    this.ctx.rnInstance.emitComponentEvent(
      this.tag,
      "onMomentumScrollBegin",
      this.createScrollEvent()
    )
  }

  onMomentumEnd() {
    this.ctx.rnInstance.emitComponentEvent(
      this.tag,
      "onMomentumScrollEnd",
      this.createScrollEvent()
    )
  }

  onScrollBegin() {
    // this.positionBeforeScrolling = { x : this.descriptor.state.contentOffsetX, y : this.descriptor.state.contentOffsetY };
    this.positionBeforeScrolling = this.getContentOffset();
    this.isScrolling = false;
    this.componentManager.setScrolling(true);
    this.allowNextScrollEvent = false;
  }

  onScrollEnd() {
    const descriptor = this.ctx.descriptorRegistry.getDescriptor<ScrollViewDescriptor>(this.tag);
    if (!descriptor) {
      return;
    }

    this.componentManager.setScrolling(false);

    // fire an additional event after the scrolling has ended
    // ignoring throttle
    this.allowNextScrollEvent = true;
    this.onScrollEvent();

    if (this.scrollState === ScrollState.Fling) {
      this.onMomentumEnd();
    } else if (this.scrollState === ScrollState.Scroll) {
      this.handleDragEnd();
    }
    this.scrollState = ScrollState.Idle;

    this.contentOffset = this.getContentOffset();
    descriptor.state.contentOffsetX = this.contentOffset.x;
    descriptor.state.contentOffsetY = this.contentOffset.y;
    this.ctx.descriptorRegistry.setState<ScrollViewState>(this.tag, descriptor.state);
    this.isScrolling = false;
  }

  private shouldWrapWithPullToRefresh() {
    const pullToRefreshTag = this.parentTag
    if (pullToRefreshTag === undefined) {
      return false
    }
    const parentDescriptor = this.ctx.descriptorRegistry.getDescriptor<Descriptor>(pullToRefreshTag)
    if (parentDescriptor.type !== RN_PULL_TO_REFRESH_VIEW_NAME) {
      return false
    }
    return true
  }

  shouldBounce(): boolean {
    let alwaysBounces = this.scrollDirection() === ScrollDirection.Vertical ? this.descriptor.rawProps.alwaysBounceVertical : this.descriptor.rawProps.alwaysBounceHorizontal;
    if (alwaysBounces === undefined) {
      alwaysBounces = true
    }
    const hasLittleContent = this.scrollDirection() === ScrollDirection.Vertical ?
      this.contentSize.height <= this.descriptor.layoutMetrics.frame.size.height : this.contentSize.width <= this.descriptor.layoutMetrics.frame.size.width;
    if (this.props.bounces && (!hasLittleContent || alwaysBounces)) {
      return true
    } else {
      return false
    }
  }

  getEdgeEffect(): EdgeEffect {
    if (this.shouldBounce()) {
      return EdgeEffect.Spring;
    }
    else {
      return EdgeEffect.None;
    }
  }

  getNestedScroll(): NestedScrollOptions {
    const isNestedInRefreshControl = this.shouldWrapWithPullToRefresh();
    if (isNestedInRefreshControl) {
      return { scrollForward: NestedScrollMode.SELF_ONLY, scrollBackward: NestedScrollMode.SELF_ONLY };
    }
    return { scrollForward: NestedScrollMode.SELF_FIRST, scrollBackward: NestedScrollMode.SELF_FIRST }
  }

  @Builder
  buildScrollCore() {
    Scroll(this.scroller) {
      Stack() {
        if (this.ctx instanceof RNComponentContext) {
          LazyForEach(this.ctx.createComponentDataSource({ tag: this.tag }),
            (descriptorWrapper: DescriptorWrapper) => {
              (this.ctx as RNComponentContext).wrappedRNComponentBuilder.builder((this.ctx as RNComponentContext), descriptorWrapper.tag)
            },
            (descriptorWrapper: DescriptorWrapper) => descriptorWrapper.tag.toString() + "@" + descriptorWrapper.renderKey
          )
        } else {
          ForEach(this.childrenTags, (childrenTag: Tag) => {
            RNComponentFactory({ ctx: this.ctx, tag: childrenTag, buildCustomComponent: this.buildCustomComponent })
          }, (childrenTag: Tag) => childrenTag.toString())
        }
      }
      .width(this.contentSize.width)
      .height(this.contentSize.height)
      .backgroundColor(Color.Transparent)
      .margin({
        top: -Number(this.descriptorWrapper?.borderWidth.top ?? 0),
        left: -Number(this.descriptorWrapper?.borderWidth.left ?? 0)
      })
    }
    .width("100%")
    .height("100%")
    .align(Alignment.TopStart)
    .scrollable(this.scrollDirection())
    .scrollBar(this.scrollBarState())
    .scrollBarColor(this.scrollBarColor())
    .enableScrollInteraction((this.props.scrollEnabled && !this.shouldPreventScrolling) ?? true)
    .nestedScroll(this.getNestedScroll())
    .edgeEffect(this.getEdgeEffect())
    .onScrollFrameBegin((offset, state) => {
      return this.onScroll(offset, state)
    })
    .onScroll(() => {
      this.onScrollEvent()
    })
    .onScrollStart(() => {
      this.onScrollBegin()
    })
    .onScrollStop(() => {
      this.onScrollEnd()
    })
    .onAppear(() => {
      if (this.descriptor.state.contentOffsetX !== 0 || this.descriptor.state.contentOffsetY !== 0) {
        this.scroller.scrollTo({
          xOffset: this.descriptor.state.contentOffsetX,
          yOffset: this.descriptor.state.contentOffsetY
        });
        this.contentOffset = {
          x: this.descriptor.state.contentOffsetX,
          y: this.descriptor.state.contentOffsetY,
        }
      } else if (this.props.contentOffsetX !== 0 || this.props.contentOffsetY !== 0) {
        this.scrollToPropsContentOffset()
      } else if (this.contentOffset.x !== 0 || this.contentOffset.y !== 0) {
        // this handles the case where `scrollTo` has been called
        // before the native component mounted
        this.scroller.scrollTo({
          xOffset: this.contentOffset.x,
          yOffset: this.contentOffset.y,
        })
      }
    })
    .friction(this.getFriction())
    .scrollSnap(this.getScrollSnap())
  }

  build() {
    if (this.shouldWrapWithPullToRefresh()) {
      RNViewBase({ ctx: this.ctx, tag: this.parentTag }) {
        RNPullToRefreshView({ ctx: this.ctx, tag: this.parentTag }) {
          this.buildScrollCore()
        }
      }
    } else {
      RNViewBase({ ctx: this.ctx, tag: this.tag, componentManager: this.componentManager }) {
        this.buildScrollCore()
      }
    }
  }
}