/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/image/ImageEventEmitter.h>
#include <react/renderer/components/image/ImageShadowNode.h>
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/ImageNode.h"
#include "RNOH/ImageSourceResolver.h"

namespace rnoh {
class ImageComponentInstance
    : public CppComponentInstance<facebook::react::ImageShadowNode>,
      public ImageNodeDelegate,
      public ImageSourceResolver::ImageSourceUpdateListener {
 private:
  ImageNode m_imageNode;
  struct ImageRawProps {
    std::optional<std::string> resizeMethod;
    std::optional<bool> focusable;
    std::optional<std::string> alt;
    std::optional<std::string> loadingIndicatorSource;
    std::optional<int32_t> fadeDuration;
    static ImageRawProps getFromDynamic(folly::dynamic value);
  };
  ImageRawProps m_rawProps;

  void setSources(facebook::react::ImageSources const& sources);

  std::string getBundlePath();
  std::string getHspModuleName();
  std::string getAbsolutePathPrefix(std::string const& bundlePath);

 public:
  ImageComponentInstance(Context context);
  void onPropsChanged(SharedConcreteProps const& props) override;
  void onStateChanged(SharedConcreteState const& state) override;

  void onProgress(uint32_t loaded, uint32_t total) override;
  void onComplete(float width, float height) override;
  void onError(int32_t errorCode) override;
  void onLoadStart();

  ImageNode& getLocalRootArkUINode() override;

  // ImageSourceResolver::ImageSourceUpdateListener
  void onImageSourceCacheUpdate() override;
};
} // namespace rnoh
