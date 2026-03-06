/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */
#ifndef RNOHARKTSCOMPONENTDELEGATE_H
#define RNOHARKTSCOMPONENTDELEGATE_H
#pragma once
#include "ArkJS.h"

namespace rnoh {

class RNArkTSComponentDelegate {
  public:
  RNArkTSComponentDelegate(napi_env env, napi_value napiDelegateObject) : m_env(env) {
    ArkJS arkJS(m_env);
    m_arkTSComponentDelegateRef = arkJS.createReference(napiDelegateObject);
  };

  folly::dynamic callSync(const std::string &methodName, std::vector<ArkJS::IntermediaryArg> args) {
    folly::dynamic result;
    try {
      ArkJS arkJS(m_env);
      auto napiArgs = arkJS.convertIntermediaryValuesToNapiValues(std::move(args));
      auto napiDelegateObject = arkJS.getObject(m_arkTSComponentDelegateRef);
      auto napiResult = napiDelegateObject.call(methodName, napiArgs);
      result = arkJS.getDynamic(napiResult);
    } catch (const std::exception &e) {
      LOG(ERROR) << std::string("#RNOH::RNArkTSComponentDelegate::callSync (" + methodName + ") failed.").c_str();
    }
    return result;
  };

  ~RNArkTSComponentDelegate() {
    ArkJS arkJS(m_env);
    arkJS.deleteReference(m_arkTSComponentDelegateRef);
  }

  private:
  napi_env m_env;
  napi_ref m_arkTSComponentDelegateRef;
};

} // namespace rnoh

#endif