/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "HarmonyReactMarker.h"

namespace rnoh {
class OHReactMarkerListener
    : public HarmonyReactMarker::HarmonyReactMarkerListener {
 public:
  using HarmonyReactMarkerId = HarmonyReactMarker::HarmonyReactMarkerId;

  static OHReactMarkerListener& getInstance();
  void logMarker(
      const HarmonyReactMarkerId markerId,
      const char* tag,
      const double timestamp) override;

 private:
  void logMarker(const std::string& marker, const std::string& tag);
  void logMarkerStart(const std::string& marker, const std::string& tag);
  void logMarkerFinish(const std::string& marker, const std::string& tag);
  int32_t getMessageId(const std::string& message);
  std::string makeMessage(const std::string& marker, const std::string& tag);
};

} // namespace rnoh
