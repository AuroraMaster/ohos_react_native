/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#ifndef native_ArkJS_H
#define native_ArkJS_H

#include <folly/Function.h>
#include <folly/dynamic.h>
#include <jsi/jsi.h>
#include <react/renderer/graphics/Color.h>
#include <react/renderer/graphics/Float.h>
#include <react/renderer/graphics/RectangleCorners.h>
#include <array>
#include <functional>
#include <string>
#include <variant>
#include <vector>
#include "RNOH/RNOHError.h"
#include "RNOH/Result.h"
#include "ThreadGuard.h"
#include "napi/native_api.h"

class RNOHNapiObjectBuilder;
class RNOHNapiObject;
class NapiRef;

class ArkJS {
 public:
  using IntermediaryCallback = std::function<void(std::vector<folly::dynamic>)>;
  using IntermediaryArg = std::variant<folly::dynamic, IntermediaryCallback>;

  ArkJS(napi_env env);

  template <size_t args_count>
  napi_value call(
      napi_value callback,
      std::array<napi_value, args_count> args,
      napi_value thisObject = nullptr) {
    return call(callback, args.data(), args.size(), thisObject);
  }

  napi_value call(
      napi_value callback,
      std::vector<napi_value> args,
      napi_value thisObject = nullptr);

  napi_value call(
      napi_value callback,
      const napi_value* args,
      int argsCount,
      napi_value thisObject);

  napi_value createBoolean(bool value);

  napi_value createInt(int value);

  napi_value createDouble(double value);

  napi_value createString(std::string const& str);

  napi_ref createReference(napi_value value);

  void deleteReference(napi_ref reference);

  NapiRef createNapiRef(napi_value value);

  napi_value createSingleUseCallback(
      std::function<void(std::vector<folly::dynamic>)>&& callback);

  napi_value createFunction(
      std::string const& name,
      napi_callback callback,
      void* data = nullptr);

  napi_value createArray();

  napi_value createArray(std::vector<napi_value>);

  std::vector<napi_value> createFromDynamics(
      std::vector<folly::dynamic> const&);

  napi_value createFromDynamic(folly::dynamic const&);

  napi_value createFromException(std::exception const&);

  napi_value createFromJSError(facebook::jsi::JSError const&);

  napi_value createFromRNOHError(rnoh::RNOHError const&);

  napi_value createResult(rnoh::Result<napi_value> const&);

  RNOHNapiObjectBuilder createObjectBuilder();

  bool isPromise(napi_value);

  RNOHNapiObjectBuilder getObjectBuilder(napi_value object);

  napi_value getUndefined();

  napi_value getNull();

  napi_value getReferenceValue(napi_ref ref);

  napi_value getReferenceValue(NapiRef const& ref);

  std::vector<napi_value> getCallbackArgs(napi_callback_info info);

  std::vector<napi_value> getCallbackArgs(
      napi_callback_info info,
      size_t args_count);

  RNOHNapiObject getObject(napi_value object);

  RNOHNapiObject getObject(napi_ref objectRef);

  RNOHNapiObject getObject(NapiRef const& objectRef);

  napi_value getObjectProperty(napi_value object, std::string const& key);

  napi_value getObjectProperty(napi_value object, napi_value key);

    bool hasProperty(napi_value object, std::string const& key);

    bool hasProperty(napi_value object, napi_value key);

    bool getBoolean(napi_value value);

  double getDouble(napi_value value);

  int getInteger(napi_value value);

  napi_value getArrayElement(napi_value array, uint32_t index);

  uint32_t getArrayLength(napi_value array);

  std::vector<uint8_t> getArrayBuffer(napi_value array);

    bool isArrayBuffer(napi_value value);

    std::vector<std::pair<napi_value, napi_value>> getObjectProperties(
        napi_value object);

  std::string getString(napi_value value);

  folly::dynamic getDynamic(napi_value value);

  std::vector<folly::dynamic> getDynamics(std::vector<napi_value> values);

  napi_env getEnv();

  napi_valuetype getType(napi_value value);

  napi_value convertIntermediaryValueToNapiValue(IntermediaryArg arg);

  std::vector<napi_value> convertIntermediaryValuesToNapiValues(
      std::vector<IntermediaryArg> args);

 private:
  napi_env m_env;

  void maybeThrowFromStatus(napi_status status, const char* message);

  void maybeRethrowAsCpp(napi_status status);
};

class RNOHNapiObject {
 public:
  RNOHNapiObject(ArkJS arkJs, napi_value object);

  template <size_t args_count>
  napi_value call(
      std::string const& key,
      std::array<napi_value, args_count> args) {
    return m_arkJS.call(this->getProperty(key), args, m_object);
  }

  napi_value call(std::string const& key, std::vector<napi_value> args) {
    return m_arkJS.call(this->getProperty(key), args, m_object);
  }

  napi_value
  call(std::string const& key, const napi_value* args, int argsCount) {
    return m_arkJS.call(this->getProperty(key), args, argsCount, m_object);
  }

  napi_value getProperty(std::string const& key);

  napi_value getProperty(napi_value key);

  std::vector<std::pair<napi_value, napi_value>> getKeyValuePairs();

 private:
  ArkJS m_arkJS;
  napi_value m_object;

  friend class RNOHNapiObjectBuilder;
};

class RNOHNapiObjectBuilder {
 public:
  RNOHNapiObjectBuilder(napi_env env, ArkJS arkJs);

  RNOHNapiObjectBuilder(napi_env env, ArkJS arkJs, napi_value object);

  RNOHNapiObjectBuilder& addProperty(const char* name, napi_value value);

  RNOHNapiObjectBuilder& addProperty(const char* name, bool value);

  RNOHNapiObjectBuilder& addProperty(const char* name, int value);

  RNOHNapiObjectBuilder& addProperty(
      const char* name,
      std::array<facebook::react::Float, 16> matrix);

  RNOHNapiObjectBuilder& addProperty(
      const char* name,
      facebook::react::Float value);

  RNOHNapiObjectBuilder& addProperty(const char* name, char const* value);

  RNOHNapiObjectBuilder& addProperty(
      const char* name,
      facebook::react::SharedColor value);

  RNOHNapiObjectBuilder& addProperty(
      const char* name,
      facebook::react::RectangleCorners<facebook::react::Float> value);

  RNOHNapiObjectBuilder& addProperty(const char* name, std::string value);

  RNOHNapiObjectBuilder& addProperty(const char* name, folly::dynamic value);

  napi_value build();

 private:
  ArkJS m_arkJS;
  napi_env m_env;
  napi_value m_object;
  std::vector<std::pair<std::string, napi_value>> m_properties;
};

class Promise {
 public:
  Promise(napi_env env, napi_value value);

  Promise& then(std::function<void(std::vector<folly::dynamic>)>&& callback);
  Promise& catch_(std::function<void(std::vector<folly::dynamic>)>&& callback);

 private:
  ArkJS m_arkJS;
  napi_value m_value;
};

class NapiRef final {
 public:
  NapiRef() = default;

  // rule of five constructors
  NapiRef(NapiRef const&) = default;
  NapiRef& operator=(NapiRef const&) = default;

  NapiRef(NapiRef&&) = default;
  NapiRef& operator=(NapiRef&&) = default;

  ~NapiRef() = default;

  operator bool() const {
    return m_ref != nullptr;
  }

 private:
  class Deleter {
   public:
    void operator()(napi_ref ref) const {
      m_threadGuard.assertThread();
      napi_delete_reference(m_env, ref);
    }

    napi_env m_env;
    rnoh::ThreadGuard m_threadGuard;
  };

  NapiRef(napi_env env, napi_ref ref) : m_env(env), m_ref(ref, Deleter{env}) {}

  napi_env m_env{};
  std::shared_ptr<napi_ref__> m_ref{nullptr};

  friend class ArkJS;
};

#endif // native_ArkJS_H