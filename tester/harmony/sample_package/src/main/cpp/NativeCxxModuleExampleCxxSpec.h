/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

// NOTE: This entire file shoule be codegen'ed.
#pragma once

#include <ReactCommon/TurboModule.h>
#include <react/bridging/Bridging.h>
#include "RNOH/ArkTSTurboModule.h"
#include <map>

using namespace rnoh;
using namespace facebook;

struct ConstantsStruct {
  bool const1;
  int32_t const2;
  std::string const3;
  bool operator==(const ConstantsStruct& other) const {
    return const1 == other.const1 && const2 == other.const2 &&
        const3 == other.const3;
  }
};

struct ObjectStruct {
  std::string a;
  std::string b; 
  std::string c;
};

struct objectStruct {
  int32_t a;
  std::string b;  
};

struct ValueStruct  {
  double x;  
  std::string y;
  objectStruct z;
};

class JSI_EXPORT NativeCxxModuleExampleCxxSpecJSI : public TurboModule {
 public:
  NativeCxxModuleExampleCxxSpecJSI(
      const TurboModule::Context ctx,
      const std::string name);

  bool getBool(jsi::Runtime& rt, bool arg);

  int32_t getEnum(jsi::Runtime& rt, int32_t arg);

  double getNumber(jsi::Runtime& rt, double arg);

  std::string getString(jsi::Runtime& rt, std::string arg);

  jsi::String getValueWithCallback(
    jsi::Runtime& rt);

  ConstantsStruct getConstants(jsi::Runtime& rt);

  std::vector<std::optional<ObjectStruct>> getArray(
    jsi::Runtime& rt, 
    std::vector<std::optional<ObjectStruct>> arg);

  std::map<std::string,std::optional<int32_t>> getMap(
    jsi::Runtime& rt,
    std::map<std::string,
    std::optional<int32_t>> arg);

  objectStruct getObject(jsi::Runtime& rt, objectStruct arg);

  std::set<float> getSet(jsi::Runtime &rt, std::set<float> arg);

  ValueStruct getValue(jsi::Runtime& rt, 
     double x, std::string y, objectStruct z);

  std::string getUnion(jsi::Runtime& rt,float x,
     std::string y,jsi::Object z);

  void voidFunc(jsi::Runtime& rt);
};