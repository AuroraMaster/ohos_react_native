/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
// RNOH patch — apply fix from https://github.com/facebook/react-native/pull/43410/files
#include "LongLivedObject.h"
#include <unordered_map>

namespace facebook {
namespace react {

// LongLivedObjectCollection
LongLivedObjectCollection& LongLivedObjectCollection::get(
    jsi::Runtime& runtime) {
  static std::unordered_map<void*, std::shared_ptr<LongLivedObjectCollection>>
      instances;
  void* key = static_cast<void*>(&runtime);
  auto entry = instances.find(key);
  if (entry == instances.end()) {
    entry =
        instances.emplace(key, std::make_shared<LongLivedObjectCollection>())
            .first;
  }
  return *(entry->second);
}

void LongLivedObjectCollection::add(std::shared_ptr<LongLivedObject> so) {
  std::lock_guard<std::mutex> lock(collectionMutex_);
  collection_.insert(std::move(so));
}

void LongLivedObjectCollection::remove(const LongLivedObject *o) {
  std::lock_guard<std::mutex> lock(collectionMutex_);
  for (auto p = collection_.begin(); p != collection_.end(); p++) {
    if (p->get() == o) {
      collection_.erase(p);
      break;
    }
  }
}

void LongLivedObjectCollection::clear() {
  std::lock_guard<std::mutex> lock(collectionMutex_);
  collection_.clear();
}

size_t LongLivedObjectCollection::size() const {
  std::lock_guard<std::mutex> lock(collectionMutex_);
  return collection_.size();
}

// LongLivedObject

void LongLivedObject::allowRelease() {
  LongLivedObjectCollection::get(runtime_).remove(this);
}

} // namespace react
} // namespace facebook
