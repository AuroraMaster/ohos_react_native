/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "UIManagerBinding.h"

#include <glog/logging.h>
#include <jsi/JSIDynamic.h>
#include <react/debug/react_native_assert.h>
#include <react/renderer/core/LayoutableShadowNode.h>
#include <react/renderer/core/TraitCast.h>
#include <react/renderer/debug/SystraceSection.h>
#include <react/renderer/runtimescheduler/RuntimeSchedulerBinding.h>
#include <react/renderer/uimanager/primitives.h>

#include <utility>

#include "bindingUtils.h"
// RNOH patch begin
#ifdef PARALLELIZATION_ON
#include <ffrt/cpp/pattern/job_partner.h>
#include "RNOH/FFRTConfig.h"
inline auto g_prehot = ffrt::submit_h(
    [] {},
    {},
    {},
    ffrt::task_attr().name("prehot").qos(ffrt::qos_user_initiated));
#endif
// RNOH patch end

namespace facebook::react {

void UIManagerBinding::createAndInstallIfNeeded(
    jsi::Runtime& runtime,
    std::shared_ptr<UIManager> const& uiManager) {
  auto uiManagerModuleName = "nativeFabricUIManager";

  auto uiManagerValue =
      runtime.global().getProperty(runtime, uiManagerModuleName);
  if (uiManagerValue.isUndefined()) {
    // The global namespace does not have an instance of the binding;
    // we need to create, install and return it.
    auto uiManagerBinding = std::make_shared<UIManagerBinding>(uiManager);
    auto object = jsi::Object::createFromHostObject(runtime, uiManagerBinding);
    runtime.global().setProperty(
        runtime, uiManagerModuleName, std::move(object));
  }
}

std::shared_ptr<UIManagerBinding> UIManagerBinding::getBinding(
    jsi::Runtime& runtime) {
  auto uiManagerModuleName = "nativeFabricUIManager";

  auto uiManagerValue =
      runtime.global().getProperty(runtime, uiManagerModuleName);
  if (uiManagerValue.isUndefined()) {
    return nullptr;
  }

  auto uiManagerObject = uiManagerValue.asObject(runtime);
  return uiManagerObject.getHostObject<UIManagerBinding>(runtime);
}

UIManagerBinding::UIManagerBinding(std::shared_ptr<UIManager> uiManager)
    : uiManager_(std::move(uiManager)) {}

UIManagerBinding::~UIManagerBinding() {
  LOG(WARNING) << "UIManagerBinding::~UIManagerBinding() was called (address: "
               << this << ").";
}

jsi::Value UIManagerBinding::getInspectorDataForInstance(
    jsi::Runtime& runtime,
    EventEmitter const& eventEmitter) const {
  auto eventTarget = eventEmitter.eventTarget_;
  EventEmitter::DispatchMutex().lock();

  if (!runtime.global().hasProperty(runtime, "__fbBatchedBridge") ||
      !eventTarget) {
    return jsi::Value::undefined();
  }

  eventTarget->retain(runtime);
  auto instanceHandle = eventTarget->getInstanceHandle(runtime);
  eventTarget->release(runtime);
  EventEmitter::DispatchMutex().unlock();

  if (instanceHandle.isUndefined()) {
    return jsi::Value::undefined();
  }

  return callMethodOfModule(
      runtime,
      "ReactFabric",
      "getInspectorDataForInstance",
      {std::move(instanceHandle)});
}

void UIManagerBinding::dispatchEvent(
    jsi::Runtime& runtime,
    EventTarget const* eventTarget,
    std::string const& type,
    ReactEventPriority priority,
    ValueFactory const& payloadFactory) const {
  SystraceSection s("UIManagerBinding::dispatchEvent", "type", type);

  auto payload = payloadFactory(runtime);

  // If a payload is null, the factory has decided to cancel the event
  if (payload.isNull()) {
    return;
  }

  auto instanceHandle = eventTarget != nullptr ? [&]() {
    auto instanceHandle = eventTarget->getInstanceHandle(runtime);
    if (instanceHandle.isUndefined()) {
      return jsi::Value::null();
    }

    // Mixing `target` into `payload`.
    if (!payload.isObject()) {
      LOG(ERROR) << "payload for dispatchEvent is not an object: "
                 << eventTarget->getTag();
    }
    react_native_assert(payload.isObject());
    payload.asObject(runtime).setProperty(
        runtime, "target", eventTarget->getTag());
    return instanceHandle;
  }()
                                               : jsi::Value::null();

  if (instanceHandle.isNull()) {
    LOG(WARNING) << "instanceHandle is null, event will be dropped";
  }

  auto& eventHandlerWrapper =
      static_cast<EventHandlerWrapper const&>(*eventHandler_);

  currentEventPriority_ = priority;
  eventHandlerWrapper.callback.call(
      runtime,
      {std::move(instanceHandle),
       jsi::String::createFromUtf8(runtime, type),
       std::move(payload)});
  currentEventPriority_ = ReactEventPriority::Default;
}

void UIManagerBinding::invalidate() const {
  uiManager_->setDelegate(nullptr);
}

jsi::Value UIManagerBinding::get(
    jsi::Runtime& runtime,
    jsi::PropNameID const& name) {
  auto methodName = name.utf8(runtime);
  SystraceSection s("UIManagerBinding::get", "name", methodName);

  // Convert shared_ptr<UIManager> to a raw ptr
  // Why? Because:
  // 1) UIManagerBinding strongly retains UIManager. The JS VM
  //    strongly retains UIManagerBinding (through the JSI).
  //    These functions are JSI functions and are only called via
  //    the JS VM; if the JS VM is torn down, those functions can't
  //    execute and these lambdas won't execute.
  // 2) The UIManager is only deallocated when all references to it
  //    are deallocated, including the UIManagerBinding. That only
  //    happens when the JS VM is deallocated. So, the raw pointer
  //    is safe.
  //
  // Even if it's safe, why not just use shared_ptr anyway as
  //  extra insurance?
  // 1) Using shared_ptr or weak_ptr when they're not needed is
  //    a pessimisation. It's more instructions executed without
  //    any additional value in this case.
  // 2) How and when exactly these lambdas is deallocated is
  //    complex. Adding shared_ptr to them which causes the UIManager
  //    to potentially live longer is unnecessary, complicated cognitive
  //    overhead.
  // 3) There is a strong suspicion that retaining UIManager from
  //    these C++ lambdas, which are retained by an object that is held onto
  //    by the JSI, caused some crashes upon deallocation of the
  //    Scheduler and JS VM. This could happen if, for instance, C++
  //    semantics cause these lambda to not be deallocated until
  //    a CPU tick (or more) after the JS VM is deallocated.
  UIManager* uiManager = uiManager_.get();

  // Semantic: Creates a new node with given pieces.
  if (methodName == "createNode") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        5,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) -> jsi::Value {
          try {
            auto eventTarget =
                eventTargetFromValue(runtime, arguments[4], arguments[0]);
            if (!eventTarget) {
              react_native_assert(false);
              return jsi::Value::undefined();
            }
            return valueFromShadowNode(
                runtime,
                uiManager->createNode(
                    tagFromValue(arguments[0]),
                    stringFromValue(runtime, arguments[1]),
                    surfaceIdFromValue(runtime, arguments[2]),
                    RawProps(runtime, arguments[3]),
                    eventTarget));
          } catch (const std::logic_error& ex) {
            LOG(FATAL) << "logic_error in createNode: " << ex.what();
          }
        });
  }
// RNOH patch begin
#ifdef PARALLELIZATION_ON
  if (methodName == "batchCreateNode") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [uiManager, this](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/
            ,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto nodeParamsArray =
              arguments[0].asObject(runtime).asArray(runtime);
          size_t arrayLength = nodeParamsArray.length(runtime);
          struct Param {
            Param(
                Tag tag,
                std::string&& name,
                SurfaceId surfaceId,
                std::shared_ptr<RawProps>&& rawProps,
                SharedEventTarget&& eventTarget)
                : tag(tag),
                  name(std::forward<std::string>(name)),
                  surfaceId(surfaceId),
                  rawProps(std::forward<std::shared_ptr<RawProps>>(rawProps)),
                  eventTarget(std::forward<SharedEventTarget>(eventTarget)) {}
            Tag tag;
            std::string name;
            SurfaceId surfaceId;
            std::shared_ptr<RawProps> rawProps;
            SharedEventTarget eventTarget;
          };

          std::vector<jsi::Object> nodes;
          nodes.reserve(arrayLength);
          std::vector<Param> params;
          params.reserve(arrayLength);

          auto ptag = jsi::String::createFromAscii(runtime, "tag");
          auto puiViewClassName =
              jsi::String::createFromAscii(runtime, "uiViewClassName");
          auto prenderLanes =
              jsi::String::createFromAscii(runtime, "renderLanes");
          auto pupdatePayload =
              jsi::String::createFromAscii(runtime, "updatePayload");
          auto pworkInProgress =
              jsi::String::createFromAscii(runtime, "workInProgress");
          auto pnode = jsi::String::createFromAscii(runtime, "node");
          auto pcobj = jsi::String::createFromAscii(runtime, "cobj");

          auto job_partner =
              ffrt::job_partner<rnoh::ScenarioID::SHADOW_TREE_PARALLELIZATION>::get_partner_of_this_thread(
                  ffrt::job_partner_attr()
                      .max_parallelism(rnoh::MAX_THREAD_NUM_SHADOW_TREE)
                      .qos(rnoh::THREAD_PRIORITY_LEVEL_5));

          constexpr uint64_t GROUP_NUM = 2; // [opt] switch for number of groups
          constexpr uint64_t GROUP_NUM2 = 100; // [opt] switch for number of subgroups
          constexpr uint64_t stack_size = 32 * 1024;
          static std::array<char, stack_size> stack[GROUP_NUM * GROUP_NUM2];
          uint64_t group = (arrayLength + GROUP_NUM - 1) / GROUP_NUM;
          {
            SystraceSection s(
                "UIManagerBinding::batchCreateNode:", arrayLength);
            size_t ijob = 0;
            for (size_t i = 0; i < arrayLength; ++i) {
              auto nodeParamObj = nodeParamsArray.getValueAtIndex(runtime, i)
                                      .getObject(runtime);
              nodes.emplace_back(nodeParamObj.getProperty(runtime, pnode)
                                     .getObject(runtime));
              auto tag =
                  tagFromValue(nodeParamObj.getProperty(runtime, ptag));
              params.emplace_back(
                  tag,
                  stringFromValue(
                      runtime,
                      nodeParamObj.getProperty(runtime, puiViewClassName)),
                  surfaceIdFromValue(
                      runtime,
                      nodeParamObj.getProperty(runtime, prenderLanes)),
                  std::make_shared<RawProps>(
                      runtime,
                      nodeParamObj.getProperty(runtime, pupdatePayload)),
                  eventTargetFromValue(
                      runtime,
                      nodeParamObj.getProperty(runtime, pworkInProgress),
                      tag));
              auto p = &params[i];
              if (!p->eventTarget) {
                react_native_assert(false);
                return jsi::Value::undefined();
              }
              // Handle small scale directly
              if (arrayLength < GROUP_NUM * GROUP_NUM2) {
                auto sn = uiManager->createNode(
                    p->tag,
                    p->name,
                    p->surfaceId,
                    *p->rawProps,
                    p->eventTarget);
                nodes[i].setProperty(
                    runtime, pcobj, valueFromShadowNode(runtime, sn));
                continue;
              }
              if ((i % group) == (group - 1) || i == (arrayLength - 1)) {
                job_partner->submit(
                    [&runtime,
                     &params,
                     &nodes,
                     &pcobj,
                     uiManager,
                     i,
                     group,
                     &job_partner,
                     stack_base = &stack[ijob * GROUP_NUM2]] {
                      auto istart = i / group * group;
                      auto num = i - istart + 1;
                      uint64_t group2 = (num + GROUP_NUM2 - 1) /
                          GROUP_NUM2; // Number of calculations per group
                      uint64_t group_num2 =
                          (num + group2 - 1) / group2; // Actual number of groups
                      // Assign the first group_num2 - 1 groups to others
                      for (uint64_t g2 = 0; g2 < group_num2 - 1; g2++) {
                        job_partner->submit(
                            [&runtime,
                             &params,
                             &nodes,
                             &pcobj,
                             uiManager,
                             istart,
                             g2,
                             group2,
                             &job_partner] {
                              auto is = istart + g2 * group2;
                              for (uint64_t j = is; j < is + group2; j++) {
                                auto p = &params[j];
                                auto sn = uiManager->createNode(
                                    p->tag,
                                    p->name,
                                    p->surfaceId,
                                    *p->rawProps,
                                    p->eventTarget);
                                auto& node = nodes[j];
                                if (job_partner->this_thread_is_master()) {
                                  node.setProperty(
                                      runtime,
                                      pcobj,
                                      valueFromShadowNode(runtime, sn));
                                } else
                                  job_partner->submit_to_master([&] {
                                    node.setProperty(
                                        runtime,
                                        pcobj,
                                        valueFromShadowNode(runtime, sn));
                                  });
                              }
                            },
                            &stack_base[g2 + 1],
                            stack_size);
                      }
                      // Process the last group in this thread（JS）
                      auto is = istart + (group_num2 - 1) * group2;
                      for (uint64_t j = is; j <= i; j++) {
                        auto p = &params[j];
                        auto sn = uiManager->createNode(
                            p->tag,
                            p->name,
                            p->surfaceId,
                            *p->rawProps,
                            p->eventTarget);
                        auto& node = nodes[j];
                        if (job_partner->this_thread_is_master()) {
                          node.setProperty(
                              runtime,
                              pcobj,
                              valueFromShadowNode(runtime, sn));
                        } else
                          job_partner->submit_to_master([&] {
                            node.setProperty(
                                runtime,
                                pcobj,
                                valueFromShadowNode(runtime, sn));
                          });
                      }
                    },
                    &stack[ijob * GROUP_NUM2],
                    stack_size);
                ijob++;
              }
            }
            job_partner->wait();
          }

          return jsi::Value::undefined();
        });
  }
#endif
// RNOH patch end
  // Semantic: Clones the node with *same* props and *same* children.
  if (methodName == "cloneNode") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          return valueFromShadowNode(
              runtime,
              uiManager->cloneNode(
                  *shadowNodeFromValue(runtime, arguments[0])));
        });
  }

  if (methodName == "setIsJSResponder") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          uiManager->setIsJSResponder(
              shadowNodeFromValue(runtime, arguments[0]),
              arguments[1].getBool(),
              arguments[2].getBool());

          return jsi::Value::undefined();
        });
  }

  if (methodName == "findNodeAtPoint") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto node = shadowNodeFromValue(runtime, arguments[0]);
          auto locationX = (Float)arguments[1].getNumber();
          auto locationY = (Float)arguments[2].getNumber();
          auto onSuccessFunction =
              arguments[3].getObject(runtime).getFunction(runtime);
          auto targetNode =
              uiManager->findNodeAtPoint(node, Point{locationX, locationY});
          auto& eventTarget = targetNode->getEventEmitter()->eventTarget_;

          EventEmitter::DispatchMutex().lock();
          eventTarget->retain(runtime);
          auto instanceHandle = eventTarget->getInstanceHandle(runtime);
          eventTarget->release(runtime);
          EventEmitter::DispatchMutex().unlock();

          onSuccessFunction.call(runtime, std::move(instanceHandle));
          return jsi::Value::undefined();
        });
  }

  // Semantic: Clones the node with *same* props and *empty* children.
  if (methodName == "cloneNodeWithNewChildren") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          return valueFromShadowNode(
              runtime,
              uiManager->cloneNode(
                  *shadowNodeFromValue(runtime, arguments[0]),
                  ShadowNode::emptySharedShadowNodeSharedList()));
        });
  }

  // Semantic: Clones the node with *given* props and *same* children.
  if (methodName == "cloneNodeWithNewProps") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto const& rawProps = RawProps(runtime, arguments[1]);
          return valueFromShadowNode(
              runtime,
              uiManager->cloneNode(
                  *shadowNodeFromValue(runtime, arguments[0]),
                  nullptr,
                  &rawProps));
        });
  }

  // Semantic: Clones the node with *given* props and *empty* children.
  if (methodName == "cloneNodeWithNewChildrenAndProps") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto const& rawProps = RawProps(runtime, arguments[1]);
          return valueFromShadowNode(
              runtime,
              uiManager->cloneNode(
                  *shadowNodeFromValue(runtime, arguments[0]),
                  ShadowNode::emptySharedShadowNodeSharedList(),
                  &rawProps));
        });
  }

  if (methodName == "appendChild") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          uiManager->appendChild(
              shadowNodeFromValue(runtime, arguments[0]),
              shadowNodeFromValue(runtime, arguments[1]));
          return jsi::Value::undefined();
        });
  }
// RNOH patch begin
#ifdef PARALLELIZATION_ON
  if (methodName == "batchAppendChild") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/
            ,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          SystraceSection s("UIManagerBinding::batchAppendChild");
          auto tagPairsArray = arguments[0].asObject(runtime).asArray(runtime);
          size_t arrayLength = tagPairsArray.length(runtime);
          auto pparentTag = jsi::String::createFromAscii(runtime, "parentTag");
          auto pcurrentTag =
              jsi::String::createFromAscii(runtime, "currentTag");
          for (size_t i = 0; i < arrayLength; ++i) {
            auto tagPairObj =
                tagPairsArray.getValueAtIndex(runtime, i).getObject(runtime);

            auto parent = shadowNodeFromValue(
                runtime, tagPairObj.getProperty(runtime, pparentTag));
            auto child = shadowNodeFromValue(
                runtime, tagPairObj.getProperty(runtime, pcurrentTag));
            uiManager->appendChild(parent, child);
          }

          return jsi::Value::undefined();
        });
  }
#endif
// RNOH patch end
  if (methodName == "createChildSet") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [](jsi::Runtime& runtime,
           jsi::Value const& /*thisValue*/,
           jsi::Value const* /*arguments*/,
           size_t /*count*/) noexcept -> jsi::Value {
          auto shadowNodeList = std::make_shared<ShadowNode::ListOfShared>(
              ShadowNode::ListOfShared({}));
          return valueFromShadowNodeList(runtime, shadowNodeList);
        });
  }

  if (methodName == "appendChildToSet") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [](jsi::Runtime& runtime,
           jsi::Value const& /*thisValue*/,
           jsi::Value const* arguments,
           size_t /*count*/) noexcept -> jsi::Value {
          auto shadowNodeList = shadowNodeListFromValue(runtime, arguments[0]);
          auto shadowNode = shadowNodeFromValue(runtime, arguments[1]);
          shadowNodeList->push_back(shadowNode);
          return jsi::Value::undefined();
        });
  }

  if (methodName == "completeRoot") {
    std::weak_ptr<UIManager> weakUIManager = uiManager_;
    // Enhanced version of the method that uses `backgroundExecutor` and
    // captures a shared pointer to `UIManager`.
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [weakUIManager, uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto runtimeSchedulerBinding =
              RuntimeSchedulerBinding::getBinding(runtime);
          auto surfaceId = surfaceIdFromValue(runtime, arguments[0]);

          if (!uiManager->backgroundExecutor_ ||
              (runtimeSchedulerBinding &&
               runtimeSchedulerBinding->getIsSynchronous())) {
            auto weakShadowNodeList =
                weakShadowNodeListFromValue(runtime, arguments[1]);
            auto shadowNodeList =
                shadowNodeListFromWeakList(weakShadowNodeList);
            if (shadowNodeList) {
              uiManager->completeSurface(
                  surfaceId,
                  shadowNodeList,
                  {/* .enableStateReconciliation = */ true,
                   /* .mountSynchronously = */ false});
            }
          } else {
            auto weakShadowNodeList =
                weakShadowNodeListFromValue(runtime, arguments[1]);
            static std::atomic_uint_fast8_t completeRootEventCounter{0};
            static std::atomic_uint_fast32_t mostRecentSurfaceId{0};
            completeRootEventCounter += 1;
            mostRecentSurfaceId = surfaceId;
            uiManager->backgroundExecutor_(
                [weakUIManager,
                 weakShadowNodeList,
                 surfaceId,
                 eventCount = completeRootEventCounter.load()] {
                  auto shouldYield = [=]() -> bool {
                    // If `completeRootEventCounter` was incremented, another
                    // `completeSurface` call has been scheduled and current
                    // `completeSurface` should yield to it.
                    return completeRootEventCounter > eventCount &&
                        mostRecentSurfaceId == surfaceId;
                  };
                  auto shadowNodeList =
                      shadowNodeListFromWeakList(weakShadowNodeList);
                  auto strongUIManager = weakUIManager.lock();
                  if (shadowNodeList && strongUIManager) {
                    strongUIManager->completeSurface(
                        surfaceId,
                        shadowNodeList,
                        {/* .enableStateReconciliation = */ true,
                         /* .mountSynchronously = */ false,
                         /* .shouldYield = */ shouldYield});
                  }
                });
          }

          return jsi::Value::undefined();
        });
  }

  if (methodName == "registerEventHandler") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [this](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto eventHandler =
              arguments[0].getObject(runtime).getFunction(runtime);
          eventHandler_ =
              std::make_unique<EventHandlerWrapper>(std::move(eventHandler));
          return jsi::Value::undefined();
        });
  }

  if (methodName == "getRelativeLayoutMetrics") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto layoutMetrics = uiManager->getRelativeLayoutMetrics(
              *shadowNodeFromValue(runtime, arguments[0]),
              shadowNodeFromValue(runtime, arguments[1]).get(),
              {/* .includeTransform = */ true});
          auto frame = layoutMetrics.frame;
          auto result = jsi::Object(runtime);
          result.setProperty(runtime, "left", frame.origin.x);
          result.setProperty(runtime, "top", frame.origin.y);
          result.setProperty(runtime, "width", frame.size.width);
          result.setProperty(runtime, "height", frame.size.height);
          return result;
        });
  }

  if (methodName == "dispatchCommand") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        3,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto shadowNode = shadowNodeFromValue(runtime, arguments[0]);
          if (shadowNode) {
            uiManager->dispatchCommand(
                shadowNode,
                stringFromValue(runtime, arguments[1]),
                commandArgsFromValue(runtime, arguments[2]));
          }
          return jsi::Value::undefined();
        });
  }

  if (methodName == "setNativeProps") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            const jsi::Value&,
            const jsi::Value* arguments,
            size_t) -> jsi::Value {
          uiManager->setNativeProps_DEPRECATED(
              shadowNodeFromValue(runtime, arguments[0]),
              RawProps(runtime, arguments[1]));

          return jsi::Value::undefined();
        });
  }

  // Legacy API
  if (methodName == "measureLayout") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        4,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto layoutMetrics = uiManager->getRelativeLayoutMetrics(
              *shadowNodeFromValue(runtime, arguments[0]),
              shadowNodeFromValue(runtime, arguments[1]).get(),
              {/* .includeTransform = */ false});

          if (layoutMetrics == EmptyLayoutMetrics) {
            auto onFailFunction =
                arguments[2].getObject(runtime).getFunction(runtime);
            onFailFunction.call(runtime);
            return jsi::Value::undefined();
          }

          auto onSuccessFunction =
              arguments[3].getObject(runtime).getFunction(runtime);
          auto frame = layoutMetrics.frame;

          onSuccessFunction.call(
              runtime,
              {jsi::Value{runtime, (double)frame.origin.x},
               jsi::Value{runtime, (double)frame.origin.y},
               jsi::Value{runtime, (double)frame.size.width},
               jsi::Value{runtime, (double)frame.size.height}});
          return jsi::Value::undefined();
        });
  }

  if (methodName == "measure") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto shadowNode = shadowNodeFromValue(runtime, arguments[0]);
          auto layoutMetrics = uiManager->getRelativeLayoutMetrics(
              *shadowNode, nullptr, {/* .includeTransform = */ true});
          auto onSuccessFunction =
              arguments[1].getObject(runtime).getFunction(runtime);

          if (layoutMetrics == EmptyLayoutMetrics) {
            onSuccessFunction.call(runtime, {0, 0, 0, 0, 0, 0});
            return jsi::Value::undefined();
          }
          auto newestCloneOfShadowNode =
              uiManager->getNewestCloneOfShadowNode(*shadowNode);

          auto layoutableShadowNode = traitCast<LayoutableShadowNode const*>(
              newestCloneOfShadowNode.get());
          Point originRelativeToParent = layoutableShadowNode != nullptr
              ? layoutableShadowNode->getLayoutMetrics().frame.origin
              : Point();

          auto frame = layoutMetrics.frame;
          onSuccessFunction.call(
              runtime,
              {jsi::Value{runtime, (double)originRelativeToParent.x},
               jsi::Value{runtime, (double)originRelativeToParent.y},
               jsi::Value{runtime, (double)frame.size.width},
               jsi::Value{runtime, (double)frame.size.height},
               jsi::Value{runtime, (double)frame.origin.x},
               jsi::Value{runtime, (double)frame.origin.y}});
          return jsi::Value::undefined();
        });
  }

  if (methodName == "measureInWindow") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto layoutMetrics = uiManager->getRelativeLayoutMetrics(
              *shadowNodeFromValue(runtime, arguments[0]),
              nullptr,
              {/* .includeTransform = */ true,
               /* .includeViewportOffset = */ true});

          auto onSuccessFunction =
              arguments[1].getObject(runtime).getFunction(runtime);

          if (layoutMetrics == EmptyLayoutMetrics) {
            onSuccessFunction.call(runtime, {0, 0, 0, 0});
            return jsi::Value::undefined();
          }

          auto frame = layoutMetrics.frame;
          onSuccessFunction.call(
              runtime,
              {jsi::Value{runtime, (double)frame.origin.x},
               jsi::Value{runtime, (double)frame.origin.y},
               jsi::Value{runtime, (double)frame.size.width},
               jsi::Value{runtime, (double)frame.size.height}});
          return jsi::Value::undefined();
        });
  }

  if (methodName == "getBoundingClientRect") {
    // This is similar to `measureInWindow`, except it's explicitly synchronous
    // (returns the result instead of passing it to a callback).
    // The behavior is similar to `Element.prototype.getBoundingClientRect` from
    // Web.
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          auto layoutMetrics = uiManager->getRelativeLayoutMetrics(
              *shadowNodeFromValue(runtime, arguments[0]),
              nullptr,
              {/* .includeTransform = */ true,
               /* .includeViewportOffset = */ true});

          if (layoutMetrics == EmptyLayoutMetrics) {
            return jsi::Value::undefined();
          }

          auto frame = layoutMetrics.frame;
          return jsi::Array::createWithElements(
              runtime,
              jsi::Value{runtime, (double)frame.origin.x},
              jsi::Value{runtime, (double)frame.origin.y},
              jsi::Value{runtime, (double)frame.size.width},
              jsi::Value{runtime, (double)frame.size.height});
        });
  }

  if (methodName == "sendAccessibilityEvent") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        2,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          uiManager->sendAccessibilityEvent(
              shadowNodeFromValue(runtime, arguments[0]),
              stringFromValue(runtime, arguments[1]));

          return jsi::Value::undefined();
        });
  }

  if (methodName == "configureNextLayoutAnimation") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        3,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const& /*thisValue*/,
            jsi::Value const* arguments,
            size_t /*count*/) noexcept -> jsi::Value {
          uiManager->configureNextLayoutAnimation(
              runtime,
              // TODO: pass in JSI value instead of folly::dynamic to RawValue
              RawValue(commandArgsFromValue(runtime, arguments[0])),
              arguments[1],
              arguments[2]);
          return jsi::Value::undefined();
        });
  }

  if (methodName == "unstable_getCurrentEventPriority") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        0,
        [this](
            jsi::Runtime&,
            jsi::Value const&,
            jsi::Value const*,
            size_t) noexcept -> jsi::Value {
          return {serialize(currentEventPriority_)};
        });
  }

  if (methodName == "unstable_DefaultEventPriority") {
    return {serialize(ReactEventPriority::Default)};
  }

  if (methodName == "unstable_DiscreteEventPriority") {
    return {serialize(ReactEventPriority::Discrete)};
  }

  if (methodName == "findShadowNodeByTag_DEPRECATED") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [uiManager](
            jsi::Runtime& runtime,
            jsi::Value const&,
            jsi::Value const* arguments,
            size_t) -> jsi::Value {
          auto shadowNode = uiManager->findShadowNodeByTag_DEPRECATED(
              tagFromValue(arguments[0]));

          if (!shadowNode) {
            return jsi::Value::null();
          }

          return valueFromShadowNode(runtime, shadowNode);
        });
  }

  return jsi::Value::undefined();
}

} // namespace facebook::react
