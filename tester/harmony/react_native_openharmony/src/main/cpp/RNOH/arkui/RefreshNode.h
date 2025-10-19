/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include "ArkUINode.h"
#include "NativeNodeApi.h"

class RefreshNodeDelegate {
 public:
  virtual ~RefreshNodeDelegate() noexcept(false) = default;
  virtual void onRefresh(){};
  
  enum class RefreshStatus {
    REFRESH_STATUS_INACTIVE = 0,
    REFRESH_STATUS_DRAG = 1,
    REFRESH_STATUS_OVERDRAG = 2,
    REFRESH_STATUS_REFRESH = 3,
    REFRESH_STATUS_DONE = 4,
  };
  virtual void onRefreshStateChanged(RefreshStatus state) {};
};

namespace rnoh {
class RefreshNode : public ArkUINode {
 protected:
  RefreshNodeDelegate* m_refreshNodeDelegate;
  static constexpr float REFRESH_NODE_SIZE = 29;

 public:
  explicit RefreshNode(const ArkUINode::Context::Shared& context = nullptr);
  ~RefreshNode();

  void insertChild(ArkUINode& child, std::size_t index);
  void removeChild(ArkUINode& child);
  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs);
  RefreshNode& setNativeRefreshing(bool isRefreshing);
  RefreshNode& setRefreshNodeDelegate(RefreshNodeDelegate* refreshNodeDelegate);
  RefreshNode& setRefreshContent(ArkUINode& refreshContent);
  RefreshNode& setRefreshPullDownRation(float pullDownRatio);
};
} // namespace rnoh