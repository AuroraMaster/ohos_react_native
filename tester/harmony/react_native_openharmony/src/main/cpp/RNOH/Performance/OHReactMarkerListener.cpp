/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "OHReactMarkerListener.h"

namespace rnoh {

OHReactMarkerListener& OHReactMarkerListener::getInstance() {
  static OHReactMarkerListener l;
  return l;
}

void OHReactMarkerListener::logMarker(
    const HarmonyReactMarkerId markerId,
    const char* tag,
    const double timestamp) {
  switch (markerId) {
    case HarmonyReactMarkerId::RUN_JS_BUNDLE_START:
      logMarkerStart("RUN_JS_BUNDLE", tag);
      break;
    case HarmonyReactMarkerId::RUN_JS_BUNDLE_STOP:
      logMarkerFinish("RUN_JS_BUNDLE", tag);
      break;
    case HarmonyReactMarkerId::CREATE_REACT_CONTEXT_START:
      logMarkerStart("CREATE_REACT_CONTEXT", "");
      break;  
    case HarmonyReactMarkerId::CREATE_REACT_CONTEXT_STOP:
      logMarkerFinish("CREATE_REACT_CONTEXT", "");
      break;
    case HarmonyReactMarkerId::JS_BUNDLE_STRING_CONVERT_START:
      logMarkerStart("loadApplicationScript_StringConvert", "");
      break;
    case HarmonyReactMarkerId::JS_BUNDLE_STRING_CONVERT_STOP:
      logMarkerFinish("loadApplicationScript_StringConvert", "");
      break;
    case HarmonyReactMarkerId::REGISTER_JS_SEGMENT_START:
      logMarkerStart("REGISTER_JS_SEGMENT", tag);
      break;
    case HarmonyReactMarkerId::REGISTER_JS_SEGMENT_STOP:
      logMarkerFinish("REGISTER_JS_SEGMENT", tag);
      break;
    case HarmonyReactMarkerId::CONTENT_APPEARED:
      logMarker("CONTENT_APPEARED", tag);
      break;
    case HarmonyReactMarkerId::CHANGE_THREAD_PRIORITY:
      logMarker("CHANGE_THREAD_PRIORITY", tag);
    break;  
    case HarmonyReactMarkerId::FABRIC_COMMIT_START:
      logMarkerStart("FABRIC_COMMIT", "");
      break;
    case HarmonyReactMarkerId::FABRIC_COMMIT_END:
      logMarkerFinish("FABRIC_COMMIT", "");
      break;
    case HarmonyReactMarkerId::FABRIC_FINISH_TRANSACTION_START:
      logMarkerStart("FABRIC_FINISH_TRANSACTION", "");
      break;
    case HarmonyReactMarkerId::FABRIC_FINISH_TRANSACTION_END:
      logMarkerFinish("FABRIC_FINISH_TRANSACTION", "");
      break;
    case HarmonyReactMarkerId::FABRIC_DIFF_START:
      logMarkerStart("FABRIC_DIFF", "");
      break;
    case HarmonyReactMarkerId::FABRIC_DIFF_END:
      logMarkerFinish("FABRIC_DIFF", "");
      break;
    case HarmonyReactMarkerId::FABRIC_LAYOUT_START:
      logMarkerStart("FABRIC_LAYOUT", "");
      break;
    case HarmonyReactMarkerId::FABRIC_LAYOUT_END:
      logMarkerFinish("FABRIC_LAYOUT", "");
      break;
    case HarmonyReactMarkerId::FABRIC_BATCH_EXECUTION_START:
      logMarkerStart("FABRIC_BATCH_EXECUTION", "");
      break;
    case HarmonyReactMarkerId::FABRIC_BATCH_EXECUTION_END:
      logMarkerFinish("FABRIC_BATCH_EXECUTION", "");
      break;
    case HarmonyReactMarkerId::FABRIC_UPDATE_UI_MAIN_THREAD_START:
      logMarkerStart("FABRIC_UPDATE_UI_MAIN_THREAD", "");
      break;
    case HarmonyReactMarkerId::FABRIC_UPDATE_UI_MAIN_THREAD_END:
      logMarkerFinish(
          "FABRIC_UPDATE_UI_MAIN_THREAD", "");
      break;
    case HarmonyReactMarkerId::REACT_BRIDGE_LOADING_START:
      logMarkerStart("REACT_BRIDGE_LOADING", tag);
      break;
    case HarmonyReactMarkerId::REACT_BRIDGE_LOADING_END:
      logMarkerFinish("REACT_BRIDGE_LOADING", tag);
      break;
    case HarmonyReactMarkerId::ON_HOST_RESUME_START:
      logMarkerStart("ON_HOST_RESUME", tag);
      break;
    case HarmonyReactMarkerId::ON_HOST_RESUME_END:
      logMarkerFinish("ON_HOST_RESUME", tag);
      break;
    case HarmonyReactMarkerId::ON_HOST_PAUSE_START:
      logMarkerStart("ON_HOST_PAUSE", tag);
      break;
    case HarmonyReactMarkerId::ON_HOST_PAUSE_END:
      logMarkerFinish("ON_HOST_PAUSE", tag);
      break;
    case HarmonyReactMarkerId::REACT_INSTANCE_INIT_START:
      logMarkerStart("REACT_INSTANCE_INIT", tag);
      break;
    case HarmonyReactMarkerId::REACT_INSTANCE_INIT_STOP:
      logMarkerFinish("REACT_INSTANCE_INIT", tag);
      break;
    case HarmonyReactMarkerId::INITIALIZE_MODULE_START:
      logMarkerStart("INITIALIZE_MODULE", tag);
      break;
    case HarmonyReactMarkerId::INITIALIZE_MODULE_END:
      logMarkerFinish("INITIALIZE_MODULE", tag);
      break;
    case HarmonyReactMarkerId::PROCESS_PACKAGES_START:
      logMarkerStart("PROCESS_PACKAGES", tag);
      break;
    case HarmonyReactMarkerId::PROCESS_PACKAGES_END:
      logMarkerFinish("PROCESS_PACKAGES", tag);
      break;
    case HarmonyReactMarkerId::CREATE_UI_MANAGER_MODULE_START:
      logMarkerStart("CREATE_UI_MANAGER_MODULE", tag);
      break;
    case HarmonyReactMarkerId::CREATE_UI_MANAGER_MODULE_END:
      logMarkerFinish("CREATE_UI_MANAGER_MODULE", tag);
      break;
    case HarmonyReactMarkerId::CREATE_MODULE_START:
      logMarkerStart("CREATE_MODULE", tag);
      break;
    case HarmonyReactMarkerId::CREATE_MODULE_END:
      logMarkerFinish("CREATE_MODULE", tag);
      break;
    case HarmonyReactMarkerId::PROCESS_CORE_REACT_PACKAGE_START:
      logMarkerStart("PROCESS_CORE_REACT_PACKAGE", tag);
      break;
    case HarmonyReactMarkerId::PROCESS_CORE_REACT_PACKAGE_END:
      logMarkerFinish("PROCESS_CORE_REACT_PACKAGE", tag);
      break;
    case HarmonyReactMarkerId::DOWNLOAD_START:
      logMarkerStart("DOWNLOAD", tag);
      break;  
    case HarmonyReactMarkerId::DOWNLOAD_END:
      logMarkerFinish("DOWNLOAD", tag);
      break;  
    case HarmonyReactMarkerId::NATIVE_REQUIRE_START:
    case HarmonyReactMarkerId::NATIVE_REQUIRE_STOP:
      break;
  }
}

void OHReactMarkerListener::logMarker(
    const std::string& marker,
    const std::string& tag) {
  logMarkerStart(marker, tag);
  logMarkerFinish(marker, tag);
}

void OHReactMarkerListener::logMarkerStart(
    const std::string& marker,
    const std::string& tag) { 
  auto message = makeMessage(marker, tag);
  OH_HiTrace_StartAsyncTrace(message.c_str(), getMessageId(message.c_str()));
}

void OHReactMarkerListener::logMarkerFinish(
    const std::string& marker,
    const std::string& tag) {
  auto message = makeMessage(marker, tag);
  OH_HiTrace_FinishAsyncTrace(message.c_str(), getMessageId(message.c_str()));
}

std::string OHReactMarkerListener::makeMessage(
    const std::string& marker,
    const std::string& tag) {
  std::string message = "ReactMarker::" + marker;
  if (!tag.empty()) {
    message += "::Tag::";
    message += tag;
  }
  return message;
};

int32_t OHReactMarkerListener::getMessageId(const std::string& message) {
  std::hash<std::string> hasher;
  size_t hash = hasher(message);
  // Ensure the hash fits into int32_t. We don't care that much about collisions
  int32_t small_hash = hash ^ (hash >> 32);
  return small_hash;
};

} // namespace rnoh