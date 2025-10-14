/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <limits>
#include <optional>
#include <unordered_map>
#include "RNOH/ArkTSMessageHub.h"
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/VSyncListener.h"

namespace rnoh {

class JSI_EXPORT TimingTurboModule
    : public ArkTSTurboModule,
      public std::enable_shared_from_this<TimingTurboModule> {
 public:
  TimingTurboModule(
      const ArkTSTurboModule::Context ctx,
      const std::string name);

  ~TimingTurboModule() override;

  void createTimer(
      double id,
      double duration,
      double jsSchedulingTime,
      bool repeats);
  void deleteTimer(double id);
  void setSendIdleEvents(bool sendIdleEvents);

  void onForeground();
  void onBackground();

 private:
  using Weak = std::weak_ptr<TimingTurboModule>;
  using Shared = std::shared_ptr<TimingTurboModule>;

  /**
   * @thread: MAIN
   * This class listens for lifecycle events and pauses/resumes timers.
   */
  class LifecycleObserver : public ArkTSMessageHub::Observer {
   public:
    LifecycleObserver(
        Weak timingTurboModule,
        ArkTSMessageHub::Shared const& messageHub)
        : ArkTSMessageHub::Observer(messageHub),
          m_timingTurboModule(timingTurboModule) {}
    void onMessageReceived(ArkTSMessage const& message) override;

   private:
    Weak m_timingTurboModule;
  };

  void triggerExpiredTimers();
  void triggerTimers(std::vector<double> const& timerIds);
  void resumeTimers();
  void pauseTimers();
  void scheduleWakeUp();
  void cancelWakeUp();
  double getNextDeadline();

  void assertJSThread() const;

  struct Timer {
    double id;
    double deadline;
    double duration;
    bool repeats;
  };

  bool isForeground{true};
  std::shared_ptr<VSyncListener> m_vsyncListener =
      std::make_shared<VSyncListener>("TimingTurboModule");
  std::optional<TaskExecutor::DelayedTask> m_wakeUpTask = std::nullopt;
  double m_nextTimerDeadline = std::numeric_limits<double>::max();
  std::unordered_map<double, Timer> m_activeTimerById{};
  std::shared_ptr<LifecycleObserver> m_lifecycleObserver = nullptr;
};

} // namespace rnoh