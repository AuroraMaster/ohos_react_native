/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include "ArkJS.h"

namespace rnoh {

struct PhysicalPixels {
  float top;
  float left;
  float width;
  float height;
  float decorHeight;
  float scale;
  float fontScale;
  float densityDpi;
  float xDpi;
  float yDpi;

  static PhysicalPixels fromNapiValue(napi_env env, napi_value value);
};

struct DisplayMetrics {
  PhysicalPixels windowPhysicalPixels;
  PhysicalPixels screenPhysicalPixels;

  static DisplayMetrics fromNapiValue(napi_env env, napi_value value);
};

class ArkTSBridge final {
  static std::shared_ptr<ArkTSBridge> instance;
  static std::once_flag initFlag;
  ArkTSBridge(napi_env env, napi_ref napiBridgeRef);

 public:
  using Shared = std::shared_ptr<ArkTSBridge>;

  static void initializeInstance(napi_env env, napi_ref arkTSBridgeHandlerRef);

  static ArkTSBridge::Shared getInstance();

  ArkTSBridge(ArkTSBridge const&) = delete;
  ArkTSBridge& operator=(ArkTSBridge const&) = delete;

  ~ArkTSBridge();

  void handleError(std::exception_ptr ex);
  DisplayMetrics getDisplayMetrics();
  float getScaleRatioDpiX() const;
  float getScaleRatioDpiY() const;
  void setScaleRatioDpi(float scaleRatioDpiX, float scaleRatioDpiY);
  uint32_t getFoldStatus();
  bool getIsSplitScreenMode();
  float getFontSizeScale();
  std::string getMetadata(std::string const& name);
  float getSDKApiVersion();

 protected:
  ArkJS m_arkJs;
  napi_ref m_arkTSBridgeRef;

 private:
  mutable std::mutex m_mutex;
  float m_scaleRatioDpiX = 1.0f;
  float m_scaleRatioDpiY = 1.0f;
};
} // namespace rnoh