/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "TimingTurboModule.h"
#include <algorithm>
#include <chrono>
#include <limits>
#include "RNOH/Assert.h"
#include "RNOH/Performance/HarmonyReactMarker.h"
#include "RNOH/RNInstance.h"

namespace rnoh {

using namespace facebook;

static jsi::Value __hostFunction_TimingTurboModule_createTimer(
    jsi::Runtime& /*rt*/,
    react::TurboModule& turboModule,
    const jsi::Value* args,
    size_t count) {
  RNOH_ASSERT(count == 4);
  auto& self = static_cast<TimingTurboModule&>(turboModule);
  auto id = args[0].getNumber();
  auto duration = args[1].getNumber();
  auto jsSchedulingTIme = args[2].getNumber();
  auto repeats = args[3].getBool();
  self.createTimer(id, duration, jsSchedulingTIme, repeats);
  return jsi::Value::undefined();
}
static jsi::Value __hostFunction_TimingTurboModule_deleteTimer(
    jsi::Runtime& /*rt*/,
    react::TurboModule& turboModule,
    const jsi::Value* args,
    size_t count) {
  RNOH_ASSERT(count == 1);
  auto id = args[0].getNumber();
  static_cast<TimingTurboModule&>(turboModule).deleteTimer(id);
  return jsi::Value::undefined();
}
static jsi::Value __hostFunction_TimingTurboModule_setSendIdleEvents(
    jsi::Runtime& /*rt*/,
    react::TurboModule& turboModule,
    const jsi::Value* args,
    size_t count) {
  RNOH_ASSERT(count == 1);
  auto sendIdleEvents = args[0].getBool();
  static_cast<TimingTurboModule&>(turboModule)
      .setSendIdleEvents(sendIdleEvents);
  return jsi::Value::undefined();
}

using TimePoint = decltype(std::chrono::system_clock::now());

double getMillisSinceEpoch() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

TimingTurboModule::TimingTurboModule(
    const ArkTSTurboModule::Context ctx,
    const std::string name)
    : ArkTSTurboModule(ctx, name) {
  methodMap_ = {
      {"createTimer", {4, __hostFunction_TimingTurboModule_createTimer}},
      {"deleteTimer", {1, __hostFunction_TimingTurboModule_deleteTimer}},
      {"setSendIdleEvents",
       {1, __hostFunction_TimingTurboModule_setSendIdleEvents}},
  };

  // LifecycleObserver must be created on the main thread
  m_ctx.taskExecutor->runTask(
      TaskThread::MAIN, [weakInstance = m_ctx.safeInstance] {
        auto instance = weakInstance.lock();
        if (!instance) {
          return;
        }
        // since this task is scheduled from `TimingTurboModule` constructor,
        // we cannot capture `weak_from_this()` into the task, and must use
        // RNInstance to get the TurboModule instead
        auto self = instance->getTurboModule<TimingTurboModule>("Timing");
        if (!self) {
          return;
        }
        if (self->m_lifecycleObserver) {
          return;
        }
        auto messageHub = self->m_ctx.arkTSMessageHub;
        self->m_lifecycleObserver = std::make_shared<LifecycleObserver>(
            self->weak_from_this(), messageHub);
      });
}

TimingTurboModule::~TimingTurboModule() {
  if (m_wakeUpTask.has_value()) {
    m_ctx.taskExecutor->cancelDelayedTask(m_wakeUpTask.value());
  }
  m_ctx.taskExecutor->runTask(
      TaskThread::MAIN,
      [lifecycleObserver = std::move(m_lifecycleObserver)] {});
}

void TimingTurboModule::createTimer(
    double id,
    double duration,
    double jsSchedulingTime,
    bool repeats) {
  assertJSThread();

  // for short-lived, one-off timers, schedule them immediately after they are
  // created
  if (duration == 0 && !repeats) {
    triggerTimers({id});
    return;
  }

  auto deadline = jsSchedulingTime + duration;
  m_activeTimerById.emplace(id, Timer{id, deadline, duration, repeats});

  if (isForeground && !m_vsyncListener->isScheduled()) {
    scheduleWakeUp();
  }
}

void TimingTurboModule::deleteTimer(double id) {
  assertJSThread();
  m_activeTimerById.erase(id);
}

void TimingTurboModule::setSendIdleEvents(bool /*sendIdleEvents*/) {
  LOG(WARNING) << "TimingTurboModule::setSendIdleEvents is not implemented";
}

void TimingTurboModule::onForeground() {
  m_ctx.taskExecutor->runTask(TaskThread::JS, [weakSelf = weak_from_this()] {
    auto self = weakSelf.lock();
    if (!self) {
      return;
    }
    if (!self->isForeground) {
      self->isForeground = true;
      self->resumeTimers();
    }
  });
}

void TimingTurboModule::onBackground() {
  m_ctx.taskExecutor->runTask(TaskThread::JS, [weakSelf = weak_from_this()] {
    auto self = weakSelf.lock();
    if (!self) {
      return;
    }
    self->isForeground = false;
    self->pauseTimers();
  });
}

void TimingTurboModule::LifecycleObserver::onMessageReceived(
    ArkTSMessage const& message) {
  auto timingModule = m_timingTurboModule.lock();
  if (!timingModule) {
    return;
  }

  if (message.name == "FOREGROUND") {
    timingModule->onForeground();
  } else if (message.name == "BACKGROUND") {
    timingModule->onBackground();
  }
}

void TimingTurboModule::triggerTimers(std::vector<double> const& timerIds) {
  assertJSThread();
  auto instance = m_ctx.safeInstance.lock();
  auto now = getMillisSinceEpoch();
  if (instance) {
    instance->callJSFunction(
        "JSTimers",
        "callTimers",
        folly::dynamic::array(folly::dynamic::array(timerIds.begin(), timerIds.end())));
  }
  for (auto id : timerIds) {
    auto it = m_activeTimerById.find(id);
    if (it == m_activeTimerById.end()) {
      continue;
    }

    auto& timer = it->second;

    if (timer.repeats) {
      timer.deadline = now + timer.duration;
    } else {
      m_activeTimerById.erase(it);
    }
  }
}

void TimingTurboModule::resumeTimers() {
  assertJSThread();
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::ON_HOST_RESUME_START);
  triggerExpiredTimers();
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::ON_HOST_RESUME_END);
}

void TimingTurboModule::pauseTimers() {
  assertJSThread();
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::ON_HOST_PAUSE_START);
  cancelWakeUp();
  HarmonyReactMarker::logMarker(
      HarmonyReactMarker::HarmonyReactMarkerId::ON_HOST_PAUSE_END);
}

void TimingTurboModule::assertJSThread() const {
  RNOH_ASSERT(m_ctx.taskExecutor->isOnTaskThread(TaskThread::JS));
}

void TimingTurboModule::triggerExpiredTimers() {
  assertJSThread();
  if (!isForeground) {
    return;
  }
  std::vector<Timer> expiredTimers;
  auto now = getMillisSinceEpoch();
  // NOTE: a possible optimization is to keep a std::priority_queue of timer ids
  // here, ordered by the deadline of the timer. We assume there's only a few
  // timers scheduled at each time, and the overhead of maintaining the queue is
  // not worth it
  for (auto const& [id, timer] : m_activeTimerById) {
    if (timer.deadline <= now) {
      expiredTimers.push_back(timer);
    }
  }
  if (!expiredTimers.empty()) {
    // timers with earlier deadlines should fire sooner
    std::sort(expiredTimers.begin(), expiredTimers.end(), [](auto a, auto b) {
      return a.deadline < b.deadline;
    });
    std::vector<double> expiredTimerIds;
    std::transform(
        expiredTimers.begin(),
        expiredTimers.end(),
        std::back_inserter(expiredTimerIds),
        [](auto timer) { return timer.id; });
    triggerTimers(expiredTimerIds);
  }

  if (!m_activeTimerById.empty()) {
    scheduleWakeUp();
  }
}

void TimingTurboModule::cancelWakeUp() {
  assertJSThread();
  m_nextTimerDeadline = std::numeric_limits<double>::max();
  if (m_wakeUpTask.has_value()) {
    m_ctx.taskExecutor->cancelDelayedTask(m_wakeUpTask.value());
    m_wakeUpTask.reset();
  }
}

double TimingTurboModule::getNextDeadline() {
  double nextDeadline = std::numeric_limits<double>::max();

  for (auto const& [id, timer] : m_activeTimerById) {
    nextDeadline = std::min(timer.deadline, nextDeadline);
  }
  return nextDeadline;
}

void TimingTurboModule::scheduleWakeUp() {
  // NOTE: following the iOS implementation, if there's a scheduled timer which
  // will expire soon (< 1s), we don't schedule a delayed task on the executor,
  // checking the scheduled timers on the next few frames instead.
  constexpr double MINIMUM_SLEEP_DELAY = 1000.;

  auto nextDeadline = getNextDeadline();
  auto now = getMillisSinceEpoch();
  auto delay = nextDeadline - now;

  if (delay >= MINIMUM_SLEEP_DELAY) {
    if (nextDeadline >= m_nextTimerDeadline) {
      return;
    }
    m_nextTimerDeadline = nextDeadline;
    cancelWakeUp();
    m_wakeUpTask = m_ctx.taskExecutor->runDelayedTask(
        TaskThread::JS,
        [weakSelf = weak_from_this()] {
          if (auto self = weakSelf.lock()) {
            self->m_nextTimerDeadline = std::numeric_limits<double>::max();
            self->triggerExpiredTimers();
          }
        },
        std::max(delay, 0.));
  } else {
    m_vsyncListener->requestFrame([weakSelf = weak_from_this()](auto) {
      if (auto self = weakSelf.lock()) {
        self->m_ctx.taskExecutor->runTask(TaskThread::JS, [weakSelf] {
          if (auto self = weakSelf.lock()) {
            self->triggerExpiredTimers();
          }
        });
      }
    });
  }
}

} // namespace rnoh