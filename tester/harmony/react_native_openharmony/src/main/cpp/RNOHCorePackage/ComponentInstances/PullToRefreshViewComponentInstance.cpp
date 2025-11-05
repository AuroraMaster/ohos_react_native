/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "PullToRefreshViewComponentInstance.h"
#include <react/renderer/components/rncore/Props.h>
#include "ScrollViewComponentInstance.h"

using namespace rnoh;

PullToRefreshViewComponentInstance::PullToRefreshViewComponentInstance(
    Context context)
    : CppComponentInstance(std::move(context)),
      m_refreshNode(m_arkUINodeCtx),
      m_refreshIndicatorContainerNode(m_arkUINodeCtx),
      m_refreshIndicatorBackgroundNode(m_arkUINodeCtx),
      m_refreshIndicatorSpinnerNode(m_arkUINodeCtx),
      m_refreshIndicatorTitleNode(m_arkUINodeCtx) {
  m_refreshIndicatorContainerNode.setZIndex(1.0);
  m_refreshIndicatorContainerNode.setVisibility(ARKUI_VISIBILITY_HIDDEN);

  m_refreshIndicatorContainerNode.insertChild(
      m_refreshIndicatorBackgroundNode, 0);
  m_refreshIndicatorBackgroundNode.insertChild(
      m_refreshIndicatorSpinnerNode, 0);
  m_refreshIndicatorBackgroundNode.setBorderRadius({500, 500, 500, 500})
      .setBackgroundColor(*facebook::react::clearColor())
      .setSize({REFRESH_INDICATOR_SIZE, REFRESH_INDICATOR_SIZE});

  getLocalRootArkUINode().setRefreshNodeDelegate(this);
  getLocalRootArkUINode().setRefreshContent(m_refreshIndicatorContainerNode);
}

void PullToRefreshViewComponentInstance::createRefreshIndicatorTitle(
    const std::string& refreshIndicatorTitleText,
    facebook::react::SharedColor const& refreshIndicatorTitleColor) {
  this->updateRefreshIndicatorTitleNode(
      refreshIndicatorTitleText, refreshIndicatorTitleColor);

  m_refreshIndicatorContainerNode.insertChild(m_refreshIndicatorTitleNode, 1);
}

void PullToRefreshViewComponentInstance::updateRefreshIndicatorTitleNode(
    const std::string& title,
    facebook::react::SharedColor const& color) {
  uint32_t refreshIndicatorTitleColor =
      color ? *color : *facebook::react::blackColor();
  m_refreshIndicatorTitleNode.setFontColor(refreshIndicatorTitleColor)
      .setTextContent(title);
}

void PullToRefreshViewComponentInstance::createOrUpdateRefreshIndicatorTitle(
    const std::string& refreshIndicatorTitleText,
    facebook::react::SharedColor const& refreshIndicatorTitleColor) {
  ArkUI_NodeHandle currRefreshIndicatorTitleNode =
      m_refreshIndicatorContainerNode.getChildAt(1);

  if (!currRefreshIndicatorTitleNode) {
    this->createRefreshIndicatorTitle(
        refreshIndicatorTitleText, refreshIndicatorTitleColor);
    return;
  }

  this->updateRefreshIndicatorTitleNode(
      refreshIndicatorTitleText, refreshIndicatorTitleColor);
}

void PullToRefreshViewComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance,
    std::size_t index) {
  CppComponentInstance::onChildInserted(childComponentInstance, index);
  getLocalRootArkUINode().insertChild(
      childComponentInstance->getLocalRootArkUINode(), index);
}

RefreshNode& PullToRefreshViewComponentInstance::getLocalRootArkUINode() {
  return m_refreshNode;
}

void PullToRefreshViewComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance) {
  CppComponentInstance::onChildRemoved(childComponentInstance);
  getLocalRootArkUINode().removeChild(childComponentInstance->getLocalRootArkUINode());
}

void PullToRefreshViewComponentInstance::onPropsChanged(
    SharedConcreteProps const& props) {
  CppComponentInstance::onPropsChanged(props);

  if (!m_props || m_props->title != props->title ||
      m_props->titleColor != props->titleColor) {
    this->createOrUpdateRefreshIndicatorTitle(props->title, props->titleColor);
  };

  if (props->rawProps.count("progressBackgroundColor") > 0) {
    auto progressBgColor = props->rawProps["progressBackgroundColor"];
    if (progressBgColor != nullptr &&
        facebook::react::isColorMeaningful(progressBgColor.asInt())) {
      m_refreshIndicatorBackgroundNode.setBackgroundColor(
          progressBgColor.asInt());
    } else {
      m_refreshIndicatorBackgroundNode.setBackgroundColor(
          facebook::react::clearColor());
    }
  };

  if (props->rawProps.count("size") > 0) {
    auto size = props->rawProps["size"];
    if (size != nullptr && size.asString() == "large") {
      m_refreshIndicatorBackgroundNode.setSize(
          {REFRESH_INDICATOR_SIZE * 1.5, REFRESH_INDICATOR_SIZE * 1.5});
    } else {
      m_refreshIndicatorBackgroundNode.setSize(
          {REFRESH_INDICATOR_SIZE, REFRESH_INDICATOR_SIZE});
    }
  }

  m_refreshIndicatorContainerNode.setOffset(0, props->progressViewOffset);

  if (props->refreshing) {
    m_enableOnRefreshNativeEvent = false;
  }
  getLocalRootArkUINode().setNativeRefreshing(props->refreshing);

  if (facebook::react::isColorMeaningful(props->tintColor)) {
    m_refreshIndicatorSpinnerNode.setColor(props->tintColor);
  }

  if (props->rawProps.count("enabled") > 0) {
    if (props->rawProps["enabled"].isBool()) {
      getLocalRootArkUINode().setEnabled(props->rawProps["enabled"].asBool());
    } else {
      getLocalRootArkUINode().setEnabled(true);
    }
  }
}

void PullToRefreshViewComponentInstance::onRefresh() {
  m_refreshIndicatorContainerNode.setVisibility(ARKUI_VISIBILITY_VISIBLE);
  getLocalRootArkUINode().setNativeRefreshing(true);
  if (m_enableOnRefreshNativeEvent) {
    m_eventEmitter->onRefresh({});
  }
  m_enableOnRefreshNativeEvent = true;
  auto instance =
      std::static_pointer_cast<RNInstanceInternal>(m_deps->rnInstance.lock());
  if (!instance) {
    return;
  }
  instance->getTaskExecutor()->runTask(
      TaskThread::JS,
      [wptr = this->weak_from_this(), wInstance = instance->weak_from_this()] {
        auto ptr = std::static_pointer_cast<PullToRefreshViewComponentInstance>(
            wptr.lock());
        
        auto instance =
            std::static_pointer_cast<RNInstanceInternal>(wInstance.lock());

        if (!ptr || !instance || !ptr->m_props || ptr->m_props->refreshing) {
          return;
        }
        instance->getTaskExecutor()->runTask(
            TaskThread::MAIN, [wptr]() mutable {
              auto ptr =
                  std::static_pointer_cast<PullToRefreshViewComponentInstance>(
                      wptr.lock());
              if (ptr && ptr->m_props && !ptr->m_props->refreshing) {
                ptr->getLocalRootArkUINode().setNativeRefreshing(false);
              }
            });
      });
}

void PullToRefreshViewComponentInstance::onRefreshStateChanged(RefreshStatus state) {
  switch (state) {
    case RefreshStatus::REFRESH_STATUS_DRAG:
      m_enableOnRefreshNativeEvent = true;
      m_refreshIndicatorContainerNode.setVisibility(ARKUI_VISIBILITY_VISIBLE);
      break;

    case RefreshStatus::REFRESH_STATUS_INACTIVE:
    case RefreshStatus::REFRESH_STATUS_DONE:
      getLocalRootArkUINode().setNativeRefreshing(false);
      m_refreshIndicatorContainerNode.setVisibility(ARKUI_VISIBILITY_HIDDEN);
      break;

    default:
      break;
  }
}

void PullToRefreshViewComponentInstance::onRefreshNodeOffsetChange(
    RefreshNode* refreshNode,
    float offset) {
  if (this->getChildren().empty()) {
    return;
  }
  const auto& firstChild = this->getChildren()[0];
  if (firstChild == nullptr) {
    return;
  }
  auto relatedScrollView =
      std::dynamic_pointer_cast<ScrollViewComponentInstance>(firstChild);
  if (relatedScrollView == nullptr) {
    return;
  }
  relatedScrollView->onPullToRefreshOffsetChange(offset);
};

facebook::react::Point PullToRefreshViewComponentInstance::getCurrentOffset()
    const {
  if (!this->getChildren().empty() && this->getChildren()[0] != nullptr) {
    auto scrollComponent = this->getChildren()[0];
    auto scrollPosition =
        scrollComponent->getLocalRootArkUINode().getLayoutPosition();
    auto pointScaleFactor = m_layoutMetrics.pointScaleFactor;

    return {
        -scrollPosition.x / pointScaleFactor,
        -scrollPosition.y / pointScaleFactor};
  }

  return {0, 0};
}

 void PullToRefreshViewComponentInstance::setRefreshPullDownRation(float
 pullDownRatio) {
   getLocalRootArkUINode().setRefreshPullDownRation(pullDownRatio);
 }