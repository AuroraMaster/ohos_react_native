/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/rncore/EventEmitters.h>
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/ColumnNode.h"
#include "RNOH/arkui/LoadingProgressNode.h"
#include "RNOH/arkui/RefreshNode.h"
#include "RNOH/arkui/StackNode.h"
#include "RNOH/arkui/TextNode.h"
#include "react/renderer/components/rncore/ShadowNodes.h"

namespace rnoh {

class PullToRefreshViewComponentInstance
    : public CppComponentInstance<facebook::react::PullToRefreshViewShadowNode>,
      public RefreshNodeDelegate {
 private:
  static constexpr float REFRESH_INDICATOR_SIZE = 29;

  RefreshNode m_refreshNode;
  bool m_enableOnRefreshNativeEvent{true};

  ColumnNode m_refreshIndicatorContainerNode;
  StackNode m_refreshIndicatorBackgroundNode;
  LoadingProgressNode m_refreshIndicatorSpinnerNode;
  TextNode m_refreshIndicatorTitleNode;
  void createRefreshIndicatorTitle(
      const std::string& refreshIndicatorTitleText,
      facebook::react::SharedColor const& refreshIndicatorTitleColor);
  void createOrUpdateRefreshIndicatorTitle(
      const std::string& refreshIndicatorTitleText,
      facebook::react::SharedColor const& refreshIndicatorTitleColor);
  void updateRefreshIndicatorTitleNode(
      const std::string& title,
      facebook::react::SharedColor const& color);

 public:
  PullToRefreshViewComponentInstance(Context context);

  RefreshNode& getLocalRootArkUINode() override;

  void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) override;
  void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) override;
  void onPropsChanged(SharedConcreteProps const& props) override;
  void onRefresh() override;
  void onRefreshStateChanged(RefreshStatus state) override;
  facebook::react::Point getCurrentOffset() const override;

  bool isRefreshControlComponentInstance() const override {
    return true;
  }

  void setRefreshPullDownRation(float pullDownRatio);

};
} // namespace rnoh