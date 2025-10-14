/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "HarmonyReactMarker.h"

namespace rnoh {

std::vector<HarmonyReactMarker::HarmonyReactMarkerListener*>
    HarmonyReactMarker::listeners = {};

void HarmonyReactMarker::setLogMarkerIfNeeded() {
  static std::once_flag flag{};
  std::call_once(flag, []() {
    ReactMarker::logTaggedMarkerImpl = HarmonyReactMarker::logMarker;
    ReactMarker::logTaggedMarkerBridgelessImpl = HarmonyReactMarker::logMarker;
    ReactMarker::getAppStartTimeImpl = HarmonyReactMarker::getAppStartTime;
  });
}

void HarmonyReactMarker::logMarker(const HarmonyReactMarkerId markerId) {
  logMarker(markerId, "");
}

void HarmonyReactMarker::logMarker(
    const HarmonyReactMarkerId markerId,
    facebook::react::Tag tag) {
  logMarker(markerId, std::to_string(tag).c_str());
}

void HarmonyReactMarker::logMarker(
    const HarmonyReactMarkerId markerId,
    const char* tag) {
  logMarker(markerId, tag, facebook::react::JSExecutor::performanceNow());
}

void HarmonyReactMarker::logMarker(
    const std::string& markerId,
    const char* tag) {
  logMarker(harmonyMarkerIdForString(markerId), tag);
}

void HarmonyReactMarker::logMarker(
    const ReactMarker::ReactMarkerId markerId,
    const char* tag) {
  logMarker(harmonyMarkerIdForReactMarkerId(markerId), tag);
}

void HarmonyReactMarker::logMarker(
    const HarmonyReactMarkerId markerId,
    const char* tag,
    const double timestamp) {
  for (auto listener : listeners) {
    listener->logMarker(markerId, tag, timestamp);
  }
}

void HarmonyReactMarker::setAppStartTime(double appStartTime) {
  sAppStartTime = appStartTime;
}

double HarmonyReactMarker::getAppStartTime() {
  return sAppStartTime;
}

void HarmonyReactMarker::addListener(HarmonyReactMarkerListener& listener) {
  listeners.push_back(&listener);
}

void HarmonyReactMarker::removeListener(HarmonyReactMarkerListener& listener) {
  listeners.erase(
      std::remove(listeners.begin(), listeners.end(), &listener),
      listeners.end());
}

HarmonyReactMarker::HarmonyReactMarkerId
HarmonyReactMarker::harmonyMarkerIdForReactMarkerId(
    const ReactMarker::ReactMarkerId markerId) {
  switch (markerId) {
    case ReactMarker::ReactMarkerId::RUN_JS_BUNDLE_START:
      return HarmonyReactMarkerId::RUN_JS_BUNDLE_START;
    case ReactMarker::ReactMarkerId::RUN_JS_BUNDLE_STOP:
      return HarmonyReactMarkerId::RUN_JS_BUNDLE_STOP;
    case ReactMarker::ReactMarkerId::CREATE_REACT_CONTEXT_STOP:
      return HarmonyReactMarkerId::CREATE_REACT_CONTEXT_STOP;
    case ReactMarker::ReactMarkerId::JS_BUNDLE_STRING_CONVERT_START:
      return HarmonyReactMarkerId::JS_BUNDLE_STRING_CONVERT_START;
    case ReactMarker::ReactMarkerId::JS_BUNDLE_STRING_CONVERT_STOP:
      return HarmonyReactMarkerId::JS_BUNDLE_STRING_CONVERT_STOP;
    case ReactMarker::ReactMarkerId::NATIVE_MODULE_SETUP_START:
      return HarmonyReactMarkerId::NATIVE_MODULE_SETUP_START;
    case ReactMarker::ReactMarkerId::NATIVE_MODULE_SETUP_STOP:
      return HarmonyReactMarkerId::NATIVE_MODULE_SETUP_STOP;
    case ReactMarker::ReactMarkerId::REGISTER_JS_SEGMENT_START:
      return HarmonyReactMarkerId::REGISTER_JS_SEGMENT_START;
    case ReactMarker::ReactMarkerId::REGISTER_JS_SEGMENT_STOP:
      return HarmonyReactMarkerId::REGISTER_JS_SEGMENT_STOP;
    case ReactMarker::ReactMarkerId::NATIVE_REQUIRE_START:
      return HarmonyReactMarkerId::NATIVE_REQUIRE_START;
    case ReactMarker::ReactMarkerId::NATIVE_REQUIRE_STOP:
      return HarmonyReactMarkerId::NATIVE_REQUIRE_STOP;
    case ReactMarker::ReactMarkerId::REACT_INSTANCE_INIT_START:
      return HarmonyReactMarkerId::REACT_INSTANCE_INIT_START;
    case ReactMarker::ReactMarkerId::REACT_INSTANCE_INIT_STOP:
      return HarmonyReactMarkerId::REACT_INSTANCE_INIT_STOP;
  }
}

HarmonyReactMarker::HarmonyReactMarkerId
HarmonyReactMarker::harmonyMarkerIdForString(const std::string& markerId) {
  if (markerId == "CREATE_REACT_CONTEXT_START") {
    return HarmonyReactMarkerId::CREATE_REACT_CONTEXT_START;
  }
  if (markerId == "DOWNLOAD_START") {
    return HarmonyReactMarkerId::DOWNLOAD_START;
  }
  if (markerId == "DOWNLOAD_END") {
    return HarmonyReactMarkerId::DOWNLOAD_END;
  }
  throw std::invalid_argument("Unknown markerId: " + markerId);
}

} // namespace rnoh
