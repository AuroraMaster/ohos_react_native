/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

/**
 * @architecture: C-API
 */
#pragma once
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/EventEmitter.h>
#include <react/renderer/core/LayoutMetrics.h>
#include <react/renderer/core/Props.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <react/renderer/core/State.h>
#include <vector>
#include "RNOH/ArkTSChannel.h"
#include "RNOH/ArkTSMessageHub.h"
#include "RNOH/RNInstance.h"
#include "RNOH/TouchTarget.h"
#include "RNOH/arkui/ArkUINode.h"
#include "RNOH/arkui/UIInputEventHandler.h"
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/ImageSourceResolver.h"

namespace rnoh {

class MountingManagerCAPI;

/**
 * @api
 */
class ComponentInstance
    : public TouchTarget,
      public std::enable_shared_from_this<ComponentInstance> {
 private:
  std::string m_componentName;
  std::unordered_set<std::string> m_nativeResponderBlockOrigins;
  
  /**
   * @brief Mark whether the current component is a JS responder.
   * @param isJSResponder
   */
  virtual void setIsJSResponder(bool isJSResponder){};

 protected:
  using Tag = facebook::react::Tag;
  using ComponentHandle = facebook::react::ComponentHandle;

 public:
  using Shared = std::shared_ptr<ComponentInstance>;
  using Weak = std::weak_ptr<ComponentInstance>;

  class Registry {
   public:
   using Weak = std::weak_ptr<Registry>;
    virtual ComponentInstance::Shared findById(const std::string& id) const = 0;
  };

  friend MountingManagerCAPI;

  struct Dependencies {
    using Shared = std::shared_ptr<const Dependencies>;

    ArkTSChannel::Shared arkTSChannel;
    ArkTSMessageHub::Shared arkTSMessageHub;
    RNInstance::Weak rnInstance;
    ImageSourceResolver::Shared imageSourceResolver;
    Registry::Weak componentInstanceRegistry;
  };

  struct Context {
    Tag tag;
    ComponentHandle componentHandle;
    std::string componentName;
    Dependencies::Shared dependencies;
    ArkUINode::Context::Shared arkUINodeContext;
  };

  virtual ArkUINode& getLocalRootArkUINode() = 0;

  ComponentInstance(Context ctx);

  virtual ~ComponentInstance() noexcept(false) = default;

  /**
   * In C++ virtual methods shouldn't be called from constructor. Override this
   * method to bypass this limitation.
   */
  virtual void onCreate() {}

  Tag getTag() const {
    return m_tag;
  }
    
  virtual std::string getCompId() = 0;

  std::string getId() const {
    auto props = getProps();
    if (props == nullptr) {
      return "";
    }
    return props->nativeId;
  }

  const std::string getComponentName() const {
    return m_componentName;
  }

  ComponentHandle getComponentHandle() const {
    return m_componentHandle;
  }

  /**
   * @internal
   */
  void insertChild(
      ComponentInstance::Shared childComponentInstance,
      std::size_t index);

  /**
   * @internal
   */
  void removeChild(ComponentInstance::Shared childComponentInstance);

  virtual facebook::react::Props::Shared getProps() const = 0;
  
    
  virtual bool getAccessibilityGroup() const = 0;

  /**
   * TODO: change to protected — those methods are intended to be called
   * only by MountingManagerCAPI which is a friend of this class
   * (latestRNOHVersion: 0.72.27)
   */
 public:
  /**
   * @deprecated This is internal method. Do not use it, it will be private in
   * the future (latestRNOHVersion: 0.72.27)
   */
  virtual void setProps(facebook::react::Props::Shared props){};
  /**
   * @deprecated This is internal method. Do not use it, it will be private in
   * the future (latestRNOHVersion: 0.72.27)
   */
  virtual void setState(facebook::react::State::Shared state){};
  /**
   * @deprecated This is internal method. Do not use it, it will be private in
   * the future (latestRNOHVersion: 0.72.27)
   */
  virtual void setLayout(facebook::react::LayoutMetrics layoutMetrics){};
  /**
   * @deprecated This is internal method. Do not use it, it will be private in
   * the future (latestRNOHVersion: 0.72.27)
   */
  virtual void setEventEmitter(
      facebook::react::SharedEventEmitter eventEmitter){};
  /**
   * @deprecated This is internal method. Do not use it, it will be private in
   * the future (latestRNOHVersion: 0.72.27)
   */
  virtual void handleCommand(
      std::string const& commandName,
      folly::dynamic const& args) {
    this->onCommandReceived(commandName, args);
  }
  /**
   * @deprecated This is internal method. Do not use it, it will be private in
   * the future (latestRNOHVersion: 0.72.27)
   */
  virtual void finalizeUpdates() {
    this->onFinalizeUpdates();
  }

 public:
  /**
   * This method is necessary to support "aria-labelledby" and
   * "accessibilityLabelledBy" props.
   */
  virtual const std::string& getAccessibilityLabel() const {
    static const std::string empty = "";
    return empty;
  }

  virtual std::vector<ComponentInstance::Shared> const& getChildren() const {
    return m_children;
  }

  virtual facebook::react::LayoutMetrics& getLayoutMetrics() {
    return m_layoutMetrics;
  }

  virtual ComponentInstance::Weak const getParent() const {
    return m_parent;
  }

  /**
   * @internal
   */
  virtual void setParent(ComponentInstance::Shared parent) {
    m_parent = parent;
  }

  virtual std::size_t const getIndex() const {
    return m_index;
  }

  virtual void setIndex(std::size_t index) {
    m_index = index;
  }

  virtual bool checkUpdateBaseNode() {
    return false;
  }

  virtual void setShadowView(facebook::react::ShadowView const& shadowView) {}

  virtual facebook::react::ShadowView& getShadowView() {
    return m_shadowView;
  }

  void setNativeResponderBlocked(
      bool blocked,
      const std::string& origin = "REACT_NATIVE") {
    if (blocked) {
      m_nativeResponderBlockOrigins.insert(origin);
    } else if (
        m_nativeResponderBlockOrigins.find(origin) !=
        m_nativeResponderBlockOrigins.end()) {
      m_nativeResponderBlockOrigins.erase(origin);
    }
    onNativeResponderBlockChange(m_nativeResponderBlockOrigins.size() > 0);
  }

  /**
   * @brief The JSResponder is the view currently recognized by the JavaScript
   * context as the "responder". It is part of the responder system and
   * represents the view that exclusively handles touch interactions,
   * responsible for receiving and processing user touch events.
   * @return bool
   */
  bool isJSResponder() const override {
    return false;
  };
  
  // TouchTarget implementation
  Tag getTouchTargetTag() const override {
    return getTag();
  }

  facebook::react::SharedTouchEventEmitter getTouchEventEmitter()
      const override {
    return nullptr;
  }

  virtual std::vector<TouchTarget::Shared> getTouchTargetChildren() override {
    auto children = getChildren();
    return std::vector<TouchTarget::Shared>(children.begin(), children.end());
  }

  /**
   * @internal
   */
  void setIgnoredPropKeys(std::unordered_set<std::string> propKeys) {
    m_ignoredPropKeys = std::move(propKeys);
  }

  /**
   * @internal
   */
  std::unordered_set<std::string> const& getIgnoredPropKeys() const {
    return m_ignoredPropKeys;
  }

  virtual bool isRefreshControlComponentInstance() const {
    return false;
  }

 protected:
  virtual void onChildInserted(
      ComponentInstance::Shared const& childComponentInstance,
      std::size_t index) {}

  virtual void onChildRemoved(
      ComponentInstance::Shared const& childComponentInstance) {}

  virtual void onFinalizeUpdates() {}

  virtual void onCommandReceived(
      std::string const& commandName,
      folly::dynamic const& args) {}

  /**
   * Override this method if your component has scrollable functionality.
   * Temporarily blocking the scrolling is required to support continuous
   * gestures like panning within these scrollable components.
   *
   * @param isBlocked A boolean indicating whether scrolling should be blocked
   * or not.
   */
  virtual void onNativeResponderBlockChange(bool isBlocked) {}

  Tag m_tag;
  bool m_isRadiusSetValid = 0;
  float m_oldPointScaleFactor = 0.0f;
  ComponentHandle m_componentHandle;
  std::vector<ComponentInstance::Shared> m_children;
  ComponentInstance::Weak m_parent;
  std::size_t m_index = 0;
  facebook::react::BorderMetrics m_oldBorderMetrics;
  facebook::react::LayoutMetrics m_layoutMetrics;
  Dependencies::Shared m_deps;
  ArkUINode::Context::Shared m_arkUINodeCtx;
  std::unordered_set<std::string> m_ignoredPropKeys;
  facebook::react::ShadowView m_shadowView;
  int m_rnInstanceId = 0;
};

} // namespace rnoh