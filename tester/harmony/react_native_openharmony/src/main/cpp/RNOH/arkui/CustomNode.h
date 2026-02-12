/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "ArkUINode.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// When the enum is available, use it; otherwise use the known integer value
#ifdef NODE_ON_CUSTOM_OVERFLOW_SCROLL
  constexpr int CUSTOM_OVERFLOW_SCROLL_EVENT = static_cast<int>(ArkUI_NodeEventType::NODE_ON_CUSTOM_OVERFLOW_SCROLL);
#else
  // Known value for NODE_ON_CUSTOM_OVERFLOW_SCROLL in API 23+
  // This needs to match the actual enum value from the SDK
  constexpr int CUSTOM_OVERFLOW_SCROLL_EVENT = 34; // Placeholder - needs actual value
#endif

namespace rnoh {
class CustomNodeDelegate {
 public:
  virtual ~CustomNodeDelegate() = default;
  virtual void onClick(){};
  virtual void onHoverIn(){};
  virtual void onHoverOut(){};
  virtual void onScroll(facebook::react::Tag nodeId, float offset) {};
};

struct UserCallback {
  std::function<void(ArkUI_NodeCustomEvent* event)> callback;
};

class CustomNode : public ArkUINode {
 protected:
  CustomNodeDelegate* m_customNodeDelegate;
  int32_t m_activeScrollChildId = -1;

 public:
  explicit CustomNode(const ArkUINode::Context::Shared& context = nullptr);
  ~CustomNode() override;

  void insertChild(ArkUINode& child, std::size_t index);
  void addChild(ArkUINode &child);
  void removeChild(ArkUINode& child);
  void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs& eventArgs)
      override;
  void onClick();
  void setCustomNodeDelegate(CustomNodeDelegate* customNodeDelegate);
  CustomNode& setAlign(int32_t align);
  CustomNode& setFocusable(bool focusable);
  void onMeasure(ArkUI_NodeCustomEventType eventType);
  void onLayout();
 private:
  UserCallback *userCallback_ = nullptr;
  void (*eventReceiver)(ArkUI_NodeCustomEvent* event);
  // Debounce timer for insertChild
  std::thread m_debounceThread;
  std::atomic<bool> m_timerRunning{false};
  std::atomic<bool> m_timerCancelled{false};
  std::mutex m_timerMutex;
  std::condition_variable m_timerCV;
  
  void startDebounceTimer();
  void stopDebounceTimer();
  void debounceTimerFunc();
};

} // namespace rnoh