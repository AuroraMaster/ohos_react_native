/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

//
// Created on 2024/5/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONY_TEXTMEASUREREGISTRY_H
#define HARMONY_TEXTMEASUREREGISTRY_H

#include <map>
#include "RNOH/ArkUITypography.h"
#include <react/renderer/textlayoutmanager/TextMeasureCache.h>
#include <arkui/styled_string.h>
#include <mutex>

struct TextMeasureInfo {
  TextMeasureInfo();
  TextMeasureInfo(rnoh::ArkUITypographyBuilder builder,
    rnoh::ArkUITypography tmpTypography) : 
    builder(std::move(builder)),
    typography(std::move(tmpTypography)) {}
  rnoh::ArkUITypographyBuilder builder;
  rnoh::ArkUITypography typography;
};

class TextMeasureRegistry {
public:
  static TextMeasureRegistry& getTextMeasureRegistry();
  void setTextMeasureInfo(const std::string& key, std::shared_ptr<TextMeasureInfo> textMeasureInfo, facebook::react::TextMeasureCacheKey& cacheKey);
  ArkUI_StyledString* getTextStyledString(const std::string& key);
  std::optional<std::shared_ptr<TextMeasureInfo>> getTextMeasureInfoByKey(const std::string& key);
  void eraseTextMeasureInfo(const std::string& key);
  std::optional<std::shared_ptr<TextMeasureInfo>> getTextMeasureInfo(
    const facebook::react::TextMeasureCacheKey& cacheKey, float scale);
  void clear();

 private:

  std::unordered_map<std::string, facebook::react::TextMeasureCacheKey> m_keyToCacheKey; // saved which cacheKey using by key

  std::unordered_map<std::string, std::shared_ptr<TextMeasureInfo>> m_keyToMeasureInfo; // saved which measureInfo using by key

  mutable folly::EvictingCacheMap<facebook::react::TextMeasureCacheKey, std::shared_ptr<TextMeasureInfo>>
    m_textMeasureInfoCache{facebook::react::kSimpleThreadSafeCacheSizeCap}; // cached all measure result
  std::mutex m_mutex;
};




#endif //HARMONY_TEXTMEASUREREGISTRY_H