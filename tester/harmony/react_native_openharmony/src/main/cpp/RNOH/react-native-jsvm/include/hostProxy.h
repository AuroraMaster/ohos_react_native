/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
 
#ifndef HOST_PROXY_H
#define HOST_PROXY_H

#include "ark_runtime/jsvm.h"
#include "common.h"

namespace rnjsvm {
class JSVMRuntime;

class HostProxy {
 protected:
  enum class ProxyType : uint8_t {
    T_NONE = 0,
    T_HOST_OBJECT,
    T_HOST_FUNCTION,
    T_NATIVE_STATE
  };

  HostProxy(ProxyType type) : type(type) {}

 public:
  static HostProxy* GetHostProxyFromJSVM(JSVM_Env env, JSVM_Value value);

  bool IsHostObject() const {
    return type == ProxyType::T_HOST_OBJECT;
  }

  bool IsHostFunction() const {
    return type == ProxyType::T_HOST_FUNCTION;
  }

  bool HasNativeState() const {
    return type == ProxyType::T_NATIVE_STATE;
  }

 private:
  ProxyType type;
};

class HostObjectProxy : public HostProxy {
 public:
  HostObjectProxy(
      JSVMRuntime& rt,
      JSVM_Env env,
      std::shared_ptr<HostObject> ho);

  /** A callback function triggered by getting a named property of an instance
   * object. */
  static JSVM_Value Getter(
      JSVM_Env env,
      JSVM_Value name,
      JSVM_Value thisArg,
      JSVM_Value namedPropertyData);

  /** A callback function triggered by setting a named property of an instance
   * object. */
  static JSVM_Value Setter(
      JSVM_Env env,
      JSVM_Value name,
      JSVM_Value property,
      JSVM_Value thisArg,
      JSVM_Value namedPropertyData);

  /** A callback function triggered by getting all indexed properties requests
   * on an object. */
  static JSVM_Value
  Enumerator(JSVM_Env env, JSVM_Value thisArg, JSVM_Value namedPropertyData);

  static void Finalizer(JSVM_Env env, void* finalizeData, void* finalizeHint);

  static JSVM_PropertyHandlerConfigurationStruct *GetConfigure() {
    return &propertyCfg;
  }

  std::shared_ptr<HostObject> GetHostObject() const {
    return hostObject;
  }

 private:
  static JSVM_PropertyHandlerConfigurationStruct propertyCfg;
  std::shared_ptr<HostObject> hostObject;
  JSVMRuntime &rt;
  
};

class ObjectNativeState: public HostProxy {
  public:
    ObjectNativeState(JSVMRuntime &rt, std::shared_ptr<NativeState> state) 
    : HostProxy(ProxyType::T_NATIVE_STATE), nativeState(state), rt(rt) {}
  
  std::shared_ptr<NativeState> GetNativeState() const {
    return nativeState;
  }

  private:
    std::shared_ptr<NativeState> nativeState;
    JSVMRuntime &rt;
};

class HostFunctionProxy : public HostProxy {
 public:
  HostFunctionProxy(JSVMRuntime& rt, HostFunctionType hostFunction)
      : HostProxy(ProxyType::T_HOST_FUNCTION),
        hostFunction(hostFunction),
        rt(rt) {
    param.callback = FunctionCallback;
    param.data = static_cast<void*>(this);
  }

  HostFunctionType &GetHostFunction() {
    return hostFunction;
  }

  JSVM_CallbackStruct* GetCallback() {
    return &param;
  }

  static JSVM_Value FunctionCallback(JSVM_Env env, JSVM_CallbackInfo info);

 private:
  JSVM_CallbackStruct param;
  HostFunctionType hostFunction;
  JSVMRuntime& rt;
};

} // namespace rnjsvm
#endif