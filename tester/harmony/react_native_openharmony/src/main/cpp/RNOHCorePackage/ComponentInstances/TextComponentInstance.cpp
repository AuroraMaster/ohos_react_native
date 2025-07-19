/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "TextComponentInstance.h"
#include <glog/logging.h>
#include <react/renderer/components/text/ParagraphProps.h>
#include <react/renderer/components/text/ParagraphState.h>
#include <react/renderer/core/ConcreteState.h>
#include <sstream>
#include "RNOH/RNInstanceCAPI.h"
#include "RNOH/TextMeasurer.h"
#include "TextConversions.h"
#include "react/renderer/attributedstring/primitives.h"
#include "react/renderer/components/view/conversions.h"

namespace rnoh {
const static float DEFAULT_LINE_SPACING = 0.15f;

TextComponentInstance::TextComponentInstance(Context context)
    : CppComponentInstance(std::move(context)),
      m_textNode(context.arkUINodeContext) {
}

TextComponentInstance::~TextComponentInstance() {
  if (m_stackNodePtr != nullptr) {
    m_stackNodePtr->removeChild(m_textNode);
    delete m_stackNodePtr;
    m_stackNodePtr = nullptr;
  }
  for (auto const& item : m_childNodes) {
    m_textNode.removeChild(*item);
  }
  if (m_key != "") {
    m_textNode.resetTextContentWithStyledString();
    TextMeasureRegistry::getTextMeasureRegistry().eraseTextMeasureInfo(m_key);
  }
  if (m_state != nullptr) {
    m_state->getData().paragraphLayoutManager.resetCache();
  }
}

void TextComponentInstance::onChildInserted(
    ComponentInstance::Shared const& childComponentInstance,
    std::size_t index) {
    if (m_stackNodePtr != nullptr) {
      m_stackNodePtr->insertChild(
        childComponentInstance->getLocalRootArkUINode(), index + 1);
    }
}

void TextComponentInstance::onChildRemoved(
    ComponentInstance::Shared const& childComponentInstance) {
  if (m_stackNodePtr != nullptr) {
    m_stackNodePtr->removeChild(childComponentInstance->getLocalRootArkUINode());
  }
}

const std::string& TextComponentInstance::getAccessibilityLabel() const {
  auto const& superAccessibilityLabel =
      CppComponentInstance::getAccessibilityLabel();
  return superAccessibilityLabel.empty() ? m_textContent
                                         : superAccessibilityLabel;
}

void TextComponentInstance::onPropsChanged(
    SharedConcreteProps const& textProps) {
  CppComponentInstance::onPropsChanged(textProps);
  if (textProps == nullptr) {
    return;
  }
  // padding
  TextPaddingInfo info = TextConversions::getArkUIPadding(textProps);
  VLOG(3) << "[text-debug] textProps->setPadding top=" << info.top
          << ", right=" << info.right << ", bottom=" << info.bottom
          << ", left=" << info.left;
  m_textNode.setPadding(info.top, info.right, info.bottom, info.left);

  // Copy Option
  VLOG(3) << "[text-debug] textProps->isSelectable=" << textProps->isSelectable;
  int32_t testCopyOption = ArkUI_CopyOptions::ARKUI_COPY_OPTIONS_NONE;
  if (textProps->isSelectable) {
    testCopyOption = ArkUI_CopyOptions::ARKUI_COPY_OPTIONS_LOCAL_DEVICE;
  }
  m_textNode.setTextCopyOption(testCopyOption);
    
    
  if (textProps->rawProps != nullptr) {
    // stack align
    facebook::react::TextAlignment alignHorizon =
        facebook::react::TextAlignment::Left;
    if (textProps->rawProps.count("writingDirection") != 0 && textProps->rawProps["writingDirection"] == "rtl") {
      alignHorizon =
          facebook::react::TextAlignment::Right;
    }
    std::string alignVertical = "top";
    if (textProps->rawProps.count("textAlignVertical") != 0 && textProps->rawProps["textAlignVertical"].isString()) {
      alignVertical = textProps->rawProps["textAlignVertical"].asString();
    }
    if (textProps->textAttributes.alignment.has_value()) {
      alignHorizon = textProps->textAttributes.alignment.value();
    }
    int32_t stackAlign =
        TextConversions::getStackAlign(alignHorizon, alignVertical);

    facebook::react::TextAlignment alignHorizonOld =
        facebook::react::TextAlignment::Left;
    std::string alignVerticalOld = "top";
    if (m_props && m_props->rawProps.count("textAlignVertical") != 0 && m_props->rawProps["textAlignVertical"].isString()) {
      alignVerticalOld = m_props->rawProps["textAlignVertical"].asString();
    }
    if (m_props && m_props->textAttributes.alignment.has_value()) {
      alignHorizonOld = m_props->textAttributes.alignment.value();
    }
    int32_t stackAlignOld =
        TextConversions::getStackAlign(alignHorizonOld, alignVerticalOld);

    if (!m_props || stackAlign != stackAlignOld) {
      VLOG(3) << "[text-debug] stackAlign=" << stackAlign
              << ", stackAlignOld=" << stackAlignOld;
      if (m_stackNodePtr) {
        m_stackNodePtr->setAlign(stackAlign);
      } else {
        m_textNode.setAlignment((ArkUI_Alignment)stackAlign);
      }
    }

    // enable
    if (textProps->rawProps.count("disabled") != 0) {
      bool enableFlag = textProps->rawProps["disabled"].asBool();
      VLOG(3) << "[text-debug] rawProps->disabled=" << enableFlag;
      m_textNode.setTextEnable(!enableFlag);
    }
    
    // selectionColor
    if (textProps->rawProps.count("selectionColor") != 0 && !textProps->rawProps["selectionColor"].isNull()) {
      uint32_t selectionColor = textProps->rawProps["selectionColor"].asInt();
      selectionColor = (selectionColor & 0x00ffffff) | 0x33000000;
      VLOG(3) << "[text-debug] selectionColor: " << selectionColor;
      m_textNode.setSelectedBackgroundColor(selectionColor);
    }
    
    // dataDetectorType
    if (textProps->rawProps.count("dataDetectorType") != 0) {
      std::string dataDetectorType = textProps->rawProps["dataDetectorType"].asString();
      VLOG(3) << "[text-debug] dataDetectorType: " << dataDetectorType;
      if (dataDetectorType == "all") {
        ArkUI_NumberValue types[] = {};
        m_textNode.setTextDataDetectorType(true, types, sizeof(types) / sizeof(ArkUI_NumberValue));
      } else if (dataDetectorType == "address") {
        ArkUI_NumberValue types[] = {
            {.i32 = ARKUI_TEXT_DATA_DETECTOR_TYPE_ADDRESS}};
        m_textNode.setTextDataDetectorType(true, types, sizeof(types) / sizeof(ArkUI_NumberValue));
      } else if (dataDetectorType == "link") {
        ArkUI_NumberValue types[] = {
            {.i32 = ARKUI_TEXT_DATA_DETECTOR_TYPE_URL}};
        m_textNode.setTextDataDetectorType(true, types, sizeof(types) / sizeof(ArkUI_NumberValue));
      } else if (dataDetectorType == "phoneNumber") {
        ArkUI_NumberValue types[] = {
            {.i32 = ARKUI_TEXT_DATA_DETECTOR_TYPE_PHONE_NUMBER}};
        m_textNode.setTextDataDetectorType(true, types, sizeof(types) / sizeof(ArkUI_NumberValue));
      } else if (dataDetectorType == "email") {
        ArkUI_NumberValue types[] = {
            {.i32 = ARKUI_TEXT_DATA_DETECTOR_TYPE_EMAIL}};
        m_textNode.setTextDataDetectorType(true, types, sizeof(types) / sizeof(ArkUI_NumberValue));
      } else {
        ArkUI_NumberValue types[] = {};
        m_textNode.setTextDataDetectorType(false, types, sizeof(types) / sizeof(ArkUI_NumberValue));
      }
    }
    
    // writingDirection
    if (textProps->rawProps.count("writingDirection") != 0) {
      int32_t writingDirection = TextConversions::getArkUIDirection(textProps->rawProps["writingDirection"].asString());
      VLOG(3) << "[text-debug] writingDirection=" << writingDirection;
      m_textNode.setWritingDirection(writingDirection);
    }
  }
  VLOG(3) << "[text-debug] setProps end";
}

void TextComponentInstance::onFinalizeUpdates() {
  auto parent = m_parent.lock();
  if (m_props->importantForAccessibility ==
          facebook::react::ImportantForAccessibility::Auto &&
      parent && !parent->getAccessibilityGroup()) {
    m_textNode.setAccessibilityMode(
        ArkUI_AccessibilityMode::ARKUI_ACCESSIBILITY_MODE_ENABLED);
  }
}

void TextComponentInstance::onStateChanged(
    SharedConcreteState const& textState) {
  CppComponentInstance::onStateChanged(textState);
  m_touchTargetChildrenNeedUpdate = true;
  for (const auto& item : m_childNodes) {
    m_textNode.removeChild(*item);
  }
  m_childNodes.clear();
  uint32_t childIndex = 0;
  auto const& fragments = textState->getData().attributedString.getFragments();
  VLOG(3) << "[text-debug] getFragments size:" << fragments.size();
  m_textNode.resetTextContentWithStyledString();
  if (fragments.empty()) {
    if (m_key != "") {
      TextMeasureRegistry::getTextMeasureRegistry().eraseTextMeasureInfo(m_key);
    }
    m_key = "";
    return;
  }
  m_key = std::to_string(m_rnInstanceId) + "_" + 
    std::to_string(fragments[0].parentShadowView.tag) + "_" + 
    std::to_string(fragments[0].parentShadowView.surfaceId);
  auto info = TextMeasureRegistry::getTextMeasureRegistry().getTextMeasureInfoByKey(m_key);
  if (info.has_value()) {
    VLOG(3) << "[text-debug] setTextContentWithStyledString";
    m_textNode.setTextContentWithStyledString(info.value());
    std::string textContent;
    for (auto& fragment: fragments) {
      textContent += fragment.string;
    }
    m_textContent = textContent;    
    m_textNode.setTextContent(textContent);
  }
  this->setTextAttributes(fragments[0].textAttributes);
}

void TextComponentInstance::setTextAttributes(
    const facebook::react::TextAttributes& textAttributes) {
  // TextAlign
  if (textAttributes.alignment.has_value()) {
    int32_t align = TextConversions::getArkUITextAlignment(
        textAttributes.alignment.value());
    VLOG(3) << "[text-debug] textAttributes.alignment=" << align;
    m_textNode.setTextAlign(align);
  }
}

ArkUINode& TextComponentInstance::getLocalRootArkUINode() {
    if (m_stackNodePtr != nullptr) {
      return *m_stackNodePtr;
    } else {
      return m_textNode;
    }
}

class TextFragmentTouchTarget : public TouchTarget {
 public:
  TextFragmentTouchTarget(
      facebook::react::Tag tag,
      TouchTarget::Weak parentTouchTarget,
      facebook::react::SharedTouchEventEmitter touchEventEmitter)
      : m_tag(tag),
        m_touchEventEmitter(std::move(touchEventEmitter)),
        m_parentTouchTarget(parentTouchTarget) {}

  void clearRects() {
    m_rects.clear();
  }

  void insertRects(std::vector<facebook::react::Rect> const& rects) {
    m_rects.insert(m_rects.end(), rects.begin(), rects.end());
  }

  facebook::react::SharedTouchEventEmitter getTouchEventEmitter()
      const override {
    return m_touchEventEmitter;
  }

  facebook::react::Point computeChildPoint(
      facebook::react::Point const& point,
      TouchTarget::Shared const& /*child*/) const override {
    return point;
  }

  bool containsPoint(facebook::react::Point const& point) const override {
    for (auto rect : m_rects) {
      if (rect.containsPoint(point)) {
        return true;
      }
    }
    return false;
  }

  bool containsPointInBoundingBox(
      facebook::react::Point const& point) override {
    return containsPoint(point);
  }

  bool canHandleTouch() const override {
    return m_touchEventEmitter != nullptr;
  }

  bool canChildrenHandleTouch() const override {
    return false;
  }

  bool canSubtreeHandleTouch(facebook::react::Point const& point) override {
    return false;
  }

  facebook::react::Tag getTouchTargetTag() const override {
    return m_tag;
  }

  TouchTarget::Shared getTouchTargetParent() const override {
    return m_parentTouchTarget.lock();
  };

  std::vector<TouchTarget::Shared> getTouchTargetChildren() override {
    return {};
  }

  facebook::react::LayoutMetrics getLayoutMetrics() const override {
    return {};
  }

  facebook::react::Transform getTransform() const override {
    return {};
  }

  facebook::react::Rect getBoundingBox() override {
    return {};
  };

  bool isClippingSubviews() const override {
    return false;
  }

  void markBoundingBoxAsDirty() override{};

 private:
  facebook::react::Tag m_tag;
  facebook::react::SharedTouchEventEmitter m_touchEventEmitter;
  std::vector<facebook::react::Rect> m_rects{};
  TouchTarget::Weak m_parentTouchTarget;
};

std::vector<TouchTarget::Shared>
TextComponentInstance::getTouchTargetChildren() {
  if (m_state == nullptr) {
    m_fragmentTouchTargetByTag.clear();
    return {};
  }

  if (m_touchTargetChildrenNeedUpdate) {
    m_touchTargetChildrenNeedUpdate = false;
    updateFragmentTouchTargets(m_state->getData());
  }

  auto const& fragments = m_state->getData().attributedString.getFragments();

  auto attachmentCount = 0;
  std::vector<TouchTarget::Shared> result;
  for (auto const& fragment : fragments) {
    if (!fragment.isAttachment()) {
      result.push_back(
          m_fragmentTouchTargetByTag.at(fragment.parentShadowView.tag));
    }
  }
  result.insert(result.end(), m_children.begin(), m_children.end());
  return result;
}

void TextComponentInstance::updateFragmentTouchTargets(
    facebook::react::ParagraphState const& newState) {
  auto const& fragments = newState.attributedString.getFragments();
  auto textLayoutManager =
      newState.paragraphLayoutManager.getTextLayoutManager();
  if (textLayoutManager == nullptr || fragments.empty()) {
    m_fragmentTouchTargetByTag.clear();
    return;
  }

  auto nativeTextLayoutManager =
      textLayoutManager->getNativeTextLayoutManager();
  auto textMeasurer = static_cast<TextMeasurer*>(nativeTextLayoutManager);
  auto left = m_layoutMetrics.contentInsets.left;
  auto right = m_layoutMetrics.contentInsets.right;
  auto top = m_layoutMetrics.contentInsets.top;
  auto width = m_layoutMetrics.frame.size.width;
  auto height = m_layoutMetrics.frame.size.height;
  auto maxWidth = width - left - right;
  Size size = {maxWidth, height};
  auto attributedString = newState.attributedString;
  auto paragraphAttributes = m_props->paragraphAttributes;
  textMeasurer->dealTextCase(attributedString, paragraphAttributes);
  auto typography = textMeasurer->measureTypography(
      attributedString,
      paragraphAttributes,
      {size, size}).build();
  auto rects = typography.getRectsForFragments({left, top});

  FragmentTouchTargetByTag touchTargetByTag;
  size_t textFragmentCount = 0;
  for (auto const& fragment : fragments) {
    if (fragment.isAttachment()) {
      continue;
    }
    auto index = textFragmentCount;
    auto eventEmitter =
        std::dynamic_pointer_cast<const facebook::react::TouchEventEmitter>(
            fragment.parentShadowView.eventEmitter);
    auto tag = fragment.parentShadowView.tag;
    auto touchTargetEntry = touchTargetByTag.find(tag);

    if (touchTargetEntry == touchTargetByTag.end()) {
      // if no entry has been inserted yet into the new map
      if (auto prevTouchTarget = m_fragmentTouchTargetByTag.find(tag);
          prevTouchTarget != m_fragmentTouchTargetByTag.end()) {
        // either reuse the existing touch target if there was a fragment with
        // the same tag
      auto fragmentTouchTarget =
          std::static_pointer_cast<TextFragmentTouchTarget>(
                prevTouchTarget->second);
        fragmentTouchTarget->clearRects();
        touchTargetEntry =
            touchTargetByTag.try_emplace(tag, std::move(fragmentTouchTarget))
                .first;
    } else {
        // or create a new one for a new fragment tag
        auto newTouchTarget = std::make_shared<TextFragmentTouchTarget>(
            tag, this->shared_from_this(), eventEmitter);
        touchTargetEntry =
            touchTargetByTag.try_emplace(tag, std::move(newTouchTarget)).first;
    }
    }

    auto fragmentTouchTarget =
        std::static_pointer_cast<TextFragmentTouchTarget>(
            touchTargetEntry->second);
    fragmentTouchTarget->insertRects(rects.at(index));
    textFragmentCount++;
  }
  m_fragmentTouchTargetByTag = std::move(touchTargetByTag);
}

bool TextComponentInstance::checkUpdateBaseNode() {
    return m_stackNodePtr == nullptr;
}

void TextComponentInstance::setShadowView(facebook::react::ShadowView const& shadowView) {
  CppComponentInstance::setShadowView(shadowView);
  m_shadowView = shadowView;
  if (m_hasCheckNesting) {
    return;
  }
  m_hasCheckNesting = true;
  auto textState = std::dynamic_pointer_cast<const ConcreteState>(shadowView.state);
  if (!textState) {
    return;
  }
  // Wrapped by a StackNode if WritingDirection is rtl or there's a Attachment Fragment
  bool isNesting = textState->getData().paragraphAttributes.writingDirection ==
      facebook::react::WritingDirection::RightToLeft;
  if (!isNesting) {
    auto const& fragments = textState->getData().attributedString.getFragments();
    for (const auto& fragment : fragments) {
      if (fragment.isAttachment()) {
        isNesting = true;
        break;
      }
    }
  }
  if (isNesting) {
    DLOG(INFO) << "new StackNode, tag=" << getTag();
    m_stackNodePtr = new StackNode();
    if (m_stackNodePtr == nullptr) {
      DLOG(INFO) << "new StackNode error";
      return;
    }
    m_stackNodePtr->insertChild(m_textNode, 0);
  }
}

} // namespace rnoh