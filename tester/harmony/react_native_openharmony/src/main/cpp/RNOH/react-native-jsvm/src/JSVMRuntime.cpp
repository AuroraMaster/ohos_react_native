/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
 
#include "JSVMRuntime.h"
#include "JSVMConverter.h"
#include <glog/logging.h>
#include "mutex.h"
#include "hostProxy.h"
#include "JSVMUtil.h"
#include <filesystem>
#include <fstream>
#include <random>
#include "RNOH/Assert.h"

#define DFX()                  \
    do {                       \
        LOG(INFO) << __func__; \
    } while (0)

#define DFX()
namespace rnjsvm {

bool JSVMRuntime::initialized = false;
std::mutex JSVMRuntime::codeCacheMtx;
std::unordered_map<std::string, std::vector<uint8_t>> JSVMRuntime::codeCacheL2 = {};
thread_local bool JSVMPointerValue::isJsThread = false;

JSVMRuntime::JSVMRuntime(folly::dynamic initOptions)
    : hostObjectClass(nullptr) {
  DFX();
  static std::mutex lockInit;
  {
    const std::lock_guard<std::mutex> lock(lockInit);
    if (!initialized) {
      auto len = initOptions.size();
      if (len > 0) {
        int argc = len + 1;
        std::vector<std::string> temp(len);
        std::vector<const char*> args(argc);
        for (size_t i = 0; i < len; i++) {
          temp.push_back(initOptions[i].asString());
          args[i + 1] = temp.back().c_str();
        }
        JSVM_InitOptions init_options{
            nullptr, &argc, (char**)args.data(), true};
        OH_JSVM_Init(&init_options);
      } else {
        OH_JSVM_Init(nullptr);
      }
      initialized = true;
        }
    }

    JSVMPointerValue::isJsThread = true;
    memset(&options, 0, sizeof(options));
    OH_JSVM_CreateVM(&options, &vm);
    OH_JSVM_OpenVMScope(vm, &vmScope);
    OH_JSVM_CreateEnv(vm, 0, nullptr, &env);
    OH_JSVM_OpenEnvScope(env, &envScope);
}

JSVMRuntime::JSVMRuntime(
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
    folly::dynamic initOptions)
    : JSVMRuntime(initOptions) {
    this->jsQueue = jsQueue;
    OH_JSVM_SetInstanceData(env, reinterpret_cast<facebook::react::MessageQueueThread *>(jsQueue.get()), nullptr, nullptr);
}

JSVMRuntime::~JSVMRuntime()
{
    OH_JSVM_CloseEnvScope(env, envScope);
    OH_JSVM_DestroyEnv(env);
    OH_JSVM_CloseVMScope(vm, vmScope);
    OH_JSVM_DestroyVM(vm);
}

Value JSVMRuntime::evaluateJavaScript(const std::shared_ptr<const Buffer> &buffer, const std::string &sourceURL)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    // 编译js代码
    auto jsSrc = std::make_shared<JSVM_Value>();
    OH_JSVM_CreateStringUtf8(env, reinterpret_cast<const char *>(buffer->data()), buffer->size(), jsSrc.get());
    bool cacheRejected = true;
    std::string name = std::filesystem::is_regular_file(sourceURL)
      ? cachePath + sourceURL
      : (cachePath/std::filesystem::path(sourceURL)).string();
    auto cache = GetCodeCache(name);

    // Memory leaks! OH_JSVM_ReleaseScript not available on NEXT-DB3
    auto script = std::make_shared<JSVM_Script>();

    CALL_JSVM_AND_THROW(OH_JSVM_CompileScript(env, *jsSrc, cache.empty() ? nullptr : cache.data(), cache.size(), true,
      &cacheRejected, script.get()));

    // 执行js代码
    auto result = std::make_shared<JSVM_Value>();
    CALL_JSVM_AND_THROW(OH_JSVM_RunScript(env, *script, result.get()));

    if (cache.empty() || cacheRejected) {
      const uint8_t* data;
      size_t len;
      CALL_JSVM_AND_THROW(OH_JSVM_CreateCodeCache(env, *script, &data, &len));
      // Memory leaks! OH_JSVM_ReleaseCache not available on NEXT-DB3
      auto buffer = std::make_shared<std::vector<uint8_t>>(data, data + len);
      UpdateCodeCache(name, *buffer);
    }

    return JSVMConverter::JSVMToJsi(env, *result);
}

std::shared_ptr<const PreparedJavaScript> JSVMRuntime::prepareJavaScript(
    const std::shared_ptr<const Buffer> &buffer, std::string sourceURL)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    // 编译js代码
    const char* src = reinterpret_cast<const char*>(buffer->data());
    JSVM_Value jsSrc;
    CALL_JSVM_AND_THROW(OH_JSVM_CreateStringUtf8(env, src, buffer->size(), &jsSrc));
    bool cacheRejected = true;
    JSVM_Script script;
    CALL_JSVM_AND_THROW(OH_JSVM_CompileScript(env, jsSrc, nullptr, 0, true, &cacheRejected, &script));

    // 将js源码编译出的脚本保存到cache
    const uint8_t* dataPtr = nullptr;
    size_t lengthPtr = 0;
    CALL_JSVM_AND_THROW(OH_JSVM_CreateCodeCache(env, script, &dataPtr, &lengthPtr));

    return std::make_shared<const JSVMPreparedJavaScript>(dataPtr, lengthPtr, buffer);
}

Value JSVMRuntime::evaluatePreparedJavaScript(const std::shared_ptr<const PreparedJavaScript> &js)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    // codeCache还原为JSVM_Script
    auto sourceJs =
        std::static_pointer_cast<const JSVMPreparedJavaScript>(js);
    JSVM_Value jsSrc;
    CALL_JSVM_AND_THROW(OH_JSVM_CreateStringUtf8(
        env, reinterpret_cast<const char*>(sourceJs->source()), sourceJs->sourceLength(), &jsSrc));
    bool cacheRejected = true;
    JSVM_Script script;
    CALL_JSVM_AND_THROW(OH_JSVM_CompileScript(
        env, jsSrc, sourceJs->codeCache(), sourceJs->codeLength(), true, &cacheRejected, &script));

    // 执行js代码
    JSVM_Value result;
    CALL_JSVM_AND_THROW(OH_JSVM_RunScript(env, script, &result));

    return JSVMConverter::JSVMToJsi(env, result);
}

bool JSVMRuntime::drainMicrotasks(int maxMicrotasksHint)
{
    return true;
}

// TODO: optimize
Object JSVMRuntime::global()
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value global = nullptr;
    CALL_JSVM(env, OH_JSVM_GetGlobal(env, &global));
    
    return JSVMConverter::make<Object>(env, global);
}

std::string JSVMRuntime::description()
{
    return "JSVM Runtime";
}

bool JSVMRuntime::isInspectable()
{
    return false;
}

inline Runtime::PointerValue *JSVMRuntime::clone(const Runtime::PointerValue *pv) {
    return reinterpret_cast<JSVMPointerValue *>(const_cast<Runtime::PointerValue *>(pv))->Ref();
}

Runtime::PointerValue* JSVMRuntime::cloneSymbol(
    const Runtime::PointerValue* pv) {
  DFX();
  return clone(pv);
}

Runtime::PointerValue* JSVMRuntime::cloneBigInt(
    const Runtime::PointerValue* pv) {
  DFX();
  return clone(pv);
}

Runtime::PointerValue* JSVMRuntime::cloneString(
    const Runtime::PointerValue* pv) {
  DFX();
  return clone(pv);
}

Runtime::PointerValue* JSVMRuntime::cloneObject(
    const Runtime::PointerValue* pv) {
  DFX();
  return clone(pv);
}

Runtime::PointerValue* JSVMRuntime::clonePropNameID(
    const Runtime::PointerValue* pv) {
  DFX();
  return clone(pv);
}

PropNameID JSVMRuntime::createPropNameIDFromAscii(const char *str, size_t length)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = nullptr;
    OH_JSVM_CreateStringLatin1(env, str, length, &value);
    return JSVMConverter::make<PropNameID>(env, value);
}

PropNameID JSVMRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = nullptr;
    OH_JSVM_CreateStringUtf8(env, reinterpret_cast<const char*>(utf8), length, &value);
    return JSVMConverter::make<PropNameID>(env, value);
}

PropNameID JSVMRuntime::createPropNameIDFromString(const String &str)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = JSVMConverter::PointerValueToJSVM(env, str);
    return JSVMConverter::make<PropNameID>(env, value);
}

PropNameID JSVMRuntime::createPropNameIDFromSymbol(const Symbol &sym)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = JSVMConverter::PointerValueToJSVM(env, sym);
    return JSVMConverter::make<PropNameID>(env, value);
}

std::string JSVMRuntime::utf8(const PropNameID& propNameID) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);
  JSVM_Value value = JSVMConverter::PointerValueToJSVM(env, propNameID);
  bool isSymbol = false;
  OH_JSVM_IsSymbol(env, value, &isSymbol);
  JSVM_Value JsvmString = nullptr;
  if (isSymbol) {
    const char* funcName = "String";
    JSVM_Value args[] = {value};
    constexpr uint32_t argc = sizeof(args) / sizeof(JSVM_Value);
    JsvmString = CallGlobalFunction(funcName, strlen(funcName), args, argc);
  } else {
    CALL_JSVM_AND_THROW(OH_JSVM_CoerceToString(env, value, &JsvmString));
  }

  return GetValueString(JsvmString);
}

bool JSVMRuntime::compare(const PropNameID &propNameID1, const PropNameID &propNameID2)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value lvalue = JSVMConverter::PointerValueToJSVM(env, propNameID1);
    JSVM_Value rvalue = JSVMConverter::PointerValueToJSVM(env, propNameID2);

    bool isStrictEquals = false;
    auto status = OH_JSVM_StrictEquals(env, lvalue, rvalue, &isStrictEquals);
    ClearException(status);

    return isStrictEquals;
}

std::string JSVMRuntime::symbolToString(const Symbol& symbol) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);

  const char* funcName = "String";
  JSVM_Value args[] = {JSVMConverter::PointerValueToJSVM(env, symbol)};
  constexpr uint32_t argc = sizeof(args) / sizeof(JSVM_Value);
  JSVM_Value symbolString =
      CallGlobalFunction(funcName, strlen(funcName), args, argc);
  return GetValueString(symbolString);
}

BigInt JSVMRuntime::createBigIntFromInt64(int64_t numberValue)
{
  JSVMUtil::HandleScopeWrapper scope(env);
  JSVM_Value result = nullptr;
  OH_JSVM_CreateBigintInt64(env, numberValue, &result);
  return JSVMConverter::make<BigInt>(env, result);
}

BigInt JSVMRuntime::createBigIntFromUint64(uint64_t numberValue)
{
  JSVMUtil::HandleScopeWrapper scope(env);
  JSVM_Value result = nullptr;
  OH_JSVM_CreateBigintUint64(env, numberValue, &result);
  return JSVMConverter::make<BigInt>(env, result);
}

bool JSVMRuntime::bigintIsInt64(const BigInt &value)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_value = 
        JSVMConverter::PointerValueToJSVM(env, value);

    bool lossless = false;
    int64_t resultValue = 0;
    OH_JSVM_GetValueBigintInt64(env, jsvm_value, &resultValue, &lossless);
    
    return lossless;
}

bool JSVMRuntime::bigintIsUint64(const BigInt &value)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_value = 
        JSVMConverter::PointerValueToJSVM(env, value);

    bool lossless = false;
    uint64_t resultValue = 0;
    JSVM_Status status = 
        OH_JSVM_GetValueBigintUint64(env, jsvm_value, &resultValue, &lossless);
    RNOH_ASSERT(status == JSVM_OK && "GetValueBigintUint64 faild.");
    
    return lossless;
}

uint64_t JSVMRuntime::truncate(const BigInt &value)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_value = 
        JSVMConverter::PointerValueToJSVM(env, value);

    bool lossless = false;
    uint64_t resultValue = 0;
    JSVM_Status status = 
        OH_JSVM_GetValueBigintUint64(env, jsvm_value, &resultValue, &lossless);
    RNOH_ASSERT(status == JSVM_OK && "GetValueBigintUint64 faild.");
    
    return resultValue;
}

String JSVMRuntime::bigintToString(const BigInt &value, int radix)
{
    DFX();
    if (radix < RADIX_MIN || radix > RADIX_MAX) {
        throw JSError(*this, "radix is not in the [2, 36] range.");
    }
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_value =
        JSVMConverter::PointerValueToJSVM(env, value);

    JSVM_Value global = nullptr;
    CALL_JSVM(env, OH_JSVM_GetGlobal(env, &global));

    JSVM_Value bigintClass = nullptr;
    const char *bigIntStringStr = "BigInt";
    JSVM_Value bigIntString = nullptr;
    CALL_JSVM(env, OH_JSVM_CreateStringUtf8(env, bigIntStringStr, strlen(bigIntStringStr), &bigIntString));
    CALL_JSVM(env, OH_JSVM_GetProperty(env, global, bigIntString, &bigintClass));

    JSVM_Value bigintNewResult = nullptr;
    JSVM_Value bigInt_args[1] = {jsvm_value};
    CALL_JSVM(env, OH_JSVM_CallFunction(env, global, bigintClass, 1, bigInt_args, &bigintNewResult));

    JSVM_Value bigintToStringFunction = nullptr;
    const char *toStringStringStr = "toString";
    JSVM_Value toStringString = nullptr;
    CALL_JSVM(env, OH_JSVM_CreateStringUtf8(env, toStringStringStr, strlen(toStringStringStr), &toStringString));
    CALL_JSVM(env, OH_JSVM_GetProperty(env, bigintNewResult, toStringString, &bigintToStringFunction));

    JSVM_Value toStringResult = nullptr;
    JSVM_Value radix_jsvm_value = nullptr;
    CALL_JSVM(env, OH_JSVM_CreateInt32(env, radix, &radix_jsvm_value));
    JSVM_Value toString_args[1] = {radix_jsvm_value};
    JSVM_Status status =
        OH_JSVM_CallFunction(env, bigintNewResult, bigintToStringFunction, 1, toString_args, &toStringResult);

    return JSVMConverter::make<String>(env, toStringResult);
}

String JSVMRuntime::createStringFromAscii(const char *str, size_t length)
{
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);
  JSVM_Value ptr = nullptr;
  OH_JSVM_CreateStringLatin1(env, str, length, &ptr);
  return JSVMConverter::make<String>(env, ptr);
}

String JSVMRuntime::createStringFromUtf8(const uint8_t *utf8, size_t length)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);
    JSVM_Value ptr = nullptr;
    const char *char_ptr = reinterpret_cast<const char*>(utf8);
    OH_JSVM_CreateStringUtf8(env, char_ptr, length, &ptr);
    return JSVMConverter::make<String>(env, ptr);
}

std::string JSVMRuntime::utf8(const String& str) {
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);
    JSVM_Value JSVMStr = JSVMConverter::PointerValueToJSVM(env, str);
    return GetValueString(JSVMStr);
}

Value JSVMRuntime::createValueFromJsonUtf8(const uint8_t *json, size_t length)
{
    DFX();
    return this->Runtime::createValueFromJsonUtf8(json, length);
}

Object JSVMRuntime::createObject()
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);
    JSVM_Value ptr = nullptr;
    OH_JSVM_CreateObject(env, &ptr);
    return JSVMConverter::make<Object>(env,ptr);
}

Object JSVMRuntime::createObject(std::shared_ptr<HostObject> ho) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);

  HostObjectProxy* hostProxy = new HostObjectProxy(*this, env, ho);

  // Init hostObjectClass
  if (unlikely(!hostObjectClass)) {
    JSVM_Callback ctor = new JSVM_CallbackStruct;
    ctor->callback = [](JSVM_Env env, JSVM_CallbackInfo info) -> JSVM_Value {
        JSVM_Value thisVar = nullptr;
        CALL_JSVM(
            env, OH_JSVM_GetCbInfo(env, info, nullptr, nullptr, &thisVar, nullptr));
        return thisVar;
    };
    ctor->data = nullptr;

    const char className[] = "native object";
    constexpr size_t len = sizeof(className) - 1;
    
    JSVM_Value tmp = nullptr;
    CALL_JSVM_AND_THROW(
        OH_JSVM_DefineClassWithPropertyHandler(
            env,
            className,
            len,
            ctor,
            0,
            nullptr,
            HostObjectProxy::GetConfigure(),
            nullptr,
            &tmp));
    OH_JSVM_CreateReference(env, tmp, 1, &hostObjectClass);
  }

  JSVM_Value localHostObjectClass = nullptr;
  OH_JSVM_GetReferenceValue(env, hostObjectClass, &localHostObjectClass);
  
  JSVM_Value instanceValue = nullptr;
  CALL_JSVM_AND_THROW(
      OH_JSVM_NewInstance(env, localHostObjectClass, 0, nullptr, &instanceValue));
  CALL_JSVM_AND_THROW(OH_JSVM_Wrap(
      env,
      instanceValue,
      (void*)hostProxy,
      [](JSVM_Env env, void* data, void* hint) {
        delete static_cast<HostObjectProxy *>(data);
      },
      nullptr,
      nullptr));

  return JSVMConverter::make<Object>(env, instanceValue);
}

std::shared_ptr<HostObject> JSVMRuntime::getHostObject(const Object& object) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);
  JSVM_Value jsObject = 
        JSVMConverter::PointerValueToJSVM(env, object);
  
  HostProxy *hostProxy = HostProxy::GetHostProxyFromJSVM(env, jsObject);
  RNOH_ASSERT(hostProxy && hostProxy->IsHostObject());

  return static_cast<HostObjectProxy *>(hostProxy)->GetHostObject();
}

HostFunctionType& JSVMRuntime::getHostFunction(const Function& func) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);
  JSVM_Value jsFunc = JSVMConverter::PointerValueToJSVM(env, func);

  HostProxy* hostProxy = HostProxy::GetHostProxyFromJSVM(env, jsFunc);
  RNOH_ASSERT(hostProxy && hostProxy->IsHostFunction());

  return static_cast<HostFunctionProxy*>(hostProxy)->GetHostFunction();
}

bool JSVMRuntime::hasNativeState(const Object& obj) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);
  JSVM_Value jsObject = JSVMConverter::PointerValueToJSVM(env, obj);
  HostProxy* hostProxy = HostProxy::GetHostProxyFromJSVM(env, jsObject);
  return hostProxy && hostProxy->HasNativeState();
}

std::shared_ptr<NativeState> JSVMRuntime::getNativeState(const Object& obj) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);
  const JSVM_Value jsObject = JSVMConverter::PointerValueToJSVM(env, obj);
  HostProxy* hostProxy = HostProxy::GetHostProxyFromJSVM(env, jsObject);
  RNOH_ASSERT(hostProxy && !hostProxy->HasNativeState());

  return static_cast<ObjectNativeState*>(hostProxy)->GetNativeState();
}

void JSVMRuntime::setNativeState(
    const Object& obj,
    std::shared_ptr<NativeState> state) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);
  ObjectNativeState* objectNativeState = new ObjectNativeState(*this, state);
  JSVM_Value object = JSVMConverter::PointerValueToJSVM(env, obj);
  OH_JSVM_Wrap(
      env,
      object,
      static_cast<void*>(objectNativeState),
      [](JSVM_Env env, void* data, void* hint) {
        delete static_cast<ObjectNativeState *>(data);
      },
      nullptr,
      nullptr);
}

Value JSVMRuntime::getProperty(const Object &object, const PropNameID &name)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_object = 
        JSVMConverter::PointerValueToJSVM(env, object);

    const JSVM_Value jsvm_name = 
        JSVMConverter::PointerValueToJSVM(env, name);

    JSVM_Value propResult = nullptr;
    JSVM_Status status = 
        OH_JSVM_GetProperty(env, jsvm_object, jsvm_name, &propResult);
    RNOH_ASSERT(status == JSVM_OK && "OH_JSVM_GetProperty faild.");

    // If not get property, return undefined
    return JSVMConverter::JSVMToJsi(env, propResult);
}

Value JSVMRuntime::getProperty(const Object& object, const String& name) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);

  const JSVM_Value jsvm_object = JSVMConverter::PointerValueToJSVM(env, object);

  const JSVM_Value jsvm_name = JSVMConverter::PointerValueToJSVM(env, name);

  JSVM_Value propResult = nullptr;
  JSVM_Status status =
      OH_JSVM_GetProperty(env, jsvm_object, jsvm_name, &propResult);

  ClearException(status);
  return JSVMConverter::JSVMToJsi(env, propResult);
}

bool JSVMRuntime::hasProperty(const Object &value, const PropNameID &name)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_object = 
      JSVMConverter::PointerValueToJSVM(env, value);

    const JSVM_Value jsvm_name = 
    JSVMConverter::PointerValueToJSVM(env, name);

    bool result = false;
    JSVM_Status status =
        OH_JSVM_HasProperty(env, jsvm_object, jsvm_name, &result);
    ClearException(status);

    return result;
}

bool JSVMRuntime::hasProperty(const Object &value, const String &name)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_object = 
      JSVMConverter::PointerValueToJSVM(env, value);

    const JSVM_Value jsvm_name = 
    JSVMConverter::PointerValueToJSVM(env, name);

    bool result = false;
    JSVM_Status status =
        OH_JSVM_HasProperty(env, jsvm_object, jsvm_name, &result);
    ClearException(status);

    return result;
}

void JSVMRuntime::setPropertyValue(
    const Object& obj,
    const PropNameID& name,
    const Value& value) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);

  JSVM_Value key = JSVMConverter::PointerValueToJSVM<PropNameID>(env, name);
  JSVM_Value object = JSVMConverter::PointerValueToJSVM<Object>(env, obj);
  JSVM_Value val = JSVMConverter::JsiToJSVM(env, value);
  auto status = OH_JSVM_SetProperty(env, object, key, val);
  // TODO: Exception
  ClearException(status);
}

void JSVMRuntime::setPropertyValue(const Object &obj, const String &name, const Value &value)
{
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);

  JSVM_Value key = JSVMConverter::PointerValueToJSVM<String>(env, name);
  JSVM_Value object = JSVMConverter::PointerValueToJSVM<Object>(env, obj);
  JSVM_Value val = JSVMConverter::JsiToJSVM(env, value);
  auto status = OH_JSVM_SetProperty(env, object, key, val);
  // TODO: Exception
  ClearException(status);
}

bool JSVMRuntime::isArray(const Object &value) const
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_value = 
      JSVMConverter::PointerValueToJSVM(env, value);

    bool result = false;
    JSVM_Status status = 
        OH_JSVM_IsArray(env, jsvm_value, &result);
    RNOH_ASSERT(status == JSVM_OK && "OH_JSVM_IsArray faild.");

    return result;
}

bool JSVMRuntime::isArrayBuffer(const Object &value) const
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_value = 
      JSVMConverter::PointerValueToJSVM(env, value);

    bool result = false;
    JSVM_Status status = 
        OH_JSVM_IsArraybuffer(env, jsvm_value, &result);
    RNOH_ASSERT(status == JSVM_OK && "OH_JSVM_IsArraybuffer faild.");

    return result;
}

bool JSVMRuntime::isFunction(const Object &value) const
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    const JSVM_Value jsvm_value = 
      JSVMConverter::PointerValueToJSVM(env, value);

    bool result = false;
    JSVM_Status status = 
        OH_JSVM_IsFunction(env, jsvm_value, &result);
    RNOH_ASSERT(status == JSVM_OK && "OH_JSVM_IsFunction faild.");

    return result;
}

bool JSVMRuntime::isHostObject(const Object &object) const
{
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);
  JSVM_Value jsObject = 
        JSVMConverter::PointerValueToJSVM(env, object);
  
  HostProxy *hostProxy = HostProxy::GetHostProxyFromJSVM(env, jsObject);

  if (!hostProxy) {
    ClearException();
    return false;
  }

  return hostProxy->IsHostObject();
}

bool JSVMRuntime::isHostFunction(const Function &func) const
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);
    JSVM_Value jsFunc =
        JSVMConverter::PointerValueToJSVM(env, func);

    HostProxy *hostProxy = HostProxy::GetHostProxyFromJSVM(env, jsFunc);

    return hostProxy && hostProxy->IsHostFunction();
}

Array JSVMRuntime::getPropertyNames(const Object& obj)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value props = nullptr;
    JSVM_Value object = JSVMConverter::PointerValueToJSVM(env, obj);
    CALL_JSVM(env, OH_JSVM_GetPropertyNames(env, object, &props));
    return JSVMConverter::make<Object>(env, props).getArray(*this); 
}

WeakObject JSVMRuntime::createWeakObject(const Object& obj) {
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = JSVMConverter::PointerValueToJSVM(env, obj);
    return make<WeakObject>(JSVMPointerValue::New<true>(env, value));
}

Value JSVMRuntime::lockWeakObject(const WeakObject & weakObj)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);
    JSVM_Value val = static_cast<const JSVMPointerValue *>(JSVMRuntime::getPointerValue(weakObj))->GetValue();
    bool isNull = true;
    OH_JSVM_IsNull(env, val, &isNull);
    if (isNull) {
        return Value::undefined();
    }

    return make<Object>(JSVMPointerValue::New<false>(env, val));
}

Array JSVMRuntime::createArray(size_t length)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);
    JSVM_Value ptr = nullptr;
    CALL_JSVM(env, OH_JSVM_CreateArrayWithLength(env, length, &ptr));
    return JSVMConverter::make<Object>(env, ptr).getArray(*this);
}

ArrayBuffer JSVMRuntime::createArrayBuffer(std::shared_ptr<MutableBuffer> buffer)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);
    JSVM_Value ptr = nullptr;
    void *arrayBufferPtr = nullptr;
    CALL_JSVM(env, OH_JSVM_CreateArraybuffer(env, buffer->size(), &arrayBufferPtr, &ptr));
    return JSVMConverter::make<Object>(env, ptr).getArrayBuffer(*this);
}

size_t JSVMRuntime::size(const Array &array)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = JSVMConverter::PointerValueToJSVM(env, array);
    uint32_t arrayLength = 0;
    CALL_JSVM(env, OH_JSVM_GetArrayLength(env, value, &arrayLength));

    return arrayLength;
}

size_t JSVMRuntime::size(const ArrayBuffer &arrayBuffer)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = JSVMConverter::PointerValueToJSVM(env, arrayBuffer);
    size_t arrayBufferLength = 0;
    CALL_JSVM(env, OH_JSVM_GetArraybufferInfo(env, value, nullptr, &arrayBufferLength));

    return arrayBufferLength;
}

uint8_t *JSVMRuntime::data(const ArrayBuffer &arrayBuffer)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = JSVMConverter::PointerValueToJSVM(env, arrayBuffer);
    void* data = nullptr;
    CALL_JSVM(env, OH_JSVM_GetArraybufferInfo(env, value, &data, nullptr));

    return reinterpret_cast<uint8_t *>(data);
}

Value JSVMRuntime::getValueAtIndex(const Array &array, size_t i)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value value = JSVMConverter::PointerValueToJSVM(env, array);
    JSVM_Value valueAtIndex = nullptr;
    uint32_t arrayLength = 0;
    CALL_JSVM(env, OH_JSVM_GetArrayLength(env, value, &arrayLength));
    if(i >= arrayLength) {
        throw JSError(*this, "Array index out of bounds");
    }
    CALL_JSVM(env, OH_JSVM_GetElement(env, value, i, &valueAtIndex));
    return JSVMConverter::JSVMToJsi(env, valueAtIndex);
}

void JSVMRuntime::setValueAtIndexImpl(const Array &array, size_t i, const Value &value)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value originValue = JSVMConverter::PointerValueToJSVM(env, array);
    JSVM_Value newValue = JSVMConverter::JsiToJSVM(env, value);
    uint32_t arrayLength = 0;
    CALL_JSVM(env, OH_JSVM_GetArrayLength(env, originValue, &arrayLength));
    if (i >= arrayLength) {
        throw JSError(*this, "Array index out of bounds");
    }
    CALL_JSVM(env, OH_JSVM_SetElement(env, originValue, (uint32_t)i, newValue));
}

Function JSVMRuntime::createFunctionFromHostFunction(
    const PropNameID& name,
    unsigned int paramCount,
    HostFunctionType func) {
  DFX();
  JSVMUtil::HandleScopeWrapper scope(env);

  auto funcName = utf8(name);
  HostFunctionProxy* proxy = new HostFunctionProxy(*this, func);

  JSVM_Value jsFunc = nullptr;
  OH_JSVM_CreateFunction(
      env, funcName.c_str(), funcName.size(), proxy->GetCallback(), &jsFunc);
  JSVM_PropertyDescriptor descriptor = {
      "length", nullptr, nullptr, nullptr, nullptr, nullptr, JSVM_DEFAULT};
  OH_JSVM_CreateDouble(env, paramCount, &(descriptor.value));
  CALL_JSVM_AND_THROW(OH_JSVM_DefineProperties(env, jsFunc, 1, &descriptor));
  OH_JSVM_Wrap(
      env,
      jsFunc,
      static_cast<void*>(proxy),
      [](JSVM_Env env, void* data, void* hint) {
        delete static_cast<HostFunctionProxy *>(data);
      },
      nullptr,
      nullptr);
  return JSVMConverter::make<Object>(env, jsFunc).getFunction(*this);
}

Value JSVMRuntime::call(const Function &func, const Value &jsThis, const Value *args, size_t count)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value funcValue = JSVMConverter::PointerValueToJSVM(env, func);
    JSVM_Value receiver = JSVMConverter::JsiToJSVM(env, jsThis);
    std::vector<JSVM_Value> argsValue;
    for (size_t i = 0; i < count; i++) {
        argsValue.push_back(JSVMConverter::JsiToJSVM(env, args[i]));
    }
    JSVM_Value result = nullptr;
    CALL_JSVM_AND_THROW(
        OH_JSVM_CallFunction(
            env, receiver, funcValue, count, argsValue.data(), &result));
    return JSVMConverter::JSVMToJsi(env, result);
}

Value JSVMRuntime::callAsConstructor(const Function &func, const Value *args, size_t count)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value funcValue = JSVMConverter::PointerValueToJSVM(env, func);
    std::vector<JSVM_Value> argsValue;
    for (size_t i = 0; i < count; i++) {
      argsValue.push_back(JSVMConverter::JsiToJSVM(env, args[i]));
    }
    JSVM_Value resultValue;
    CALL_JSVM(env, OH_JSVM_NewInstance(env, funcValue, count, argsValue.data(), &resultValue));
    return JSVMConverter::JSVMToJsi(env, resultValue);
}

bool JSVMRuntime::strictEqualsCommon(const Pointer& a, const Pointer& b) const {
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value lvalue = JSVMConverter::PointerValueToJSVM(env, a);
    JSVM_Value rvalue = JSVMConverter::PointerValueToJSVM(env, b);
    bool isStrictEquals = false;
    CALL_JSVM(env, OH_JSVM_StrictEquals(env, lvalue, rvalue, &isStrictEquals));

    return isStrictEquals;
}

bool JSVMRuntime::strictEquals(const Symbol &a, const Symbol &b) const
{
    DFX();
    return strictEqualsCommon(a, b);
}

bool JSVMRuntime::strictEquals(const BigInt &a, const BigInt &b) const
{
    DFX();
    return strictEqualsCommon(a, b);
}

bool JSVMRuntime::strictEquals(const String &a, const String &b) const
{
    DFX();
    return strictEqualsCommon(a, b);
}

bool JSVMRuntime::strictEquals(const Object &a, const Object &b) const
{
    DFX();
    return strictEqualsCommon(a, b);
}

bool JSVMRuntime::instanceOf(const Object &o, const Function &f)
{
    DFX();
    JSVMUtil::HandleScopeWrapper scope(env);

    JSVM_Value obj = JSVMConverter::PointerValueToJSVM<Object>(env, o);
    JSVM_Value type = JSVMConverter::PointerValueToJSVM(env, f);
    bool result = false;
    CALL_JSVM(env, OH_JSVM_Instanceof(env, obj, type, &result));

    return result;
}

inline std::string JSVMRuntime::GetValueString(JSVM_Value value) {
    constexpr size_t PREALLOC_SIZE = 256;
    char preallocMemory[PREALLOC_SIZE];

    char *buff = preallocMemory;
    size_t totalLen = 0;
    OH_JSVM_GetValueStringUtf8(env, value, nullptr, 0, &totalLen);
    size_t needed = totalLen + 1;

    if (needed > PREALLOC_SIZE) {
        buff = new char[needed];
    }

    OH_JSVM_GetValueStringUtf8(env, value, buff, needed, nullptr);

    std::string ret(buff, totalLen);

    if (needed > PREALLOC_SIZE) {
        delete[] buff;
    }

    return ret;
}

inline JSVM_Value JSVMRuntime::GetGlobalProperty(const char * str){
    JSVM_Value global = nullptr;
    CALL_JSVM(env, OH_JSVM_GetGlobal(env, &global));
    JSVM_Value jsvmValue = nullptr;
    JSVM_Value String = nullptr;
    CALL_JSVM(env, OH_JSVM_CreateStringUtf8(env, str, strlen(str), &String));
    CALL_JSVM(env, OH_JSVM_GetProperty(env, global, String, &jsvmValue));
    return jsvmValue;
}

inline JSVM_Value JSVMRuntime::CallGlobalFunction(
    const char* funcName,
    size_t length,
    JSVM_Value* args,
    uint32_t argc) {
  JSVM_Value global = nullptr;
  OH_JSVM_GetGlobal(env, &global);
  JSVM_Value JSFuncName = nullptr;
  OH_JSVM_CreateStringUtf8(env, funcName, length, &JSFuncName);
  JSVM_Value JSFunc = nullptr;
  CALL_JSVM_AND_THROW(OH_JSVM_GetProperty(env, global, JSFuncName, &JSFunc));
  bool isFunc = false;
  if (OH_JSVM_IsFunction(env, JSFunc, &isFunc) != JSVM_OK || !isFunc) {
    ThrowError();
  }

  JSVM_Value result;
  CALL_JSVM_AND_THROW(
      OH_JSVM_CallFunction(env, global, JSFunc, argc, args, &result));

  return result;
}

std::vector<uint8_t> JSVMRuntime::GetCodeCache(const std::string &sourceURL) {
  return GetCodeCacheL2(sourceURL);
}

void JSVMRuntime::UpdateCodeCache(const std::string &sourceURL, const std::vector<uint8_t>& buffer) {
  UpdateCodeCacheL1(sourceURL, buffer);
}

std::vector<uint8_t> JSVMRuntime::GetCodeCacheL1(const std::string &sourceURL) {
  try {
    std::ifstream file(sourceURL, std::ifstream::binary);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    DLOG(INFO) << "L1 CACHE HIT: " << sourceURL << "; size = " << size;
    UpdateCodeCacheL2(sourceURL, buffer);
    return buffer;
  } catch (...) {
    DLOG(INFO) << "L1 CACHE MISS: " << sourceURL;
    return std::vector<uint8_t>{};
  }
}

void JSVMRuntime::UpdateCodeCacheL1(
    const std::string& sourceURL,
    const std::vector<uint8_t>& buffer) {
  DLOG(INFO) << "Update L1 CACHE: " << sourceURL
             << "; size = " << buffer.size();

  namespace fs = std::filesystem;

  fs::path targetPath = sourceURL;
  fs::create_directories(targetPath.parent_path());

  std::string tmpSuffix = ".tmp_" + std::to_string(getpid()) + "_" +
      std::to_string(std::random_device{}());
  fs::path tmpPath = sourceURL + tmpSuffix;

  std::ofstream outFile(tmpPath, std::ios::binary | std::ios::out);
  if (outFile) {
    outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    outFile.close();

    if (!outFile.fail() && fs::file_size(tmpPath) == buffer.size()) {
      std::error_code ec_perm;
      fs::permissions(
          tmpPath,
          fs::perms::owner_write | fs::perms::group_write |
              fs::perms::others_write,
          fs::perm_options::remove,
          ec_perm);

      if (ec_perm) {
        LOG(ERROR) << "Set read-only failed for temp file: " << tmpPath;
        fs::remove(tmpPath);
        LOG(ERROR) << "Updating L1 CACHE failed: " << sourceURL;
        return;
      }

      std::error_code ec_rename;
      fs::rename(tmpPath, targetPath, ec_rename);

      if (!ec_rename) {
        DLOG(INFO) << "Updating L1 CACHE success: " << sourceURL;
        UpdateCodeCacheL2(sourceURL, buffer);
        return;
      }
      LOG(ERROR) << "Rename failed: " << ec_rename.message();
    } else {
      LOG(ERROR) << "Write incomplete to temp file: " << tmpPath;
    }
    fs::remove(tmpPath);
  } else {
    LOG(ERROR) << "Open temp file failed: " << tmpPath;
  }
  LOG(ERROR) << "Updating L1 CACHE failed: " << sourceURL;
}

std::vector<uint8_t> JSVMRuntime::GetCodeCacheL2(const std::string &sourceURL) {
  {
    std::lock_guard<std::mutex> lock(codeCacheMtx);
    if (auto it = codeCacheL2.find(sourceURL); it != codeCacheL2.end()) {
      DLOG(INFO) << "L2 CACHE HIT: " << sourceURL
                 << "; size = " << it->second.size();
      return it->second;
    }
    DLOG(INFO) << "L2 CACHE MISS: " << sourceURL;
  }
  return GetCodeCacheL1(sourceURL);
}

void JSVMRuntime::UpdateCodeCacheL2(const std::string &sourceURL, const std::vector<uint8_t>& buffer) {
  std::lock_guard<std::mutex> lock(codeCacheMtx);
  DLOG(INFO) << "Update L2 CACHE: " << sourceURL << "; size = " << buffer.size();
  codeCacheL2[sourceURL] = buffer;
}

void JSVMRuntime::ThrowError() {
  bool isPending = false;
  if (JSVM_OK == OH_JSVM_IsExceptionPending((env), &isPending) && isPending) {
    JSVM_Value error;
    if (JSVM_OK == OH_JSVM_GetAndClearLastException((env), &error)) {
      // 获取异常堆栈
      JSVM_Value stack = nullptr;
      CALL_JSVM(env, OH_JSVM_GetNamedProperty((env), error, "stack", &stack));

      JSVM_Value message = nullptr;
      CALL_JSVM(env, OH_JSVM_GetNamedProperty((env), error, "message", &message));

      std::string stackstr = stack? GetValueString(stack) : "";
      std::string messagestr = message? GetValueString(message) : "";

      throw JSError(*this, messagestr, stackstr);
    }
  }

  throw JSError(*this, "JSVMRuntime: unkown execption");
}

inline void JSVMRuntime::ClearException() const {
  JSVM_Value exception;
  OH_JSVM_GetAndClearLastException(env, &exception);
}

inline void JSVMRuntime::ClearException(JSVM_Status status) const {
  if (unlikely(status == JSVM_PENDING_EXCEPTION)) {
    JSVM_Value exception;
    OH_JSVM_GetAndClearLastException(env, &exception);
  }
}

}  // namespace rnjsvm
