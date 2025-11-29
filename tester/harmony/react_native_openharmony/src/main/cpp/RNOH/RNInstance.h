/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <rawfile/raw_file_manager.h>
#include <functional>
#include <memory>

#include <cxxreact/Instance.h>
#include <cxxreact/ModuleRegistry.h>
#include <cxxreact/NativeModule.h>
#include <folly/dynamic.h>
#include <react/renderer/animations/LayoutAnimationDriver.h>
#include <react/renderer/scheduler/Scheduler.h>

#include "RNOH/ArkTSChannel.h"
#include "RNOH/Assert.h"
#include "RNOH/EventEmitRequestHandler.h"
#include "RNOH/MessageQueueThread.h"
#include "RNOH/MutationsToNapiConverter.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "RNOH/TouchTarget.h"
#include "RNOH/arkui/UIInputEventHandler.h"
#include "RNOH/ArkTSBridge.h"

namespace rnoh {
class TurboModule;
using MutationsListener = std::function<void(
    MutationsToNapiConverter const&,
    facebook::react::ShadowViewMutationList const& mutations)>;
using SharedNativeResourceManager = std::shared_ptr<NativeResourceManager>;

class Surface {
 public:
  /**
   * @api
   */
  struct LayoutContext {
    /**
     * @internal
     */
    static LayoutContext from(facebook::react::LayoutContext layoutContext) {
      return {.viewportOffset = layoutContext.viewportOffset};
    }

    facebook::react::Point viewportOffset{};
  };

  using Weak = std::weak_ptr<Surface>;

  virtual LayoutContext getLayoutContext() = 0;
  virtual DisplayMetrics getDisplayMetrics() = 0;
};

class RNInstance {
  using ContextContainer = facebook::react::ContextContainer;

  public:
  virtual std::optional<Surface::Weak> getSurfaceByRootTag(
      facebook::react::Tag rootTag) {
    RNOH_ASSERT_MSG(false, "Not implemented");
    return std::nullopt;
  };

  using Weak = std::weak_ptr<RNInstance>;

  /**
   * @actor RNOH_LIBRARY
   * @brief Prevents locking RNInstance if it's about to be destroyed,
   * avoiding crashes when the last shared_ptr is held on main thread.
   */
  class SafeWeak {
   public:
    SafeWeak() = default;

    SafeWeak(
        const std::weak_ptr<RNInstance>& instance,

        const std::weak_ptr<std::atomic<bool>>& destroyFlag)
        : m_weak(instance), m_destroyFlag(destroyFlag) {}

    std::shared_ptr<RNInstance> lock() const {
      auto flag = m_destroyFlag.lock();
      if (!flag || flag->load()) {
        return nullptr;
      }

      return m_weak.lock();
    }

    bool expired() const {
      return m_weak.expired();
    }

    void reset() {
      m_weak.reset();
    }

    long use_count() const {
      return m_weak.use_count();
    }

   private:
    std::weak_ptr<RNInstance> m_weak;
    std::weak_ptr<std::atomic<bool>> m_destroyFlag;
  };

  virtual ~RNInstance();

  virtual ContextContainer const& getContextContainer() const = 0;
  virtual std::shared_ptr<TurboModule> getTurboModule(
      const std::string& name) = 0;
  template <typename T>
  std::shared_ptr<T> getTurboModule(const std::string& name) {
    auto turboModule = getTurboModule(name);
    return std::dynamic_pointer_cast<T>(turboModule);
  }
  virtual std::string getCacheDir() = 0;
  virtual NativeResourceManager const* getNativeResourceManager() const = 0;
  virtual void synchronouslyUpdateViewOnUIThread(
      facebook::react::Tag tag,
      folly::dynamic props) = 0;
  virtual void postMessageToArkTS(
      const std::string& name,
      folly::dynamic const& payload) = 0;
  virtual void callJSFunction(
      std::string&& module,
      std::string&& method,
      folly::dynamic&& params) = 0;
  virtual int getId() {
    return 0;
  };
};

class RNInstanceInternal : public RNInstance,
                           public std::enable_shared_from_this<RNInstance> {
 public:
    explicit RNInstanceInternal(
        int id,
        std::shared_ptr<facebook::react::ContextContainer> contextContainer,
        std::shared_ptr<TaskExecutor> taskExecutor,
        SharedNativeResourceManager nativeResourceManager,
        std::string hspModuleName);

  virtual ~RNInstanceInternal() = default;

  virtual TaskExecutor::Shared getTaskExecutor() = 0;
  virtual void start() = 0;
  virtual void createSurface(
      facebook::react::Tag surfaceId,
      std::string const& moduleName) = 0;
  virtual facebook::react::Size measureSurface(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      bool isRTL) = 0;
  virtual void updateSurfaceConstraints(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      bool isRTL) = 0;
  virtual void startSurface(
      facebook::react::Tag surfaceId,
      float minWidth,
      float minHeight,
      float maxWidth,
      float maxHeight,
      float viewportOffsetX,
      float viewportOffsetY,
      float pixelRatio,
      bool isRTL,
      folly::dynamic&& initialProps) = 0;
  virtual void setSurfaceProps(
      facebook::react::Tag surfaceId,
      folly::dynamic&& props) = 0;
  virtual void stopSurface(facebook::react::Tag surfaceId, 
    std::function<void()> onStop) = 0;
  virtual void destroySurface(facebook::react::Tag surfaceId) = 0;
  virtual void setSurfaceDisplayMode(
      facebook::react::Tag surfaceId,
      facebook::react::DisplayMode displayMode) = 0;
  virtual void emitComponentEvent(
      napi_env env,
      facebook::react::Tag tag,
      std::string eventName,
      napi_value payload) = 0;
  virtual void onMemoryLevel(size_t memoryLevel) = 0;
  virtual void updateState(
      napi_env env,
      std::string const& componentName,
      facebook::react::Tag tag,
      napi_value newState) = 0;
  virtual void handleArkTSMessage(
      const std::string& name,
      folly::dynamic const& payload) = 0;
    void setBundlePath(std::string const& path);
    std::string getBundlePath();
    virtual void registerFont(
        std::string const& fontFamily,
        std::string const& fontFilePath) = 0;
  virtual void setJavaScriptExecutorFactory(
      std::shared_ptr<facebook::react::JSExecutorFactory> jsExecutorFactory) = 0;

    void loadScriptFromBuffer(
        std::vector<uint8_t> bundle,
        std::string const sourceURL,
        std::function<void(const std::string)> onFinish);
    void loadScriptFromFile(
        std::string const fileURL,
        std::function<void(const std::string)> onFinish);
    void loadScriptFromRawFile(
        std::string const rawFileURL,
        std::function<void(const std::string)> onFinish);
    void loadScript(
        std::unique_ptr<facebook::react::JSBigString const>,
        std::string const sourceURL,
        std::function<void(const std::string)> onFinish);
    NativeResourceManager const *getNativeResourceManager() const;
    void onCreate();
    void markSelfAboutToDestroyed();
    std::string getHspModuleName();

    TaskExecutor::Shared taskExecutor;

protected:
    int m_id;
    std::shared_ptr<std::atomic<bool>> m_isAboutToBeDestroyed;
    RNInstance::SafeWeak m_weakSelf;
    facebook::react::ContextContainer::Shared m_contextContainer;
    std::unique_ptr<facebook::react::SchedulerDelegate> m_schedulerDelegate = nullptr;
    SharedNativeResourceManager m_nativeResourceManager;
    std::string m_bundlePath;
    std::string m_hspModuleName;
    /**
     * NOTE: Order matters. m_scheduler holds indirectly jsi::Values.
     * These values must be destructed before the runtime.
     * The runtime is destructed when `m_reactInstance` is destructed.
     * Therefore, `m_scheduler` must be declared after `m_reactInstance`.
     */
    std::shared_ptr<facebook::react::Instance> instance;
    std::shared_ptr<facebook::react::Scheduler> scheduler = nullptr;
};

} // namespace rnoh
