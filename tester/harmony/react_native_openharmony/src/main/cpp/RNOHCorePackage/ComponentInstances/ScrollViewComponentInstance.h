/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/scrollview/ScrollViewEventEmitter.h>
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/ScrollNode.h"
#include "RNOH/arkui/StackNode.h"
#include "RNOHCorePackage/TurboModules/Animated/NativeAnimatedTurboModule.h"

namespace rnoh {

class ScrollViewComponentInstance
    : public CppComponentInstance<facebook::react::ScrollViewShadowNode>,
      public ScrollNodeDelegate {
 private:
  enum ScrollState : int32_t { IDLE, SCROLL, FLING, CANCELING };
  struct ChildTagWithOffset {
    facebook::react::Tag tag;
    facebook::react::Float offset;
  };
  ScrollNode m_scrollNode;
  StackNode m_contentContainerNode;
  bool m_horizontal = false;
  facebook::react::Size m_contentSize;
  facebook::react::Size m_containerSize;
  ScrollState m_scrollState = IDLE;
  facebook::react::Point m_currentOffset = {0, 0};
  bool m_persistentScrollbar = false;
  long long m_lastScrollDispatchTime = 0;
  bool m_allowNextScrollEvent = false;
  bool m_scrollStartFling = false;
  facebook::react::Float m_scrollEventThrottle = 0;
  bool m_isNativeResponderBlocked = false;
  std::weak_ptr<NativeAnimatedTurboModule> m_nativeAnimatedTurboModule{};
  bool m_allowScrollPropagation = true;
  bool m_scrollNestedModeFromOutside = false;
  bool m_disableIntervalMomentum = false;
  bool m_scrollToOverflowEnabled = false;
  float m_recentScrollFrameOffset = 0;
  bool m_shouldAdjustScrollPositionOnNextRender = false;
  std::vector<facebook::react::Float> m_snapToOffsets = {};
  std::optional<ChildTagWithOffset> m_firstVisibleView = std::nullopt;
  bool m_enableScrollInteraction = true;  

  // Mimics of implementation in ImageComponentInstance.cpp
  struct ScrollViewRawProps {
    std::optional<std::string> overScrollMode;
    std::optional<bool> nestedScrollEnabled;
    std::optional<uint32_t> endFillColor;
    std::optional<float> fadingEdgeLength;
    static ScrollViewRawProps getFromDynamic(folly::dynamic value);
  };
  ScrollViewRawProps m_rawProps;

  facebook::react::Float getFrictionFromDecelerationRate(
      facebook::react::Float decelerationRate);
  void emitOnScrollEndDragEvent();
  void emitOnMomentumScrollEndEvent();
  void scrollToEnd(bool animated);
  ArkUI_ScrollBarDisplayMode getScrollBarDisplayMode(
      bool horizontal,
      bool persistentScrollBar,
      bool showsVerticalScrollIndicator,
      bool showsHorizontalScrollIndicator);
  void setScrollSnap(
      bool snapToStart,
      bool snapToEnd,
      const std::vector<facebook::react::Float>& snapToOffsets,
      facebook::react::Float snapToInterval,
      facebook::react::ScrollViewSnapToAlignment snapToAlignment);
  bool scrollMovedBySignificantOffset(facebook::react::Point newOffset);
  folly::dynamic getScrollEventPayload(
      facebook::react::ScrollViewMetrics const& scrollViewMetrics);

  void sendEventForNativeAnimations(
      facebook::react::ScrollViewMetrics const& scrollViewMetrics);
  bool isHorizontal(SharedConcreteProps const& props);
  void disableIntervalMomentum();
  std::optional<float> getNextSnapTarget();
  void adjustVisibleContentPosition(
      facebook::react::ScrollViewMaintainVisibleContentPosition const&
          scrollViewMaintainVisibleContentPosition);
  std::optional<ChildTagWithOffset> getFirstVisibleView(
      int32_t minIndexForVisible);

  void updateContentClippedSubviews();

 public:
  ScrollViewComponentInstance(Context context);

  ScrollNode& getLocalRootArkUINode() override;

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;

  void setLayout(facebook::react::LayoutMetrics layoutMetrics) override;

  void onStateChanged(SharedConcreteState const& state) override;
  void onPropsChanged(SharedConcreteProps const& props) override;

  void onCommandReceived(
      std::string const& commandName,
      folly::dynamic const& args) override;

  // ScrollNodeDelegate implementation
  void onScroll() override;
  void onScrollStart() override;
  void onScrollStop() override;
  float onScrollFrameBegin(float offset, int32_t scrollState) override;
  void onAppear() override;

  void onFinalizeUpdates() override;

  // TouchTarget implementation
  facebook::react::Point getCurrentOffset() const override;

  facebook::react::ScrollViewMetrics getScrollViewMetrics();

  bool isHandlingTouches() const override;

  bool setKeyboardAvoider(
      ComponentInstance::Weak keyboardAvoidingComponentInstance);
  
  /**
   * This method is provided for internal third-party library use
   * and is not recommended for developers to use.
   * @internal
   */
  void setNestedScrollMode(ArkUI_ScrollNestedMode scrollForward, ArkUI_ScrollNestedMode scrollBackward);

 protected:
  void onNativeResponderBlockChange(bool isBlocked) override;

 private:
  void updateStateWithContentOffset(facebook::react::Point contentOffset);
  void updateOffsetAfterChildChange(facebook::react::Point offset);
  bool isContentSmallerThanContainer(SharedConcreteProps const& props);
  bool isAtEnd(facebook::react::Point currentOffset);
  /**
   * When the `onEndReachedThreshold` of VirtualizedList is set to 0,
   * due to the rebound effect of the Scroll component, 
   * after calling `scrollToEnd`, the last remaining position deviates from the bottom,
   * causing `onEndReached` to not be triggered.
   * So when `isCloseToTargetOffset` returns true, you need to actively emit the `onScroll` event.
   */
  bool isCloseToTargetOffset(facebook::react::Point currentOffset);
  void onContentSizeChanged();
  facebook::react::Float adjustOffsetToRTL(facebook::react::Float x) const;
  facebook::react::Point getScrollOffset() const;
  facebook::react::Point getContentViewOffset() const;
  ComponentInstance::Weak m_keyboardAvoider;
  bool isNestedScroll();
  bool isEnableScrollInteraction(bool scrollEnabled);
  bool shouldDisableScrollInteraction();
  std::optional<facebook::react::Point> m_targetOffsetOfScrollToCommand = std::nullopt;
};

/**
 * HACK: This interface is needed for adjusting scroll position when keyboard
 * appears. It is meant to be implemented by ComponentInstances that can trigger
 * keyboard visibility e.g. TextInput.
 *
 * Adjusting the scroll position when the keyboard appears is the
 * responsibility of the platform. However, the platform doesn't adjust the
 * position correctly when a ScrollView changes its dimensions right after
 * the platform starts adjusting the position. To fix this problem, this hack
 * checks for changes in "__keyboardAvoidingViewBottomHeight" to detect
 * mutations triggered by the keyboard appearance.
 *
 * "__keyboardAvoidingViewBottomHeight" is injected by KeyboardAvoidingView.
 * ScrollView needs to be placed directly inside KeyboardAvoidingView.
 */
class KeyboardAvoider {
 public:
  using Weak = std::weak_ptr<KeyboardAvoider>;

  virtual facebook::react::Float getBottomEdgeOffsetRelativeToScrollView(
      std::shared_ptr<ScrollViewComponentInstance> scrollView) = 0;
};
} // namespace rnoh