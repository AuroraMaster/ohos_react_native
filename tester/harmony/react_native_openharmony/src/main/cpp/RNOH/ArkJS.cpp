/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "ArkJS.h"
#include <stdexcept>
#include <string>
#include "napi/native_api.h"

static void
maybeThrowFromStatus(napi_env env, napi_status status, const char* message) {
  if (status != napi_ok) {
    napi_extended_error_info const* error_info;
    napi_get_last_error_info(env, &error_info);
    std::string msg_str = message;
    std::string error_code_msg_str = ". Error code: ";
    std::string status_str = error_info->error_message;
    std::string full_msg = msg_str + error_code_msg_str + status_str;
    napi_throw_error(env, nullptr, message);
    // stops a code execution after throwing napi_error
    throw std::runtime_error(full_msg);
  }
}

void ArkJS::maybeRethrowAsCpp(napi_status status) {
  if (status == napi_ok) {
    return;
  }
  bool hasThrown;
  napi_is_exception_pending(m_env, &hasThrown);
  if (status != napi_pending_exception && !hasThrown) {
    return;
  }
  napi_value nError;
  napi_get_and_clear_last_exception(m_env, &nError);

  auto message = getObjectProperty(nError, "message");
  auto messageStr = getString(message);
  throw std::runtime_error(messageStr);
}

ArkJS::ArkJS(napi_env env) {
  m_env = env;
}

napi_env ArkJS::getEnv() {
  return m_env;
}

napi_value ArkJS::call(
    napi_value callback,
    std::vector<napi_value> args,
    napi_value thisObject) {
  return call(callback, args.data(), args.size(), thisObject);
}

napi_value ArkJS::call(
    napi_value callback,
    const napi_value* args,
    int argsCount,
    napi_value thisObject) {
  napi_value result = nullptr;
  auto status =
      napi_call_function(m_env, thisObject, callback, argsCount, args, &result);
  this->maybeRethrowAsCpp(status);
  return result;
}

napi_value ArkJS::createBoolean(bool value) {
  napi_value result;
  napi_get_boolean(m_env, value, &result);
  return result;
}

napi_value ArkJS::createInt(int value) {
  napi_value result;
  napi_create_int32(m_env, static_cast<int32_t>(value), &result);
  return result;
}

napi_value ArkJS::createDouble(double value) {
  napi_value result;
  napi_create_double(m_env, value, &result);
  return result;
}

napi_value ArkJS::createString(std::string const& str) {
  napi_value result;
  auto status =
      napi_create_string_utf8(m_env, str.c_str(), str.length(), &result);
  this->maybeThrowFromStatus(status, "Failed to create string");
  return result;
}

napi_value ArkJS::getUndefined() {
  napi_value result;
  napi_get_undefined(m_env, &result);
  return result;
}

napi_value ArkJS::getNull() {
  napi_value result;
  napi_get_null(m_env, &result);
  return result;
}

RNOHNapiObjectBuilder ArkJS::createObjectBuilder() {
  return RNOHNapiObjectBuilder(m_env, *this);
}

std::vector<napi_value> ArkJS::createFromDynamics(
    std::vector<folly::dynamic> const& dynamics) {
  std::vector<napi_value> results(dynamics.size());
  for (size_t i = 0; i < dynamics.size(); ++i) {
    results[i] = this->createFromDynamic(dynamics[i]);
  }
  return results;
}

napi_value ArkJS::createFromDynamic(folly::dynamic const& dyn) {
  if (dyn.isString()) {
    return this->createString(dyn.asString());
  } else if (dyn.isObject()) {
    auto objectBuilder = this->createObjectBuilder();
    for (const auto& [key, value] : dyn.items()) {
      objectBuilder.addProperty(
          key.asString().c_str(), this->createFromDynamic(value));
    }
    return objectBuilder.build();
  } else if (dyn.isDouble()) {
    return this->createDouble(dyn.asDouble());
  } else if (dyn.isBool()) {
    return this->createBoolean(dyn.asBool());
  } else if (dyn.isArray()) {
    std::vector<napi_value> n_values(dyn.size());
    for (size_t i = 0; i < dyn.size(); ++i) {
      n_values[i] = this->createFromDynamic(dyn[i]);
    }
    return this->createArray(n_values);
  } else if (dyn.isInt()) {
    return this->createDouble(dyn.asInt());
  } else if (dyn.isNull()) {
    return this->getNull();
  } else {
    return this->getUndefined();
  }
}

napi_value ArkJS::createFromException(std::exception const& e) {
  folly::dynamic errData = folly::dynamic::object("message", e.what());
  return this->createFromDynamic(errData);
}

napi_value ArkJS::createFromJSError(facebook::jsi::JSError const& e) {
  folly::dynamic errData = folly::dynamic::object;
  errData["message"] = e.getMessage();
  try {
    std::rethrow_if_nested(e);
  } catch (std::exception const& nested) {
    errData["details"] = nested.what();
  }
  folly::dynamic dynStacktrace = folly::dynamic::array;
  dynStacktrace.push_back(e.getStack());
  errData["stacktrace"] = std::move(dynStacktrace);
  return this->createFromDynamic(errData);
}

napi_value ArkJS::createFromRNOHError(rnoh::RNOHError const& e) {
  folly::dynamic errData = folly::dynamic::object;
  errData["message"] = e.getMessage();

  folly::dynamic dynStacktrace = folly::dynamic::array;
  for (const auto& trace : e.getStacktrace()) {
    dynStacktrace.push_back(trace);
  }
  errData["stacktrace"] = dynStacktrace;

  folly::dynamic dynSuggestions = folly::dynamic::array;
  for (const auto& trace : e.getSuggestions()) {
    dynSuggestions.push_back(trace);
  }
  errData["suggestions"] = dynSuggestions;

  try {
    std::rethrow_if_nested(e);
  } catch (std::exception const& nested) {
    errData["details"] = nested.what();
  }
  return this->createFromDynamic(errData);
}

napi_value ArkJS::createResult(const rnoh::Result<napi_value>& result) {
  if (!result.isOk()) {
    auto resultBuilder =
        this->createObjectBuilder().addProperty("ok", this->getNull());
    try {
      std::rethrow_exception(result.unwrapErr());
    } catch (const rnoh::RNOHError& e) {
      resultBuilder.addProperty("err", this->createFromRNOHError(e));
    } catch (const facebook::jsi::JSError& e) {
      resultBuilder.addProperty("err", this->createFromJSError(e));
    } catch (const std::exception& e) {
      resultBuilder.addProperty("err", this->createFromException(e));
    }
    return resultBuilder.build();
  }
  return this->createObjectBuilder()
      .addProperty("ok", result.unwrap())
      .addProperty("err", this->getNull())
      .build();
}

napi_value ArkJS::getReferenceValue(napi_ref ref) {
  napi_value result;
  auto status = napi_get_reference_value(m_env, ref, &result);
  this->maybeThrowFromStatus(status, "Couldn't get a reference value");
  return result;
}

napi_ref ArkJS::createReference(napi_value value) {
  napi_ref result;
  auto status = napi_create_reference(m_env, value, 1, &result);
  this->maybeThrowFromStatus(status, "Couldn't create a reference");
  return result;
}

void ArkJS::deleteReference(napi_ref reference) {
  auto status = napi_delete_reference(m_env, reference);
  this->maybeThrowFromStatus(status, "Couldn't delete a reference");
}

napi_value ArkJS::getReferenceValue(NapiRef const& ref) {
  RNOH_ASSERT(ref.m_env == m_env);
  return getReferenceValue(ref.m_ref.get());
}

NapiRef ArkJS::createNapiRef(napi_value value) {
  return {m_env, createReference(value)};
}


std::function<napi_value(napi_env, std::vector<napi_value>)>*
createNapiCallback(
    std::function<void(std::vector<folly::dynamic>)>&& callback) {
  return new std::function(
      [callback = std::move(callback)](
          napi_env env,
          std::vector<napi_value> callbackNapiArgs) mutable -> napi_value {
        ArkJS arkJS(env);
        callback(arkJS.getDynamics(callbackNapiArgs));
        return arkJS.getUndefined();
      });
}

napi_value singleUseCallback(napi_env env, napi_callback_info info) {
  void* data;
  napi_get_cb_info(env, info, nullptr, nullptr, nullptr, &data);
  auto callback = static_cast<
      std::function<napi_value(napi_env, std::vector<napi_value>)>*>(data);
  ArkJS arkJS(env);
  (*callback)(env, arkJS.getCallbackArgs(info));
  delete callback;
  return arkJS.getUndefined();
}

/*
 * The callback will be deallocated after is called. It cannot be called more
 * than once. Creates memory leaks if the callback is not called. Consider
 * changing this implementation when adding napi finalizers is supported. .
 */
napi_value ArkJS::createSingleUseCallback(
    std::function<void(std::vector<folly::dynamic>)>&& callback) {
  return createFunction(
      "callback", singleUseCallback, createNapiCallback(std::move(callback)));
}

napi_value ArkJS::createFunction(
    std::string const& name,
    napi_callback callback,
    void* data) {
  napi_value result;
  auto status = napi_create_function(
      m_env, name.c_str(), name.length(), callback, data, &result);
  this->maybeThrowFromStatus(status, "Couldn't create a function");
  return result;
}

napi_value ArkJS::createArray() {
  napi_value result;
  napi_create_array(m_env, &result);
  return result;
}

napi_value ArkJS::createArray(std::vector<napi_value> values) {
  napi_value result;
  napi_create_array(m_env, &result);
  for (size_t i = 0; i < values.size(); i++) {
    napi_set_element(m_env, result, i, values[i]);
  }
  return result;
}

std::vector<napi_value> ArkJS::getCallbackArgs(napi_callback_info info) {
  size_t argc;
  napi_get_cb_info(m_env, info, &argc, nullptr, nullptr, nullptr);
  return getCallbackArgs(info, argc);
}

std::vector<napi_value> ArkJS::getCallbackArgs(
    napi_callback_info info,
    size_t args_count) {
  size_t argc = args_count;
  std::vector<napi_value> args(args_count, nullptr);
  napi_get_cb_info(m_env, info, &argc, args.data(), nullptr, nullptr);
  return args;
}

RNOHNapiObject ArkJS::getObject(napi_value object) {
  return RNOHNapiObject(*this, object);
}

RNOHNapiObject ArkJS::getObject(napi_ref objectRef) {
  return getObject(getReferenceValue(objectRef));
}

bool ArkJS::hasProperty(napi_value object, std::string const& key) {
  return hasProperty(object, this->createString(key));
}

RNOHNapiObject ArkJS::getObject(NapiRef const& objectRef) {
  return getObject(getReferenceValue(objectRef));
}

bool ArkJS::hasProperty(napi_value object, napi_value key) {
  bool result;
  auto status = napi_has_property(m_env, object, key, &result);
  this->maybeThrowFromStatus(status, "Failed to check if object has property");
  return result;
}

napi_value ArkJS::getObjectProperty(napi_value object, std::string const& key) {
  return getObjectProperty(object, this->createString(key));
}

napi_value ArkJS::getObjectProperty(napi_value object, napi_value key) {
  napi_value result;
  auto status = napi_get_property(m_env, object, key, &result);
  this->maybeThrowFromStatus(status, "Failed to retrieve property from object");
  return result;
}

bool ArkJS::getBoolean(napi_value value) {
  bool result;
  auto status = napi_get_value_bool(m_env, value, &result);
  this->maybeThrowFromStatus(status, "Failed to retrieve boolean value");
  return result;
}

int ArkJS::getInteger(napi_value value) {
  int result;
  auto status = napi_get_value_int32(m_env, value, &result);
  this->maybeThrowFromStatus(status, "Failed to retrieve integer value");
  return result;
}

double ArkJS::getDouble(napi_value value) {
  double result;
  auto status = napi_get_value_double(m_env, value, &result);
  this->maybeThrowFromStatus(status, "Failed to retrieve double value");
  return result;
}

napi_value ArkJS::getArrayElement(napi_value array, uint32_t index) {
  napi_value result;
  auto status = napi_get_element(m_env, array, index, &result);
  this->maybeThrowFromStatus(status, "Failed to retrieve value at index");
  return result;
}

uint32_t ArkJS::getArrayLength(napi_value array) {
  uint32_t length;
  auto status = napi_get_array_length(m_env, array, &length);
  this->maybeThrowFromStatus(status, "Failed to read array length");
  return length;
}

std::vector<uint8_t> ArkJS::getArrayBuffer(napi_value array) {
  void* data;
  size_t length;
  auto status = napi_get_arraybuffer_info(m_env, array, &data, &length);
  this->maybeThrowFromStatus(status, "Failed to read array buffer");
  return std::vector<uint8_t>(
      static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + length);
}

std::vector<std::pair<napi_value, napi_value>> ArkJS::getObjectProperties(
    napi_value object) {
  napi_value propertyNames;
  auto status = napi_get_property_names(m_env, object, &propertyNames);
  this->maybeThrowFromStatus(status, "Failed to retrieve property names");
  uint32_t length = this->getArrayLength(propertyNames);
  std::vector<std::pair<napi_value, napi_value>> result;
  for (uint32_t i = 0; i < length; i++) {
    napi_value propertyName = this->getArrayElement(propertyNames, i);
    napi_value propertyValue = this->getObjectProperty(object, propertyName);
    result.emplace_back(propertyName, propertyValue);
  }
  return result;
}

std::string ArkJS::getString(napi_value value) {
  size_t length;
  napi_status status;
  status = napi_get_value_string_utf8(m_env, value, nullptr, 0, &length);
  this->maybeThrowFromStatus(status, "Failed to get the length of the string");
  std::string buffer(length, '\0');
  status = napi_get_value_string_utf8(
      m_env, value, buffer.data(), length + 1, &length);
  this->maybeThrowFromStatus(status, "Failed to get the string data");
  return buffer;
}

void ArkJS::maybeThrowFromStatus(napi_status status, const char* message) {
  ::maybeThrowFromStatus(m_env, status, message);
}

napi_valuetype ArkJS::getType(napi_value value) {
  napi_valuetype result;
  auto status = napi_typeof(m_env, value, &result);
  this->maybeThrowFromStatus(status, "Failed to get value type");
  return result;
}

folly::dynamic ArkJS::getDynamic(napi_value value) {
  switch (this->getType(value)) {
    case napi_undefined:
      return folly::dynamic(nullptr);
    case napi_null:
      return folly::dynamic(nullptr);
    case napi_boolean:
      return folly::dynamic(this->getBoolean(value));
    case napi_number:
      return folly::dynamic(this->getDouble(value));
    case napi_string:
      return folly::dynamic(this->getString(value));
    case napi_object: {
      bool isArray;
      auto status = napi_is_array(m_env, value, &isArray);
      assert(status == napi_ok);
      if (isArray) {
        auto arrayLength = this->getArrayLength(value);
        folly::dynamic result = folly::dynamic::array();
        for (uint32_t i = 0; i < arrayLength; ++i) {
          result.push_back(this->getDynamic(this->getArrayElement(value, i)));
        }
        return result;
      } else {
        folly::dynamic result = folly::dynamic::object();
        for (auto [key, val] : this->getObject(value).getKeyValuePairs()) {
          result[this->getString(key)] = this->getDynamic(val);
        }
        return result;
      }
    }
    default:
      return folly::dynamic(nullptr);
  }
}

std::vector<folly::dynamic> ArkJS::getDynamics(std::vector<napi_value> values) {
  std::vector<folly::dynamic> dynamics;
  for (auto value : values) {
    dynamics.push_back(this->getDynamic(value));
  }
  return dynamics;
}

std::vector<napi_value> ArkJS::convertIntermediaryValuesToNapiValues(
    std::vector<IntermediaryArg> args) {
  std::vector<napi_value> napiArgs;
  for (auto& arg : args) {
    napiArgs.push_back(convertIntermediaryValueToNapiValue(std::move(arg)));
  }
  return napiArgs;
}

napi_value ArkJS::convertIntermediaryValueToNapiValue(IntermediaryArg arg) {
  return std::visit(
      [this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, folly::dynamic>) {
          return this->createFromDynamic(std::move(arg));
        } else if constexpr (std::is_same_v<T, IntermediaryCallback>) {
          return this->createSingleUseCallback(std::move(arg));
        } else {
          static_assert(
              std::is_same_v<T, folly::dynamic> ||
                  std::is_same_v<T, IntermediaryCallback>,
              "invalid type passed!");
        }
      },
      std::move(arg));
}

RNOHNapiObjectBuilder::RNOHNapiObjectBuilder(napi_env env, ArkJS arkJS)
    : m_env(env), m_arkJS(arkJS) {
  napi_value obj;
  auto status = napi_create_object(env, &obj);
  maybeThrowFromStatus(env, status, "Failed to create an object");
  m_object = obj;
}

RNOHNapiObjectBuilder::RNOHNapiObjectBuilder(
    napi_env env,
    ArkJS arkJS,
    napi_value object)
    : m_env(env), m_arkJS(arkJS), m_object(object) {}

RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    napi_value value) {
  m_properties.emplace_back(name, value);
  return *this;
}

RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    bool value) {
  addProperty(name, m_arkJS.createBoolean(value));
  return *this;
}

RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    int value) {
  auto napi_value = m_arkJS.createInt(value);
  addProperty(name, napi_value);
  return *this;
}

RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    facebook::react::Float value) {
  addProperty(name, m_arkJS.createDouble(value));
  return *this;
}

RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    char const* value) {
  addProperty(name, m_arkJS.createString(value));
  return *this;
}

RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    facebook::react::SharedColor value) {
  if (!value) {
    addProperty(name, m_arkJS.getUndefined());
    return *this;
  }
  auto colorComponents = colorComponentsFromColor(value);
  napi_value n_value;
  napi_create_array(m_env, &n_value);
  napi_set_element(
      m_env, n_value, 0, m_arkJS.createDouble(colorComponents.red));
  napi_set_element(
      m_env, n_value, 1, m_arkJS.createDouble(colorComponents.green));
  napi_set_element(
      m_env, n_value, 2, m_arkJS.createDouble(colorComponents.blue));
  napi_set_element(
      m_env, n_value, 3, m_arkJS.createDouble(colorComponents.alpha));
  addProperty(name, n_value);
  return *this;
}
RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    facebook::react::RectangleCorners<facebook::react::Float> value) {
  napi_value n_value;
  napi_create_object(m_env, &n_value);

  napi_property_descriptor corners[] = {
      {"topLeft",
       nullptr,
       nullptr,
       nullptr,
       nullptr,
       m_arkJS.createDouble(value.topLeft),
       napi_default_jsproperty,
       nullptr},
      {"topRight",
       nullptr,
       nullptr,
       nullptr,
       nullptr,
       m_arkJS.createDouble(value.topRight),
       napi_default_jsproperty,
       nullptr},
      {"bottomLeft",
       nullptr,
       nullptr,
       nullptr,
       nullptr,
       m_arkJS.createDouble(value.bottomLeft),
       napi_default_jsproperty,
       nullptr},
      {"bottomRight",
       nullptr,
       nullptr,
       nullptr,
       nullptr,
       m_arkJS.createDouble(value.bottomRight),
       napi_default_jsproperty,
       nullptr}};

  napi_define_properties(m_env, n_value, 4, corners);
  addProperty(name, n_value);
  return *this;
}
RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    std::array<facebook::react::Float, 16> matrix) {
  napi_value n_value;
  napi_create_array_with_length(m_env, matrix.size(), &n_value);

  for (std::size_t i = 0; i < matrix.size(); ++i) {
    napi_set_element(m_env, n_value, i, m_arkJS.createDouble(matrix[i]));
  }

  addProperty(name, n_value);
  return *this;
}

RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    std::string value) {
  addProperty(name, m_arkJS.createString(value));
  return *this;
}

RNOHNapiObjectBuilder& RNOHNapiObjectBuilder::addProperty(
    const char* name,
    folly::dynamic value) {
  addProperty(name, m_arkJS.createFromDynamic(value));
  return *this;
}

napi_value RNOHNapiObjectBuilder::build() {
  if (!m_properties.empty()) {
    std::vector<napi_property_descriptor> properties;

    for (auto const& [key, value] : m_properties) {
      properties.push_back(
          napi_property_descriptor{
              key.c_str(), // UTF-8 encoded property name
              nullptr, // name string as napi_value

              nullptr, // method implementation
              nullptr, // getter
              nullptr, // setter
              value, // property value as napi_value

              napi_default_jsproperty, // attributes
              nullptr // data
          });
    }
    auto status = napi_define_properties(
        m_env, m_object, properties.size(), properties.data());
    maybeThrowFromStatus(m_env, status, "Failed to create an object");
  }
  return m_object;
}

RNOHNapiObject::RNOHNapiObject(ArkJS arkJS, napi_value object)
    : m_arkJS(arkJS), m_object(object) {}

napi_value RNOHNapiObject::getProperty(std::string const& key) {
  return m_arkJS.getObjectProperty(m_object, key);
}

napi_value RNOHNapiObject::getProperty(napi_value key) {
  return m_arkJS.getObjectProperty(m_object, key);
}

std::vector<std::pair<napi_value, napi_value>>
RNOHNapiObject::getKeyValuePairs() {
  return m_arkJS.getObjectProperties(m_object);
}

bool ArkJS::isPromise(napi_value value) {
  bool result;
  napi_is_promise(m_env, value, &result);
  return result;
}

RNOHNapiObjectBuilder ArkJS::getObjectBuilder(napi_value object) {
  return RNOHNapiObjectBuilder(m_env, *this, object);
}

Promise::Promise(napi_env env, napi_value value)
    : m_arkJS(ArkJS(env)), m_value(value) {}

Promise& Promise::then(
    std::function<void(std::vector<folly::dynamic>)>&& callback) {
  auto obj = m_arkJS.getObject(m_value);
  obj.call("then", {m_arkJS.createSingleUseCallback(std::move(callback))});
  return *this;
}

Promise& Promise::catch_(
    std::function<void(std::vector<folly::dynamic>)>&& callback) {
  auto obj = m_arkJS.getObject(m_value);
  obj.call("catch", {m_arkJS.createSingleUseCallback(std::move(callback))});
  return *this;
}

bool ArkJS::isArrayBuffer(napi_value value) {
  bool result = false;
  maybeThrowFromStatus(
      napi_is_arraybuffer(m_env, value, &result),
      "Failed to check if value is an ArrayBuffer");
  return result;
}
