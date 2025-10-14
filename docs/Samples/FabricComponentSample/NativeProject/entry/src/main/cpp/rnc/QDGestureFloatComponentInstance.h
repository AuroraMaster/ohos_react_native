/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "RNOH/arkui/StackNode.h"
#include "RNOH/arkui/ScrollNode.h"
#include "RNOH/arkui/ColumnNode.h"
#include "RNOH/generated/components/BaseQDGestureFloatComponentInstance.h"

namespace rnoh {
class QDGestureFloatComponentInstance : public BaseQDGestureFloatComponentInstance, ScrollNodeDelegate {
  using Super = BaseQDGestureFloatComponentInstance;

private:
  // 根容器
  StackNode m_scrollContainerNode;
  // scroll 组件
  ScrollNode m_scrollNode;
  // scroll 内容
  ColumnNode m_scrollContentNode;
  // 上方透明区域
  StackNode m_transparentNode;
  // 内容区域
  StackNode m_contentNode;
  
  // 组件宽高
  float m_width;
  float m_height;
  bool m_isInit = false;
  // 停止百分比、吸顶百分比
  float m_stopPercent = 0.75f;
  float m_stopPercentMax = 0.85f;
public:
  QDGestureFloatComponentInstance(Context context);

  StackNode &getLocalRootArkUINode();

  void onPropsChanged(SharedConcreteProps const &props);

  void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;
  void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;

  facebook::react::Point getCurrentOffset() const override;
  
  void onScroll() override;
  void onScrollStop() override;
  void onScrollToCommand(float offsetY, bool animated) override;
};
} // namespace rnoh