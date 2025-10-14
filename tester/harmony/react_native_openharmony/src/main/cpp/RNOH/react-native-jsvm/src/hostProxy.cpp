/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
 
#include "hostProxy.h"
#include <glog/logging.h>
#include "JSVMConverter.h"
#include "JSVMRuntime.h"
#include "RNOH/Assert.h"

namespace rnjsvm {

JSVM_PropertyHandlerConfigurationStruct HostObjectProxy::propertyCfg = {
  .genericNamedPropertyGetterCallback = HostObjectProxy::Getter,
  .genericNamedPropertySetterCallback = HostObjectProxy::Setter,
  .genericNamedPropertyDeleterCallback = nullptr,
  .genericNamedPropertyEnumeratorCallback = HostObjectProxy::Enumerator,
  .genericIndexedPropertyGetterCallback = HostObjectProxy::Getter,
  .genericIndexedPropertySetterCallback = HostObjectProxy::Setter,
  .genericIndexedPropertyDeleterCallback = nullptr,
  .genericIndexedPropertyEnumeratorCallback = nullptr,
  .namedPropertyData = nullptr,
  .indexedPropertyData = nullptr
};

HostObjectProxy::HostObjectProxy(
    JSVMRuntime& rt,
    JSVM_Env env,
    std::shared_ptr<HostObject> ho)
    : HostProxy(ProxyType::T_HOST_OBJECT), hostObject(ho), rt(rt) {}

JSVM_Value HostObjectProxy::Getter(
    JSVM_Env env,
    JSVM_Value name,
    JSVM_Value thisArg,
    JSVM_Value namedPropertyData) {
  void* data = nullptr;

  auto status = OH_JSVM_Unwrap(env, thisArg, &data);

  if (!data || status != JSVM_OK) {
    // TODO: throw error
    JSVM_Value result = nullptr;
    OH_JSVM_GetUndefined(env, &result);
    return result;
  }

  HostObjectProxy* hostObjectProxy = static_cast<HostObjectProxy*>(data);
  Value ret = Value::undefined();
  try {
    ret = hostObjectProxy->hostObject->get(
        hostObjectProxy->rt, JSVMConverter::make<PropNameID>(env, name));
  } catch (const JSError& error) {
    JSVM_Value errorValue = JSVMConverter::JsiToJSVM(env, error.value());
    OH_JSVM_Throw(env, errorValue);
  } catch (const std::exception& ex) {
    std::string exceptionString("Exception in HostObejctProxy.Getter:");
    exceptionString += ex.what();
    OH_JSVM_ThrowError(env, nullptr, exceptionString.c_str());
  } catch (...) {
    std::string exceptionString("Exception in HostObejctProxy.Getter: <unknown>");
    OH_JSVM_ThrowError(env, nullptr, exceptionString.c_str());
  }

  return JSVMConverter::JsiToJSVM(env, ret);
}

JSVM_Value HostObjectProxy::Setter(
    JSVM_Env env,
    JSVM_Value name,
    JSVM_Value property,
    JSVM_Value thisArg,
    JSVM_Value namedPropertyData) {
  void* data = nullptr;

  auto status = OH_JSVM_Unwrap(env, thisArg, &data);

  if (!data || status != JSVM_OK) {
    // TODO: throw error
    JSVM_Value result = nullptr;
    OH_JSVM_GetUndefined(env, &result);
    return result;
  }

  HostObjectProxy* hostObjectProxy = static_cast<HostObjectProxy*>(data);
  try {
    hostObjectProxy->hostObject->set(
        hostObjectProxy->rt,
        JSVMConverter::make<PropNameID>(env, name),
        JSVMConverter::JSVMToJsi(env, property));
  } catch (const JSError& error) {
    JSVM_Value errorValue = JSVMConverter::JsiToJSVM(env, error.value());
    OH_JSVM_Throw(env, errorValue);
    return JSVMConverter::JsiToJSVM(env, Value::undefined());
  } catch (const std::exception& ex) {
    std::string exceptionString("Exception in HostObejctProxy.Setter:");
    exceptionString += ex.what();
    OH_JSVM_ThrowError(env, nullptr, exceptionString.c_str());
    return JSVMConverter::JsiToJSVM(env, Value::undefined());
  } catch (...) {
    std::string exceptionString("Exception in HostObejctProxy.Setter: <unknown>");
    OH_JSVM_ThrowError(env, nullptr, exceptionString.c_str());
    return JSVMConverter::JsiToJSVM(env, Value::undefined());
  }

  // TODO: return value
  return property;
}

JSVM_Value HostObjectProxy::Enumerator(
    JSVM_Env env,
    JSVM_Value thisArg,
    JSVM_Value namedPropertyData) {
  // TODO: check
  HostObjectProxy* hostObjectProxy;
  OH_JSVM_Unwrap(
      env, thisArg, reinterpret_cast<void**>(&hostObjectProxy));
  // TODO: check

  const auto& names =
      hostObjectProxy->hostObject->getPropertyNames(hostObjectProxy->rt);
  size_t size = names.size();

  JSVM_Value nameArr;

  OH_JSVM_CreateArrayWithLength(env, size, &nameArr);

  for (uint32_t idx = 0; idx < size; ++idx) {
    JSVM_Value name =
        JSVMConverter::PointerValueToJSVM<PropNameID>(env, names[idx]);
    OH_JSVM_SetElement(env, nameArr, idx, name);
  }

  return nameArr;
}

void HostObjectProxy::Finalizer(
    JSVM_Env env,
    void* finalizeData,
    void* finalizeHint) {}

HostProxy* HostProxy::GetHostProxyFromJSVM(JSVM_Env env, JSVM_Value value) {
  HostProxy* hostProxy = nullptr;

  OH_JSVM_Unwrap(env, value, reinterpret_cast<void**>(&hostProxy));

  return hostProxy;
}

JSVM_Value HostFunctionProxy::FunctionCallback(
    JSVM_Env env,
    JSVM_CallbackInfo info) {
  size_t argc = 0;
  JSVM_Value thisArg = nullptr;
  void* data = nullptr;

  // Get args size info and this pointer
  OH_JSVM_GetCbInfo(env, info, &argc, nullptr, &thisArg, &data);
  RNOH_ASSERT(data && "can not acquire function");

  // Get and construct jsi arguments
  constexpr uint32_t PREALLOC_SIZE = 8;
  JSVM_Value argvStack[PREALLOC_SIZE];
  Value valueStack[PREALLOC_SIZE];
  JSVM_Value* argv = argvStack;
  Value* values = valueStack;

  if (argc > PREALLOC_SIZE) {
    argv = new JSVM_Value[argc];
    values = new Value[argc];
  }

  OH_JSVM_GetCbInfo(env, info, &argc, argv, nullptr, nullptr);

  for (uint32_t idx = 0; idx < argc; idx++) {
    values[idx] = JSVMConverter::JSVMToJsi(env, argv[idx]);
  }

  HostFunctionProxy* proxy = static_cast<HostFunctionProxy*>(data);

  // convert this to jsi value
  Value thisVal = JSVMConverter::JSVMToJsi(env, thisArg);

  // call jsi function
  Value ret = Value::undefined();
  try {
    ret = proxy->hostFunction(proxy->rt, thisVal, values, argc);
  } catch (const JSError& error) {
    JSVM_Value errorValue = JSVMConverter::JsiToJSVM(env, error.value());
    OH_JSVM_Throw(env, errorValue);
  } catch (const std::exception& ex) {
    std::string exceptionString("Exception in HostFunction: ");
    exceptionString += ex.what();
    OH_JSVM_ThrowError(env, nullptr, exceptionString.c_str());
  } catch (...) {
    std::string exceptionString("Exception in HostFunction: <unknown>");
    OH_JSVM_ThrowError(env, nullptr, exceptionString.c_str());
  }

  if (argc > PREALLOC_SIZE) {
    delete[] argv;
    delete[] values;
  }

  return JSVMConverter::JsiToJSVM(env, ret);
}
} // namespace rnjsvm