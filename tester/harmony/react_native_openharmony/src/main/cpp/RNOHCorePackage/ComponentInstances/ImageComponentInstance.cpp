/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ImageComponentInstance.h"
#include <react/renderer/components/image/ImageProps.h>
#include <react/renderer/components/image/ImageState.h>
#include <react/renderer/core/ConcreteState.h>
#include <sstream>

namespace rnoh {

const std::string RAWFILE_PREFIX = "resource://RAWFILE/assets/";
const std::string INVALID_PATH_PREFIX = "invalidpathprefix/";

ImageComponentInstance::ImageComponentInstance(Context context)
    : CppComponentInstance(std::move(context)),
      m_imageNode(context.arkUINodeContext) {
  this->getLocalRootArkUINode().setNodeDelegate(this);
  this->getLocalRootArkUINode().setInterpolation(
      ARKUI_IMAGE_INTERPOLATION_HIGH);
  this->getLocalRootArkUINode().setDraggable(false);
  this->getLocalRootArkUINode().setAccessibilityMode(
    facebook::react::ImportantForAccessibility::Auto);
}

std::string ImageComponentInstance::FindLocalCacheByUri(std::string const& uri) {
  if(uri.find("http", 0) != 0) {
    return uri;
  }

  if (!m_deps) {
    return uri;
  }

  auto rnInstance = m_deps->rnInstance.lock();
  if (!rnInstance) {
    return uri;
  }

  auto turboModule = rnInstance->getTurboModule("ImageLoader");
  if (!turboModule) {
    return uri;
  }

  auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
  if (!arkTsTurboModule) {
    return uri;
  }

  auto cache = arkTsTurboModule->callSync("getPrefetchResult", {uri});
  if (!cache.isString()) {
    return uri;
  }

  return cache.asString();
}

std::string ImageComponentInstance::getBundlePath() {
  if (!m_deps) {
    return INVALID_PATH_PREFIX;
  }

  auto rnInstance = m_deps->rnInstance.lock();
  if (!rnInstance) {
    return INVALID_PATH_PREFIX;
  }

  auto internalInstance = std::dynamic_pointer_cast<RNInstanceInternal>(rnInstance);
  if (!internalInstance) {
    return INVALID_PATH_PREFIX;
  }

  std::string bundlePath = internalInstance->getBundlePath();
  if (bundlePath.empty()) {
    return INVALID_PATH_PREFIX;
  }

  return bundlePath;
}

std::string ImageComponentInstance::getAbsolutePathPrefix(std::string const& bundlePath) {
  if (bundlePath == INVALID_PATH_PREFIX) {
    return INVALID_PATH_PREFIX;
  }

  if (bundlePath.find('/', 0) != 0) {
    return RAWFILE_PREFIX;
  }

  auto pos = bundlePath.rfind('/');
  if (pos == std::string::npos) {
    return RAWFILE_PREFIX;
  }

  std::string prefix = "file://" + bundlePath.substr(0, pos + 1);
  return prefix;
}

void ImageComponentInstance::onPropsChanged(SharedConcreteProps const& props) {
  CppComponentInstance::onPropsChanged(props);

  auto rawProps = ImageRawProps::getFromDynamic(props->rawProps);

  if (!m_props || m_props->sources != props->sources) {
    std::string uri = FindLocalCacheByUri(props->sources[0].uri);
    this->getLocalRootArkUINode().setSources(uri, getAbsolutePathPrefix(getBundlePath()));
    if (!this->getLocalRootArkUINode().getUri().empty()) {
      onLoadStart();
    }
  }

  if (!m_props || m_props->tintColor != props->tintColor) {
    this->getLocalRootArkUINode().setTintColor(props->tintColor);
  }

  if (!m_props || m_props->blurRadius != props->blurRadius) {
    this->getLocalRootArkUINode().setBlur(props->blurRadius);
  }

  if (!m_props || m_props->resizeMode != props->resizeMode) {
    this->getLocalRootArkUINode().setObjectRepeat(props->resizeMode);
    this->getLocalRootArkUINode().setResizeMode(props->resizeMode);
  }

  if (!m_props || m_props->capInsets != props->capInsets) {
    this->getLocalRootArkUINode().setCapInsets(props->capInsets, m_layoutMetrics.pointScaleFactor);
  }

  if (!m_props || m_props->defaultSources != props->defaultSources) {
    if (!(props->defaultSources.empty())) {
      this->getLocalRootArkUINode().setAlt(props->defaultSources[0].uri, getAbsolutePathPrefix(getBundlePath()));
    }
  }

  if (m_rawProps.fadeDuration != rawProps.fadeDuration) {
    m_rawProps.fadeDuration = rawProps.fadeDuration;
    if (m_rawProps.fadeDuration.has_value()) {
      this->getLocalRootArkUINode().setFadeDuration(
          m_rawProps.fadeDuration.value());
    }
  }

  if (m_rawProps.loadingIndicatorSource != rawProps.loadingIndicatorSource) {
    m_rawProps.loadingIndicatorSource = rawProps.loadingIndicatorSource;
    if (m_rawProps.loadingIndicatorSource.has_value()) {
      this->getLocalRootArkUINode().setAlt(
          m_rawProps.loadingIndicatorSource.value(), getAbsolutePathPrefix(getBundlePath()));
    }
  }

  if (m_rawProps.resizeMethod != rawProps.resizeMethod) {
    m_rawProps.resizeMethod = rawProps.resizeMethod;
    if (m_rawProps.resizeMethod.has_value()) {
      this->getLocalRootArkUINode().setResizeMethod(
          m_rawProps.resizeMethod.value());
    } else {
      this->getLocalRootArkUINode().resetResizeMethod();
    }
  }

  if (m_rawProps.focusable != rawProps.focusable) {
    m_rawProps.focusable = rawProps.focusable;
    if (m_rawProps.focusable.has_value()) {
      this->getLocalRootArkUINode().setFocusable(m_rawProps.focusable.value());
    } else {
      this->getLocalRootArkUINode().resetFocusable();
    }
  }

  if (m_rawProps.alt != rawProps.alt) {
    m_rawProps.alt = rawProps.alt;
    if (m_rawProps.alt.has_value()) {
      this->getLocalRootArkUINode().setAccessibilityText(
          m_rawProps.alt.value());
    } else {
      this->getLocalRootArkUINode().resetAccessibilityText();
    }
  }
}

void ImageComponentInstance::onStateChanged(SharedConcreteState const& state) {
  CppComponentInstance::onStateChanged(state);
  auto source = state->getData().getImageSource();
  this->getLocalRootArkUINode().setSources(FindLocalCacheByUri(source.uri), getAbsolutePathPrefix(getBundlePath()));
  this->getLocalRootArkUINode().setBlur(state->getData().getBlurRadius());
}

ImageNode& ImageComponentInstance::getLocalRootArkUINode() {
  return m_imageNode;
}

void ImageComponentInstance::onComplete(float width, float height) {
  if (m_eventEmitter == nullptr) {
    return;
  }

  std::string uri = this->getLocalRootArkUINode().getUri();
  m_eventEmitter->dispatchEvent("load", [=](facebook::jsi::Runtime& runtime) {
    auto payload = facebook::jsi::Object(runtime);
    auto source = facebook::jsi::Object(runtime);
    source.setProperty(runtime, "width", width);
    source.setProperty(runtime, "height", height);
    source.setProperty(runtime, "uri", uri);
    payload.setProperty(runtime, "source", source);
    return payload;
  });
  m_eventEmitter->onLoadEnd();
}

void ImageComponentInstance::onError(int32_t errorCode) {
  if (m_eventEmitter == nullptr) {
    return;
  }

  std::string errMsg = "error message: the image load failed, ";
  if (errorCode == 401) {
    errMsg =
        "error message: the image could not be obtained because the image path is invalid, ";
  } else if (errorCode == 103101) {
    errMsg = "error message: the image format is not supported, ";
  }

  errMsg += std::string("error code: ") + std::to_string(errorCode);
  m_eventEmitter->dispatchEvent(
      "error", [errMsg](facebook::jsi::Runtime& runtime) {
        auto payload = facebook::jsi::Object(runtime);
        payload.setProperty(runtime, "error", errMsg);
        return payload;
      });
  m_eventEmitter->onLoadEnd();
}

void ImageComponentInstance::onProgress(uint32_t loaded, uint32_t total) {
  if (m_eventEmitter == nullptr) {
    return;
  }

  m_eventEmitter->dispatchEvent(
    "progress", [=](facebook::jsi::Runtime& runtime) {
      auto payload = facebook::jsi::Object(runtime);
      payload.setProperty(runtime, "loaded", (int32_t)loaded);
      payload.setProperty(runtime, "total", (int32_t)total);
      return payload;
    });
}

void ImageComponentInstance::onLoadStart() {
  if (m_eventEmitter) {
    m_eventEmitter->onLoadStart();
  }
}

ImageComponentInstance::ImageRawProps
ImageComponentInstance::ImageRawProps::getFromDynamic(folly::dynamic value) {
  auto resizeMethod = (value.count("resizeMethod") > 0)
      ? std::optional(value["resizeMethod"].asString())
      : std::nullopt;
  auto focusable = (value.count("focusable") > 0)
      ? std::optional(value["focusable"].asBool())
      : std::nullopt;
  auto alt = (value.count("alt") > 0) ? std::optional(value["alt"].asString())
                                      : std::nullopt;
                        
  auto loadingIndicatorSource = (value.count("loadingIndicatorSource") > 0)
      ? std::optional(value["loadingIndicatorSource"].at("uri").getString())
      : std::nullopt;
  auto fadeDuration = (value.count("fadeDuration") > 0)
      ? std::optional(value["fadeDuration"].asInt())
      : std::nullopt;

  return {resizeMethod, focusable, alt, loadingIndicatorSource, fadeDuration};
}

} // namespace rnoh
