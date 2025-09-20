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

#include "TextMeasureRegistry.h"
#include "TextMeasureCache.h"

TextMeasureRegistry& TextMeasureRegistry::getTextMeasureRegistry() {
  static TextMeasureRegistry TextMeasureRegistry;
  return TextMeasureRegistry;
}

void TextMeasureRegistry::setTextMeasureInfo(const std::string& key, std::shared_ptr<TextMeasureInfo> measureInfo, facebook::react::TextMeasureCacheKey& cacheKey) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_textMeasureInfoCache.set(cacheKey, measureInfo);
  m_keyToMeasureInfo.erase(key);
  m_keyToCacheKey.erase(key);
  m_keyToMeasureInfo.emplace(key, measureInfo);
  m_keyToCacheKey.emplace(key, cacheKey);
}

ArkUI_StyledString* TextMeasureRegistry::getTextStyledString(const std::string& key) {
  std::lock_guard<std::mutex> lock(m_mutex);
  auto itor = m_keyToMeasureInfo.find(key);
  if (itor != m_keyToMeasureInfo.end()) {
    return itor->second->builder.getTextStyleString();
  }
  return nullptr;
}

std::optional<std::shared_ptr<TextMeasureInfo>> TextMeasureRegistry::getTextMeasureInfo(
    const facebook::react::TextMeasureCacheKey& cacheKey, float scale)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  std::optional<std::shared_ptr<TextMeasureInfo>> measureInfo = std::nullopt;
  auto itor = m_textMeasureInfoCache.find(cacheKey);
  if (itor != m_textMeasureInfoCache.end()) {
    if (scale == itor->second->builder.getScale()) {
        measureInfo = itor->second;
    }
  }
  return measureInfo;
}

std::optional<std::shared_ptr<TextMeasureInfo>> TextMeasureRegistry::getTextMeasureInfoByKey(const std::string& key) {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::optional<std::shared_ptr<TextMeasureInfo>> measureInfo = std::nullopt;
  auto itor = m_keyToMeasureInfo.find(key);
  if (itor != m_keyToMeasureInfo.end()) {
    measureInfo = itor->second;
  }
  return measureInfo;
}

void TextMeasureRegistry::eraseTextMeasureInfo(const std::string& key) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_keyToMeasureInfo.find(key) != m_keyToMeasureInfo.end()) {
    m_keyToMeasureInfo.erase(key);
    m_keyToCacheKey.erase(key);
  }
}

void TextMeasureRegistry::clear() {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_keyToMeasureInfo.clear();
  m_keyToMeasureInfo.clear();
  m_textMeasureInfoCache.clear();
}