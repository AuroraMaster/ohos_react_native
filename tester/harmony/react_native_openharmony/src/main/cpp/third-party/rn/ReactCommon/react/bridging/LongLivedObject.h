/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
// RNOH patch — apply fix from https://github.com/facebook/react-native/pull/43410/files

#pragma once

#include <jsi/jsi.h>
#include <memory>
#include <mutex>
#include <unordered_set>

namespace facebook {
namespace react {

/**
 * A simple wrapper class that can be registered to a collection that keep it
 * alive for extended period of time. This object can be removed from the
 * collection when needed.
 *
 * The subclass of this class must be created using std::make_shared<T>().
 * After creation, add it to the `LongLivedObjectCollection`.
 * When done with the object, call `allowRelease()` to allow the OS to release
 * it.
 */
class LongLivedObject {
 public:
  void allowRelease();

 protected:
  explicit LongLivedObject(jsi::Runtime& runtime) : runtime_(runtime) {}
  virtual ~LongLivedObject() = default;
  jsi::Runtime& runtime_;
};

/**
 * A singleton, thread-safe, write-only collection for the `LongLivedObject`s.
 */
class LongLivedObjectCollection {
 public:
  static LongLivedObjectCollection& get(jsi::Runtime& runtime);

  LongLivedObjectCollection() = default;
  LongLivedObjectCollection(LongLivedObjectCollection const &) = delete;
  void operator=(LongLivedObjectCollection const &) = delete;

  void add(std::shared_ptr<LongLivedObject> o);
  void remove(const LongLivedObject *o);
  void clear();
  size_t size() const;

 private:
  std::unordered_set<std::shared_ptr<LongLivedObject>> collection_;
  mutable std::mutex collectionMutex_;
};

} // namespace react
} // namespace facebook
