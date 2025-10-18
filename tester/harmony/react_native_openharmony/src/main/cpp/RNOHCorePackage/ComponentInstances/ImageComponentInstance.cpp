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
#include "ffrt/cpp/pattern/job_partner.h"

namespace rnoh {

const std::string RAWFILE_PREFIX = "resource://RAWFILE/assets/";
const std::string INVALID_PATH_PREFIX = "invalidpathprefix/";
const std::string RESFILE_PREFIX = "file:///data/storage/el1/bundle/";
const std::string BASE_64_PREFIX = "data:";
const std::string BASE_64_MARK = ";base64,";
const std::int32_t BASE_64_FORMAT_LENGTH = 8; // length of ";base64,"
const std::int32_t BASE_64_MIME_TYPE_LENGTH = 6; // length of "image/"
const std::string BASE_64_STANDARD_PREFIX = "data:image/png;base64,";
const std::string RESFILE_PATH = "/resources/resfile/assets/";
const std::unordered_set<std::string> validImageTypes = {
    "png",
    "jpeg",
    "jpg",
    "gif",
    "bmp",
    "webp",
};

ImageComponentInstance::ImageComponentInstance(Context context)
    : CppComponentInstance(std::move(context)),
      ImageSourceResolver::ImageSourceUpdateListener(
        m_deps->imageSourceResolver),
      m_imageNode(m_arkUINodeCtx) {
  this->getLocalRootArkUINode().setNodeDelegate(this);
  this->getLocalRootArkUINode().setInterpolation(
      ARKUI_IMAGE_INTERPOLATION_HIGH);
  this->getLocalRootArkUINode().setDraggable(false);
  this->getLocalRootArkUINode().setAccessibilityMode(
    facebook::react::ImportantForAccessibility::Auto);
}

bool isValidMimeType(const std::string& mimeType) {
  if (mimeType.empty()) {
    return false;
  }

  if (mimeType.substr(0, BASE_64_MIME_TYPE_LENGTH) != "image/") {
    return false;
  }
  std::string imageType = mimeType.substr(BASE_64_MIME_TYPE_LENGTH);

  return validImageTypes.find(imageType) != validImageTypes.end();
}

std::string processBase64Uri(const std::string& uri) {
  size_t base64Pos = uri.find(BASE_64_MARK);
  if (base64Pos == std::string::npos) {
    return uri;
  }
  size_t mimeStart = BASE_64_PREFIX.length();
  std::string mimeType = uri.substr(mimeStart, base64Pos - mimeStart);
  if (base64Pos <= mimeStart || !isValidMimeType(mimeType)) {
    // Only change to image/png when MIME type is illegal.
    return BASE_64_STANDARD_PREFIX +
        uri.substr(base64Pos + BASE_64_FORMAT_LENGTH);
  }

  return uri;
}

void ImageComponentInstance::setSources(
    facebook::react::ImageSources const& sources) {
  // Defined layoutMetrics are necessary for determining the best source
  // for current container dimensions
  if (m_layoutMetrics == facebook::react::EmptyLayoutMetrics) {
    return;
  }

  auto uri = m_deps->imageSourceResolver->resolveImageSource(
      *this, m_layoutMetrics, sources);
  if (uri.rfind(BASE_64_PREFIX, 0) == 0 && uri.find(BASE_64_MARK) != std::string::npos) {
    uri = processBase64Uri(uri);
  }
  this->getLocalRootArkUINode().setSources(uri, getAbsolutePathPrefix(getBundlePath()));
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

std::string ImageComponentInstance::getHspModuleName() {
  if (!m_deps) {
    return "";
  }

  auto rnInstance = m_deps->rnInstance.lock();
  if (!rnInstance) {
    return "";
  }

  auto internalInstance = std::dynamic_pointer_cast<RNInstanceInternal>(rnInstance);
  if (!internalInstance) {
    return "";
  }
  return internalInstance->getHspModuleName();
}

std::string ImageComponentInstance::getAbsolutePathPrefix(std::string const& bundlePath) {
  if (bundlePath == INVALID_PATH_PREFIX) {
    return INVALID_PATH_PREFIX;
  }
  
  auto pos = bundlePath.rfind('/');
  if (pos == std::string::npos || bundlePath.find('/', 0) != 0) {
    if (this->getHspModuleName().empty()) {
      return std::string(RAWFILE_PREFIX);
    }
    return std::string(RESFILE_PREFIX) + this->getHspModuleName() +
        std::string(RESFILE_PATH);
  }

  std::string prefix = "file://" + bundlePath.substr(0, pos + 1);
  return prefix;
}

void ImageComponentInstance::onPropsChanged(SharedConcreteProps const& props) {
  CppComponentInstance::onPropsChanged(props);

  auto rawProps = ImageRawProps::getFromDynamic(props->rawProps);

  if (!m_props || m_props->sources != props->sources) {
    setSources(props->sources);
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

void ImageComponentInstance::onImageSourceCacheUpdate() {
  if (m_state != nullptr) {
    setSources({m_state->getData().getImageSource()});
  }
}

void ImageComponentInstance::onStateChanged(SharedConcreteState const& state) {
  CppComponentInstance::onStateChanged(state);
  setSources({state->getData().getImageSource()});
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
